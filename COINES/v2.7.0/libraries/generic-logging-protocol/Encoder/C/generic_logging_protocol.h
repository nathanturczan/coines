/**
 * Copyright (C) 2020 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file        generic_logging_protocol.h
 *
 * @brief
 *
 */
 
/*!
 * @addtogroup generic_logging_protocol
 * @brief
 * @{*/

#ifndef GENERIC_LOGGING_PROTOCOL_H_
#define GENERIC_LOGGING_PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define GLP_USE_CSV

/**********************************************************************************/
/* header includes */
/**********************************************************************************/
#include <stdint.h>
#include <string.h>

#ifdef GLP_USE_CSV
#include <stdlib.h>
#include <stdio.h>
#endif

/**********************************************************************************/
/* macro definitions */
/**********************************************************************************/

/* copies BYTES bytes from SOURCE to MESSAGE,
 * proceeds the MESSAGE pointer by BYTES
 * and adds BYTES to ACCUMULATED_BYTES */
#define GLP_COPY_TO_MSG(MESSAGE, SOURCE, BYTES, ACCUMULATED_BYTES) \
memcpy((MESSAGE), (SOURCE), (BYTES)); \
(MESSAGE) += (BYTES); \
(ACCUMULATED_BYTES) += (BYTES)

/* copies BYTES bytes from MESSAGE to DESTINATION,
 * proceeds the MESSAGE pointer by BYTES
 * and adds BYTES to ACCUMULATED_BYTES */
#define GLP_COPY_FROM_MSG(DESTINATION, MESSAGE, BYTES, ACCUMULATED_BYTES) \
memcpy((DESTINATION), (MESSAGE), (BYTES)); \
(MESSAGE) += (BYTES); \
(ACCUMULATED_BYTES) += (BYTES)

/**********************************************************************************/
/* type definitions */
/**********************************************************************************/

#define GLP_VERSION                     (2)

#define GLP_MAX_CHANNELS                (32)    /* do not increase above 32! */
#define GLP_MAX_DESCRIPTION_LENGTH      (1024)

#define GLP_SUCCESS                             (0x00)
#define GLP_NO_ACTIVE_CHANNELS                  (0x08)
#define GLP_DECODE_START_BYTES_FAILURE          (0x40)
#define GLP_DECODE_STOP_BYTES_FAILURE           (0x80)

#define GLP_CHANNEL_NAME_LENGTH         (10)

typedef struct glp_channel_s
{
    char name[GLP_CHANNEL_NAME_LENGTH]; /* abbreviation of what's logged */
    char format_identifier; /* format identifier of the data type */
    char unit_identifier; /* unit identifier of the data type */
    void *ptr_to_data; /* pointer to the the data to log */
    double data; /* value of the data to log (doesn't have to be float) */
    int8_t data_size; /* width of the data in bytes */
} glp_channel_t;

typedef struct glp_meta_object_s
{
    /* channels active */
    uint32_t channel_active_bits;/* 1bit per channel */
    uint8_t channel_active_count;/* number of active channels */

    /* channels with data ready */
    uint32_t channel_dataready_bits;
    uint8_t channel_dataready_count;

    glp_channel_t channel[GLP_MAX_CHANNELS]; /* configurations for each channel */

    uint8_t packet_counter; /* used to detect missing packages */

    /* description/comment/additional information about the log */
    char description[GLP_MAX_DESCRIPTION_LENGTH];

    uint8_t status; /* status to detect run-time errors */
} glp_meta_object_t;

typedef enum glp_frame_source_enum_type
{
    GLP_FROM_STORED_DATA = 0,
    GLP_FROM_POINTERS,
} glp_frame_source_enum_t;

typedef enum glp_file_format_enum_type
{
    GLP_BINARY,
    GLP_CSV,
    GLP_JSON,
    GLP_XML
/* and so on */
} glp_file_format_enum_t;

/**********************************************************************************/
/* (extern) variable declarations */
/**********************************************************************************/

/**********************************************************************************/
/* function prototype declarations */
/**********************************************************************************/

/*!
 * @brief This API initializes a generic log object
 *
 * @param[out] log_obj   : Pointer to the log object
 *
 * @return None
 */
void glp_init(glp_meta_object_t *log_obj);

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
uint8_t glp_channel_init(glp_meta_object_t *log_obj, char *name, char format_identifier, char unit_identifier);

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
                                      char unit_identifier);

/*!
 * @brief This API returns the id of a channel by its name
 *
 * @param[in]  log_obj   : Pointer to the log object
 * @param[in]  name      : Name of the channel (abbreviation)
 *
 * @return Id of the matching channel
 */
uint8_t glp_channel_get_id_from_name(glp_meta_object_t *log_obj, char *name);

/*!
 * @brief This API disables a channel by its id
 *
 * @param[in]  id        : Id of the channel to be disabled
 *
 * @param[out] log_obj   : Pointer to the log object
 *
 * @return Active channels as bits
 */
uint32_t glp_channel_disable_by_id(glp_meta_object_t *log_obj, uint8_t id);

/*!
 * @brief This API disables a channel by its name
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- channel name
 *
 * @returns Active channels as bits
 * */
uint32_t glp_channel_disable_by_name(glp_meta_object_t *log_obj, char *name);

/*!
 * @brief This API updates the stored value in one channel
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] int8_t- channel id
 * @param[in] void-data
 *
 * @returns Active channels as bits
 * */
void glp_channel_update(glp_meta_object_t *log_obj, int8_t id, void *data);

/*!
 * @brief This API appends data to the description of the log object
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- description
 * @param[in] uint32_t-bytes_to_add
 *
 * @returns Number of bytes appended
 * */
uint32_t glp_description_add_bytes(glp_meta_object_t *log_obj, char *description, uint32_t bytes_to_add);

/*!
 * @brief This API appends a string to the description of the log object
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- description
 *
 * @returns Number of bytes appended
 * */
uint32_t glp_description_add_string(glp_meta_object_t *log_obj, char *description);

/*!
 * @brief This API encodes the header data of a log object in a message
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] uint8_t- header_msg
 *
 * @returns length of the message in bytes
 * */
uint32_t glp_header_pack_bin(glp_meta_object_t *log_obj, uint8_t *header_msg);

/*!
 * @brief This API decodes the header data from a message into a log object
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char- header_msg
 *
 * @returns length of the header message in bytes
 * */
uint32_t glp_header_unpack_bin(glp_meta_object_t *log_obj, char *header_msg);

/*!
 * @brief  glp_header_pack_csv
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char-header_lines
 *
 * @returns *header_lines
 * */
uint32_t glp_header_pack_csv(glp_meta_object_t *log_obj, char *header_msg);

/*!
 * @brief  glp_header_unpack_csv
 *
 * @param[in] glp_meta_object_t-log_obj
 * @param[in] char-header_msg
 *
 * @returns *header_msg
 * */
uint32_t glp_header_unpack_csv(glp_meta_object_t *log_obj, char *header_msg);

/*!
 * @brief This API calculates the maximum length (i.e. all active channels have dataready) of a frame message in bytes
 *
 * @param[in]  log_obj   : Pointer to the log object from which the frame should be created
 *
 * @return Maxmimum length of the message in bytes
 */
uint16_t glp_frame_calculate_bytes_max(glp_meta_object_t *log_obj);

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
uint32_t glp_frame_pack_bin_from_data(glp_meta_object_t *log_obj, uint8_t *frame_msg, uint64_t time);

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
                                              uint8_t selected_channels_count);

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
uint32_t glp_frame_pack_csv_from_data(glp_meta_object_t *log_obj, char *frame_msg, uint64_t time);

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
                                              uint8_t selected_channels_count);

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
uint32_t glp_frame_unpack(glp_meta_object_t *log_obj, uint8_t *frame_msg, uint64_t *time);

/* uses unpack and pack for conversion */

/**********************************************************************************/
/* inline function definitions */
/**********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GENERIC_LOGGING_PROTOCOL_H_ */

/** @}*/
