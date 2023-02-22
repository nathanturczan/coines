/**
 * Copyright (C) 2020 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file        generic_logging_protocol.c
 *
 * @brief
 *
 */
 
/*!
 * @addtogroup generic_logging_protocol
 * @brief
 * @{*/

/**********************************************************************************/
/* system header includes */
/**********************************************************************************/

/**********************************************************************************/
/* own header files */
/**********************************************************************************/

#include "generic_logging_protocol.h"

/**********************************************************************************/
/* local macro definitions */
/**********************************************************************************/

/**********************************************************************************/
/* constant definitions */
/**********************************************************************************/

/* header + frame start/stop as constants to allow access by their
 * address (as it is impossible to point on preprocessor defines) */
static const uint16_t glp_header_start = 0xCCCC;
static const uint16_t glp_header_stop = 0x3333;
static const uint16_t glp_frame_start = 0x9999;
static const uint16_t glp_frame_stop = 0x6666;

/**********************************************************************************/
/* global variables */
/**********************************************************************************/

/**********************************************************************************/
/* static variables */
/**********************************************************************************/

/**********************************************************************************/
/* static function declaration */
/**********************************************************************************/

/*!
 * @brief This internal API encodes the log data of a log object in a message according to the selected data source
 *
 * @param[in]  frame_source : Selected source for the frame (e.g. stored data or pointers)
 * @param[in]  log_obj   : Pointer to the log object from which the message will be created
 * @param[in]  time      : Time of the message creation
 *
 * @param[out] frame_msg : Pointer to the message buffer
 *
 * @return Length of the message in bytes
 */
static uint16_t glp_frame_pack_bin(glp_frame_source_enum_t frame_source, glp_meta_object_t *log_obj, uint8_t *frame_msg,
                                   uint64_t time)
{
    if (log_obj->channel_active_count == 0)
    {
        return 0;
    }

    uint32_t i;
    uint16_t total_bytes = 0, frame_length = 0;

    /* write message */
    {
        /* 2 frame start bytes */
        GLP_COPY_TO_MSG(frame_msg, &glp_frame_start, 2, total_bytes);
        /* skip frame length as it is unknown yet, but save the
         * position of the message pointer for later use */
        void *frame_length_pos = frame_msg;
        /* still move message pointer forward */
        frame_msg += 2;
        /* time stamp */
        GLP_COPY_TO_MSG(frame_msg, &time, 8, frame_length);
        /* packet counter */
        GLP_COPY_TO_MSG(frame_msg, &log_obj->packet_counter, 1, frame_length);
        log_obj->packet_counter++;
        /* channel dataready count in this frame */
        GLP_COPY_TO_MSG(frame_msg, &log_obj->channel_dataready_count, 1, frame_length);
        /* channel dataready bits in this frame */
        GLP_COPY_TO_MSG(frame_msg, &log_obj->channel_dataready_bits, 4, frame_length);

        /* data */
        for (i = 0; i < GLP_MAX_CHANNELS; i++)
        {
            /* check wether current channel (i) has data ready */
            if (log_obj->channel_dataready_bits & (1 << i))
            {
                switch (frame_source)
                {
                    case GLP_FROM_STORED_DATA:
                        GLP_COPY_TO_MSG(frame_msg, &log_obj->channel[i].data, log_obj->channel[i].data_size,
                                        frame_length)
                        ;
                        break;
                    case GLP_FROM_POINTERS:
                        GLP_COPY_TO_MSG(frame_msg, log_obj->channel[i].ptr_to_data, log_obj->channel[i].data_size,
                                        frame_length)
                        ;
                }
                /* unset dataready bit */
                log_obj->channel_dataready_bits &= ~(1 << i);
                /* decrease dataready count */
                log_obj->channel_dataready_count--;
            }
            if (log_obj->channel_dataready_count == 0)
            {
                break;
            }
        }
        /* add the frame length to the total byte count */
        total_bytes += frame_length;
        /* 2 frame stop bytes */
        GLP_COPY_TO_MSG(frame_msg, &glp_frame_stop, 2, total_bytes);
        /* insert the frame_length at the correct position in the message */
        GLP_COPY_TO_MSG(frame_length_pos, &frame_length, 2, total_bytes);
    }
    return total_bytes;
}

/*!
 * @brief This internal API decodes the log data of a log object in a message according to the selected data source
 *
 * @param[in] glp_frame_source_enum_t-frame_source
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char -frame_msg
 * @param[in] float -time
 *
 * @return *frame_msg_start
 */
static uint32_t glp_frame_pack_csv(glp_frame_source_enum_t frame_source, glp_meta_object_t *log_obj,
                                       char *frame_msg,
                                       uint64_t time)
{
    (void)time;
    if (log_obj->channel_active_count == 0)
    {
        log_obj->status = GLP_NO_ACTIVE_CHANNELS;
        return 0;
    }
    void *data;
    char *frame_msg_start = frame_msg;

    uint32_t id;

    /* write channel values */
    {
        /* channel dataready bits in this frame */
        frame_msg += sprintf(frame_msg, "%lu", log_obj->channel_dataready_bits);

        /* channels values */
        for (id = 0; id < GLP_MAX_CHANNELS; id++)
        {
            /* check wether current channel (i) has data ready */
            if (log_obj->channel_dataready_bits & (1 << id))
            {
                switch (frame_source)
                {
                    case GLP_FROM_STORED_DATA:
                        data = &log_obj->channel[id].data;
                        break;
                    case GLP_FROM_POINTERS:
                        data = log_obj->channel[id].ptr_to_data;

                }
                /* typecasting is necessary to prevent binary changes! */
                switch (log_obj->channel[id].format_identifier)
                {
                    /* FLOAT32 */
                    case 'f':
                        /* no casting necessary :) */
                        frame_msg += sprintf(frame_msg, "%f,", (float)log_obj->channel[id].data);
                        break;

                        /* SIGNED INT */
                    case 'b':
                        /* cast pointer to int8, dereference and lastly cast to int32 for printing */
                        frame_msg += sprintf(frame_msg, "%ld,", (int32_t) * (int8_t *)data);
                        break;
                    case 'h':
                        /* cast pointer to int16, dereference and lastly cast to int32 for printing */
                        frame_msg += sprintf(frame_msg, "%ld,", (int32_t) * (int16_t *)data);
                        break;
                    case 'i':
                        /* cast pointer to int32 and dereference */
                        frame_msg += sprintf(frame_msg, "%ld,", *(int32_t *)data);
                        break;

                        /* UNSIGNED INT */
                    case 'B':
                        /* cast pointer to uint8, dereference and lastly cast to uint32 for printing */
                        frame_msg += sprintf(frame_msg, "%lu,", (uint32_t) * (uint8_t *)data);
                        break;
                    case 'H':
                        /* cast pointer to uint16, dereference and lastly cast to uint32 for printing */
                        frame_msg += sprintf(frame_msg, "%lu,", (uint32_t) * (uint16_t *)data);
                        break;
                    case 'I':
                        /* cast pointer to uint32 and dereference */
                        frame_msg += sprintf(frame_msg, "%lu,", (uint32_t) * (uint32_t *)data);
                        break;

                        /* FLOAT16 */
                    case 'e':
                        // TODO: conversion from FLOAT16 to FLOAT32
                        frame_msg += sprintf(frame_msg, "%f,", (float)log_obj->channel[id].data);
                        break;
                        /* Unsigned long int */
                    case 'Q':
                        frame_msg += sprintf(frame_msg, "%llu,", (uint64_t) * (uint8_t *)data);
                        break;
                }
                /* unset dataready bit */
                log_obj->channel_dataready_bits &= ~(1 << id);
                /* decrease dataready count */
                log_obj->channel_dataready_count--;
            }
            if (log_obj->channel_dataready_count == 0)
            {
                break;
            }
        }
        /* append \n */
        *frame_msg++ = '\n';
    }
    /* append \0 to finish string */
    *frame_msg = '\0';

    return frame_msg - frame_msg_start;
}

/**********************************************************************************/
/* functions */
/**********************************************************************************/

/*!
 * @brief This API initializes a generic log object
 *
 * @param[out] log_obj   : Pointer to the log object
 *
 * @return None
 */
void glp_init(glp_meta_object_t *log_obj)
{
    log_obj->description[0] = '\0';
    log_obj->channel_active_bits = 0;
    log_obj->channel_active_count = 0;
    log_obj->channel_dataready_bits = 0;
    log_obj->channel_dataready_count = 0;
    log_obj->packet_counter = 0;
    log_obj->status = GLP_SUCCESS;
}

/**********************************************************************************/

/*!
 * @brief This API initializes one channel of log object
 *
 * @param[in]  name      : Name of the channel (abbreviation)
 * @param[in]  data_size : Size of the logged data in bytes
 * @param[in]  format_identifier : Used for decoding, to be written in the header
 *
 * @param[out] log_obj   : Pointer to the log object
 *
 * @return Id of used channel
 */
uint8_t glp_channel_init(glp_meta_object_t *log_obj, char *name, char format_identifier, char unit_identifier)
{
    return glp_channel_init_with_pointer(log_obj, name, NULL, format_identifier, unit_identifier);
}

/**********************************************************************************/

/*!
 * @brief This API initializes one channel of log object and sets the pointer
 * of the channel to the data to be logged
 *
 * @param[in]  name      : Name of the channel (abbreviation)
 * @param[in]  name      : Pointer to the date to be logged
 * @param[in]  data_size : Size of the logged data in bytes
 * @param[in]  format_identifier : Used for decoding, to be written in the header
 *
 * @param[out] log_obj   : Pointer to the log object
 *
 * @return Id of used channel
 */
uint8_t glp_channel_init_with_pointer(glp_meta_object_t *log_obj, char *name, void *ptr_to_data,
                                      char format_identifier,
                                      char unit_identifier)
{
    int32_t id;
    uint32_t channel_inactive_bits = ~log_obj->channel_active_bits;

    for (id = 0; id < GLP_MAX_CHANNELS; id++)
    {
        uint32_t length_of_name = strlen(name);
        if (length_of_name > GLP_CHANNEL_NAME_LENGTH)
        {
            length_of_name = GLP_CHANNEL_NAME_LENGTH;
        }
        /* use first free channel */
        if (channel_inactive_bits & (1 << id))
        {
            memcpy(log_obj->channel[id].name, name, length_of_name);
            log_obj->channel[id].ptr_to_data = ptr_to_data;
            /* set data to bytes to zero */
            memset(&log_obj->channel[id].data, 0x00, 4);

            /* set data size according to format identifier
             * https://docs.python.org/3/library/struct.html#format-characters */
            switch (format_identifier)
            {
                case 'b':
                    case 'B':
                    log_obj->channel[id].data_size = 1;
                    break;
                case 'h':
                    case 'H':
                    case 'e':
                    log_obj->channel[id].data_size = 2;
                    break;
                case 'i':
                    case 'I':
                    case 'f':
                    log_obj->channel[id].data_size = 4;
                    break;
                case 'Q':
                    log_obj->channel[id].data_size = 8;
                    break;
            }

            log_obj->channel[id].format_identifier = format_identifier;
            log_obj->channel[id].unit_identifier = unit_identifier;
            log_obj->channel_active_bits |= (1 << id);
            log_obj->channel_active_count++;
            /* fill free bytes with spaces */
            while (length_of_name < GLP_CHANNEL_NAME_LENGTH)
            {
                log_obj->channel[id].name[length_of_name++] = ' ';
            }
            /* return channel id */
            return id;
        }
    }
    /* all channels in use */
    return 0xFF;
}

/**********************************************************************************/

/*!
 * @brief This API returns the id of a channel by its name
 *
 * @param[in]  log_obj   : Pointer to the log object
 * @param[in]  name      : Name of the channel (abbreviation)
 *
 * @return Id of the matching channel
 */
uint8_t glp_channel_get_id_from_name(glp_meta_object_t *log_obj, char *name)
{
    uint32_t channel_active_processed_count;

    uint32_t i;
    /* channels */
    for (i = 0; i < GLP_MAX_CHANNELS; i++)
    {
        if (log_obj->channel_active_bits & (1 << i))
        {
            // FIXME: strcmpi?
            if (strcmp(log_obj->channel[i].name, name))
            {
                return i;
            }
            channel_active_processed_count++;
        }
        if (channel_active_processed_count == log_obj->channel_active_count)
        {
            break;
        }
    }
    return 0xFF;
}

/**********************************************************************************/

/*!
 * @brief This API disables a channel by its id
 *
 * @param[in]  id        : Id of the channel to be disabled
 *
 * @param[out] log_obj   : Pointer to the log object
 *
 * @return Active channels as bits
 */
uint32_t glp_channel_disable_by_id(glp_meta_object_t *log_obj, uint8_t id)
{
    log_obj->channel_active_bits &= ~(1 << id);
    log_obj->channel_active_count--;
    return log_obj->channel_active_bits;
}

/**********************************************************************************/

/*!
 * @brief This API disables a channel by its name
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- channel name
 *
 * @returns Active channels as bits
 * */
uint32_t glp_channel_disable_by_name(glp_meta_object_t *log_obj, char *name)
{
    uint8_t id = glp_channel_get_id_from_name(log_obj, name);
    return glp_channel_disable_by_id(log_obj, id);
}

/**********************************************************************************/

/*!
 * @brief This API updates the stored value in one channel
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] int8_t- channel id
 * @param[in] void-data
 *
 * @returns Active channels as bits
 * */
void glp_channel_update(glp_meta_object_t *log_obj, int8_t id, void *data)
{
    /* check channel active bit */
    if (log_obj->channel_active_bits & (1 << id))
    {
        /* copy data to the channel */
        memcpy(&log_obj->channel[id].data, data, log_obj->channel[id].data_size);
        /* set according dataready bit */
        log_obj->channel_dataready_bits |= (1 << id);
        /* increment dataready count */
        log_obj->channel_dataready_count++;
    }
}

/**********************************************************************************/

/*!
 * @brief This API appends data to the description of the log object
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- description
 * @param[in] uint32_t-bytes_to_add
 *
 * @returns Number of bytes appended
 * */
uint32_t glp_description_add_bytes(glp_meta_object_t *log_obj, char *description, uint32_t bytes_to_add)
{
    uint32_t bytes_current = (uint32_t)strlen(log_obj->description);

    if ((bytes_current + bytes_to_add) > GLP_MAX_DESCRIPTION_LENGTH)
    {
        /* reserve 1 byte for '\0' */
        bytes_to_add = GLP_MAX_DESCRIPTION_LENGTH - bytes_current - 1;
    }
    /* copy bytes_to_add bytes to the end of the current description */
    memcpy(log_obj->description + bytes_current, description, bytes_to_add);
    log_obj->description[bytes_current + bytes_to_add] = '\0';
    return bytes_to_add;
}

/**********************************************************************************/

/*!
 * @brief This API appends a string to the description of the log object
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- description
 *
 * @returns Number of bytes appended
 * */
uint32_t glp_description_add_string(glp_meta_object_t *log_obj, char *description)
{
    uint32_t bytes_current = (uint32_t)strlen(log_obj->description);
    uint32_t bytes_to_add = (uint32_t)strlen(description);

    /* append to current description if there is enough free space remaining */
    if ((bytes_current + bytes_to_add) < GLP_MAX_DESCRIPTION_LENGTH)
    {
        strcat(log_obj->description, description);
    }
    else
    {
        /* there is not enough free space remaining
         * add_bytes takes over */
        bytes_to_add = glp_description_add_bytes(log_obj, description, bytes_to_add);
    }
    return bytes_to_add;
}

/**********************************************************************************/

/*!
 * @brief This API encodes the header data of a log object in a message
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] uint8_t- header_msg
 *
 * @returns length of the message in bytes
 * */
uint32_t glp_header_pack_bin(glp_meta_object_t *log_obj, uint8_t *header_msg)
{
    uint32_t active_channels_processed = 0;
    uint32_t total_bytes = 0;
    uint32_t version = GLP_VERSION;

    /* 2 header start bytes */
    GLP_COPY_TO_MSG(header_msg, &glp_header_start, 2, total_bytes);
    /* protocol version */
    GLP_COPY_TO_MSG(header_msg, &version, 1, total_bytes);
    /* number of active channels */
    GLP_COPY_TO_MSG(header_msg, &log_obj->channel_active_count, 1, total_bytes);

    uint32_t i;
    /* channels */
    for (i = 0; i < GLP_MAX_CHANNELS; i++)
    {
        if (log_obj->channel_active_bits & (1 << i))
        {
            /* channel id */
            GLP_COPY_TO_MSG(header_msg, (uint8_t* )&i, 1, total_bytes);
            /* channel name */
            GLP_COPY_TO_MSG(header_msg, &log_obj->channel[i].name, GLP_CHANNEL_NAME_LENGTH, total_bytes);
            /* channel format identifier */
            GLP_COPY_TO_MSG(header_msg, &log_obj->channel[i].format_identifier, 1, total_bytes);
            /* channel unit */
            GLP_COPY_TO_MSG(header_msg, &log_obj->channel[i].unit_identifier, 1, total_bytes);
            active_channels_processed++;
        }
        if (active_channels_processed == log_obj->channel_active_count)
        {
            break;
        }
    }
    /* description size */
    uint16_t description_size = (uint16_t)strlen(log_obj->description);
    GLP_COPY_TO_MSG(header_msg, &description_size, 2, total_bytes);
    /* description itself */
    if (description_size > 0)
    {
        GLP_COPY_TO_MSG(header_msg, &log_obj->description, description_size, total_bytes);
    }
    /* 2 header stop bytes */
    GLP_COPY_TO_MSG(header_msg, &glp_header_stop, 2, total_bytes);
    return total_bytes;
}

/**********************************************************************************/

/*!
 * @brief This API decodes the header data from a message into a log object
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- header_msg
 *
 * @returns length of the header message in bytes
 * */
uint32_t glp_header_unpack_bin(glp_meta_object_t *log_obj, char *header_msg)
{
    /* check header start bytes */
    if (memcmp(header_msg, &glp_header_start, 2))
    {
        log_obj->status |= GLP_DECODE_START_BYTES_FAILURE;
    }
    /* proceed pointer by 2 header start bytes and 1 version byte */
    header_msg += 3;
    uint32_t bytes_read = 3;
    /* copy number of active channels and proceed pointer */
    GLP_COPY_FROM_MSG(&log_obj->channel_active_count, header_msg, 1,
                      bytes_read);
    /* decode channels */
    {
        uint32_t i;
        uint8_t id;
        for (i = 0; i < log_obj->channel_active_count; i++)
        {
            /* get channel id */
            GLP_COPY_FROM_MSG(&id, header_msg, 1, bytes_read);
            /* get channel name */
            GLP_COPY_FROM_MSG(&log_obj->channel[id].name, header_msg, GLP_CHANNEL_NAME_LENGTH, bytes_read);
            /* get channel format identifier */
            GLP_COPY_FROM_MSG(&log_obj->channel[id].format_identifier, header_msg, 1, bytes_read);
            /* get channel unit identifier */
            GLP_COPY_FROM_MSG(&log_obj->channel[id].unit_identifier, header_msg, 1, bytes_read);
            /* set channel active bit */
            log_obj->channel_active_bits |= (1 << id);
            /* set data size according to format identifier
             * https://docs.python.org/3/library/struct.html#format-characters */
            switch (log_obj->channel[id].format_identifier)
            {
                case 'b':
                    case 'B':
                    log_obj->channel[id].data_size = 1;
                    break;
                case 'h':
                    case 'H':
                    case 'e':
                    log_obj->channel[id].data_size = 2;
                    break;
                case 'i':
                    case 'I':
                    case 'f':
                    log_obj->channel[id].data_size = 4;
                    break;
                case 'Q':
                    log_obj->channel[id].data_size = 8;
                    break;
            }
        }
    }
    uint16_t description_size;
    /* get description size */
    GLP_COPY_FROM_MSG(&description_size, header_msg, 2, bytes_read);
    /* get description if size is greater zero */
    if (description_size)
    {
        GLP_COPY_FROM_MSG(log_obj->description, header_msg, description_size, bytes_read);
        log_obj->description[description_size] = '\0';
    }
    /* check header stop bytes */
    if (memcmp(header_msg, &glp_header_stop, 2))
    {
        log_obj->status |= GLP_DECODE_STOP_BYTES_FAILURE;
    }

    return (bytes_read + 2);
}

/**********************************************************************************/

/*!
 * @brief  glp_header_pack_csv
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char-header_lines
 *
 * @returns *header_lines
 * */
uint32_t glp_header_pack_csv(glp_meta_object_t *log_obj, char *header_lines)
{
    uint32_t active_channels_processed = 0;
    char *start = header_lines;

    uint32_t id, id_max;
    /* write first line containing additional header information */
    {
        /* write protocol version and number of active channels */
        header_lines += sprintf(header_lines, "# v%d,%d,", GLP_VERSION, log_obj->channel_active_count);
        for (id = 0; id < GLP_MAX_CHANNELS; id++)
        {
            if (log_obj->channel_active_bits & (1 << id))
            {
                /* write channel id and according format identifier */
                header_lines += sprintf(header_lines, "%d%c,",(unsigned int)id, log_obj->channel[id].format_identifier);
                active_channels_processed++;
            }
            if (active_channels_processed == log_obj->channel_active_count)
            {
                id_max = id;
                break;
            }
        }
        /* write description */
        header_lines += sprintf(header_lines, "%s\n", log_obj->description);
    }
    /* write column names */
    {
        char unit[4];
        /* first col contains data dataready bits as uint32_teger */
        header_lines += sprintf(header_lines, "%s,", "dataready");
        for (id = 0; id <= id_max; id++)
        {
            if (log_obj->channel_active_bits & (1 << id))
            {
                /* write channel name, 9 bytes, because snprintf will automatically add '\0' */
                memcpy(header_lines, log_obj->channel[id].name, GLP_CHANNEL_NAME_LENGTH);
                header_lines += GLP_CHANNEL_NAME_LENGTH;
                /* resolve unit from its identifier */
                switch (log_obj->channel[id].unit_identifier)
                {
                    case 'c':
                        memcpy(&unit, "degC\0", 3);
                        break;
                    case 'd':
                        memcpy(&unit, "dps\0", 3);
                        break;
                    case 'g':
                        memcpy(&unit, "g\0", 3);
                        break;
                    case 'l':
                        memcpy(&unit, "lsb\0", 3);
                        break;
                    case 'p':
                        memcpy(&unit, "hPa\0", 3);
                        break;
                    case 'h':
                        memcpy(&unit, "%\0", 3);
                        break;
                    case 't':
                        memcpy(&unit, "uT\0", 3);
                        break;
                    case 'm':
                        memcpy(&unit, "mm\0", 3);
                        break;
                    case 'u':
                        memcpy(&unit, "ms\0", 3);
                        break;
                }
                header_lines += sprintf(header_lines, " [%s],", unit);
            }
        }
        /* append \n */
        *header_lines++ = '\n';
    }
    /* append \0 to finish string */
    *header_lines = '\0';
    return header_lines - start;
}

/**********************************************************************************/

/*!
 * @brief  glp_header_unpack_csv
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char-header_msg
 *
 * @returns *header_msg
 * */
uint32_t glp_header_unpack_csv(glp_meta_object_t *log_obj, char *header_msg)
{
    char *header_msg_start = header_msg;
    uint32_t temp_uint = 0;

    uint32_t id, id_max;
    char temp_char;
    int32_t bytes_read = 0;
    /* write first line containing addditional header information */
    {
        uint32_t i;
        /* skip version by scanning for first commata, then skip anything not being numerical (in case there is),
         * lastly scan active channel count */
        sscanf(header_msg, "%*[^,]%*[^0-9]%u%n",(unsigned int *) &temp_uint, (int *)&bytes_read);
        header_msg += bytes_read;
        log_obj->channel_active_count = (uint8_t)temp_uint;
        for (i = 0; i < log_obj->channel_active_count; i++)
        {
            sscanf(header_msg, ",%u%c%n", (unsigned int *)&id, &temp_char, (int *)&bytes_read);
            header_msg += bytes_read;
            log_obj->channel_active_bits |= (1 << id);
            log_obj->channel[id].format_identifier = temp_char;
        }
        id_max = id;
        /* scan description */
        sscanf(header_msg, ",%63[^\n]%n", log_obj->description, (int *)&bytes_read);
        header_msg += bytes_read;
        log_obj->description[bytes_read] = '\0';
    }
    /* scan column names */
    {
        char unit[4], name[9];
        //uint32_t id;
        /* skip datarready column */
        sscanf(header_msg, "\n%*[^,]%n", (int *)&bytes_read);
        header_msg += bytes_read;
        for (id = 0; id <= id_max; id++)
        {
            if (log_obj->channel_active_bits & (1 << id))
            {
                /* scan name, anything not being an opening bracket and at last the unit in between brackets */
                sscanf(header_msg, ",%8[^[]" "%*[^[]" "[%3[^]]]%n", name, unit, (int *)&bytes_read);
                header_msg += bytes_read;
                /* copy name to channel */
                memcpy(&log_obj->channel[id].name, &name, GLP_CHANNEL_NAME_LENGTH);
                /* resolve unit identifier */
                if (strncmp(unit, "lsb", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'l';
                }
                else if (strncmp(unit, "g", 2) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'g';
                }
                else if (strncmp(unit, "dps", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'd';
                }
                else if (strncmp(unit, "hPa", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'p';
                }
                else if (strncmp(unit, "degC", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'c';
                }
                else if (strncmp(unit, "%", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'h';
                }
                else if (strncmp(unit, "uT", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 't';
                }
                else if (strncmp(unit, "mm", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'm';
                }
                else if (strncmp(unit, "ms", 3) == 0)
                {
                    log_obj->channel[id].unit_identifier = 'u';
                }
                else
                {
                    log_obj->channel[id].unit_identifier = '\0';
                }
            }
        }
    }
    return header_msg - header_msg_start;
}

/**********************************************************************************/

/*!
     * @brief This API calculates the maximum length (i.e. all active channels have dataready) of a frame message in bytes
     *
     * @param[in]  log_obj   : Pointer to the log object from which the frame should be created
     *
     * @return Maxmimum length of the message in bytes
     */
uint16_t glp_frame_calculate_bytes_max(glp_meta_object_t *log_obj)
{
    uint16_t frame_bytes_max = 0;
    uint8_t channel_active_left = log_obj->channel_active_count;

    frame_bytes_max += (
                       2 /* frame start bytes */
                       + 2 /* frame length bytes */
                       + 8 /* time stamp bytes */
                       + 2 /* packet count bytes */
                       + 1 /* active channel count */
                       + 4 /* active channel bits bytes */
                       //            + 4               /* crc bytes */
                       + 2); /* frame stop bytes */

    uint32_t i;
    for (i = 0; i < GLP_MAX_CHANNELS; i++)
    {
        /* check wether current channel (i) is a active one */
        if (log_obj->channel_active_bits & (1 << i))
        {
            /* deacrease number of active channels left */
            channel_active_left--;
            /* add data_size to frame byte counter */
            frame_bytes_max += log_obj->channel[i].data_size;
        }
        /* break if there are no active channels left */
        if (channel_active_left == 0)
        {
            break;
        }
    }
    return frame_bytes_max;
}

/**********************************************************************************/

/*!
 * @brief This API encodes the log data of a log object in a message
 *
 * @param[in]  log_obj   : Pointer to the log object from which the message will be created
 * @param[in]  time      : Time of the message creation
 *
 * @param[out] frame_msg : Pointer to the message buffer
 *
 * @return Length of the message in bytes
 */
uint32_t glp_frame_pack_bin_from_data(glp_meta_object_t *log_obj, uint8_t *frame_msg, uint64_t time)
{
    return glp_frame_pack_bin(GLP_FROM_STORED_DATA, log_obj, frame_msg, time);
}

/**********************************************************************************/

/*!
 * @brief  This API encodes the data from the data pointers in the channels of a log object in a message
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] uint8_t-frame_msg
 * @param[in] float-time
 * @param[in] uint32_t-selected_channels_bits
 * @param[in] uint8_t-selected_channels_count
 *
 * @return uint16_t glp_frame_pack_bin
 * */
uint32_t glp_frame_pack_bin_from_pointers(glp_meta_object_t *log_obj, uint8_t *frame_msg, uint64_t time,
                                              uint32_t selected_channels_bits,
                                              uint8_t selected_channels_count)
{
    log_obj->channel_dataready_bits |= selected_channels_bits;
    log_obj->channel_dataready_count += selected_channels_count;
    return glp_frame_pack_bin(GLP_FROM_POINTERS, log_obj, frame_msg, time);
}

/**********************************************************************************/

/*!
 * @brief This API decodes the log data from a message into a log object
 *
 * @param[in]  frame_msg : Pointer to the message buffer
 * @param[in]  time      : Pointer to the variable where the time of the message creation will be stored
 *
 * @param[out] log_obj   : Pointer to the log object where the data will be stored
 *
 * @return Number of bytes read
 */
uint32_t glp_frame_unpack(glp_meta_object_t *log_obj, uint8_t *frame_msg, uint64_t *time)
{
    /* check header start bytes */
    if (memcmp(frame_msg, &glp_frame_start, 2))
    {
        log_obj->status |= GLP_DECODE_START_BYTES_FAILURE;
    }
    /* proceed pointer by 2 header start bytes */
    frame_msg += 2;
    /* directly add start bytes */
    uint32_t bytes_read = 2;

    uint16_t frame_length;
    /* get the frame length */
    GLP_COPY_FROM_MSG(&frame_length, frame_msg, 2, bytes_read);
    /* check header stop bytes */
    if (memcmp(frame_msg + frame_length, &glp_frame_stop, 2))
    {
        log_obj->status |= GLP_DECODE_STOP_BYTES_FAILURE;
    }
    bytes_read += 2;
    /* get the time stamp */
    GLP_COPY_FROM_MSG(time, frame_msg, 8, bytes_read);
    /* get the packet count */
    GLP_COPY_FROM_MSG(&log_obj->packet_counter, frame_msg, 1, bytes_read);
    /* get the channel active count */
    GLP_COPY_FROM_MSG(&log_obj->channel_dataready_count, frame_msg, 1, bytes_read);
    /* get the channel dataready bits */
    GLP_COPY_FROM_MSG(&log_obj->channel_dataready_bits, frame_msg, 4, bytes_read);

    uint8_t channel_dataready_left = log_obj->channel_dataready_count;
    /* decode the channel data */
    {
        uint32_t i;
        for (i = 0; i < GLP_MAX_CHANNELS; i++)
        {
            /* check wether current channel (i) is a active one */
            if ((log_obj->channel_active_bits & (1 << i))
                && (log_obj->channel_dataready_bits & (1 << i)))
            {
                /* get the channel's data from the message */
                GLP_COPY_FROM_MSG(&log_obj->channel[i].data, frame_msg, log_obj->channel[i].data_size, bytes_read);
                /* deacrease number of active channels left */
                channel_dataready_left--;
                if (channel_dataready_left == 0)
                {
                    break;
                }
            }
        }
    }
    // TODO: CRC
    return bytes_read;
}

/**********************************************************************************/

/*!
 * @brief This API decodes the log data of a log object in a message
 *
 * @param[in]  log_obj   : Pointer to the log object from which the message will be created
 * @param[in]  time      : Time of the message creation
 *
 * @param[out] frame_msg : Pointer to the message buffer
 *
 * @return uint32_t glp_frame_pack_csv
 */
uint32_t glp_frame_pack_csv_from_data(glp_meta_object_t *log_obj, char *frame_msg, uint64_t time)
{
    return glp_frame_pack_csv(GLP_FROM_STORED_DATA, log_obj, frame_msg, time);
}

/**********************************************************************************/

/*!
 * @brief This API decodes the data from the data pointers in the channels of a log object in a message
 *
 * @param[in]  log_obj   : Pointer to the log object from which the message will be created
 * @param[in]  time      : Time of the message creation
 * @param[in]  selected_channels_bits : Selected channels bit encoded
 * @param[in]  selected_channels_count : Number of selected channels
 *
 * @param[out] frame_msg : Pointer to the message buffer
 *
 * @return uint32_t glp_frame_pack_csv
 */
uint32_t glp_frame_pack_csv_from_pointers(glp_meta_object_t *log_obj, char *frame_msg, uint64_t time,
                                              uint32_t selected_channels_bits,
                                              uint8_t selected_channels_count)
{
    log_obj->channel_dataready_bits |= selected_channels_bits;
    log_obj->channel_dataready_count += selected_channels_count;
    return glp_frame_pack_csv(GLP_FROM_POINTERS, log_obj, frame_msg, time);
}

/** @}*/
