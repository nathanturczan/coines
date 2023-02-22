/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#include "coines_bridge.h"
#include "serial_com.h"
#include "coines.h"

/* Static variables */
static uint8_t resp_buffer[COINES_BUFFER_SIZE] = { 0 };
static uint8_t cmd_packet[COINES_BUFFER_SIZE] = { 0 };
static uint16_t cmd_packet_size;
/*! Variable to hold sensor count */
static uint8_t coines_sensor_id_count = 0;
static int8_t scom_read_status = SCOM_OK;
static int8_t scom_write_status = SCOM_OK;

/* Static functions */
static int16_t coines_connect_usb(void);
static int16_t coines_disconnect(void);
static int16_t coines_send_packet(uint8_t command, uint8_t *payload, uint16_t length);

/* Global variable */
typedef struct
{
    uint16_t no_of_sensors_enabled; /**< Number of sensors enabled */
    uint16_t sensors_byte_count[COINES_MAX_SENSOR_COUNT]; /**< Sensor byte count */
} comm_stream_info_t;

static comm_stream_info_t coines_sensor_info;

struct coines_streaming_settings
{
    uint8_t channel_id; /*< streaming channel 1/2 */
    struct coines_streaming_config stream_config; /*< streaming config */
    struct coines_streaming_blocks data_blocks; /*< streaming data blocks */
};

/*! variable to hold the maximum no of streaming configuration buffer*/
struct coines_streaming_settings coines_streaming_cfg_buf[COINES_MAX_SENSOR_COUNT];

static enum coines_comm_intf comm_intf = COINES_COMM_INTF_USB;

static int16_t coines_send_multi_packet(uint8_t command,
                                        uint8_t *payload_header,
                                        uint16_t header_length,
                                        uint8_t *payload_body,
                                        uint16_t body_length)
{
    int8_t ret = SCOM_OK;
    uint16_t bytes_to_write = 0;

    if ((header_length != 0) && (payload_header == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    cmd_packet_size = header_length + body_length + 4;

    cmd_packet[COINES_PROTO_HEADER_POS] = COINES_CMD_HEADER;
    memcpy(&cmd_packet[COINES_PROTO_LENGTH_POS], &cmd_packet_size, 2);
    cmd_packet[COINES_PROTO_CMD_POS] = command;
    if (header_length != 0)
    {
        memcpy(&cmd_packet[COINES_PROTO_PAYLOAD_POS], payload_header, header_length);
        if (payload_body && body_length)
        {
            memcpy(&cmd_packet[COINES_PROTO_PAYLOAD_POS + header_length], payload_body, body_length);
        }
    }

    for (uint16_t pi = 0; pi < cmd_packet_size; pi += bytes_to_write)
    {
        if (cmd_packet_size < COINES_PACKET_SIZE)
        {
            bytes_to_write = cmd_packet_size;
        }
        else if ((cmd_packet_size - pi) < COINES_PACKET_SIZE)
        {
            bytes_to_write = cmd_packet_size - pi;
        }
        else
        {
            bytes_to_write = COINES_PACKET_SIZE;
        }

        coines_write_intf(comm_intf, &cmd_packet[pi], bytes_to_write);

        if (scom_write_status != SCOM_OK)
        {
            /* Assumes that the device recovers from the partial packet
             * with a timeout
             */
            return COINES_E_COMM_IO_ERROR;
        }
    }

    return ret;
}

static int16_t coines_send_packet(uint8_t command, uint8_t *payload, uint16_t length)
{
    return coines_send_multi_packet(command, payload, length, NULL, 0);
}

static int16_t coines_receive_resp(uint8_t command, uint16_t *resp_length)
{
    int8_t ret = COINES_SUCCESS;
    uint32_t bytes_available;
    uint32_t packet_idx = 0;
    uint16_t packet_length = 0;

    packet_idx = 0;
    do
    {
        bytes_available = coines_read_intf(comm_intf, &resp_buffer[packet_idx], 3);
        if (scom_read_status == SCOM_OK)
        {
            
            if (bytes_available)
            {
                packet_idx += bytes_available;
            }
        }
        else
        {
            return COINES_E_COMM_IO_ERROR;
        }
    } while (packet_idx < 3);
    
    memcpy(&packet_length, &resp_buffer[COINES_PROTO_LENGTH_POS], 2);

    if (packet_length)
    {
        do
        {
            bytes_available = coines_read_intf(comm_intf, &resp_buffer[packet_idx], packet_length - packet_idx);
            if (scom_read_status == SCOM_OK)
            {
                if (bytes_available)
                {
                    packet_idx += bytes_available;
                }
            }
            else
            {
                return COINES_E_COMM_IO_ERROR;
            }
        } while (packet_idx < packet_length);

        if (resp_buffer[COINES_PROTO_HEADER_POS] == COINES_RESP_OK_HEADER)
        {
            *resp_length = packet_length - COINES_PROTO_PAYLOAD_POS;
            if (resp_buffer[COINES_PROTO_CMD_POS] != command)
            {
                return COINES_E_COMM_WRONG_RESPONSE;
            }
        }
        else if (resp_buffer[COINES_PROTO_HEADER_POS] == COINES_RESP_NOK_HEADER)
        {
            if (resp_buffer[COINES_PROTO_PAYLOAD_POS] != COINES_SUCCESS)
            {
                return (int8_t)resp_buffer[COINES_PROTO_PAYLOAD_POS];
            }
            else
            {
                return COINES_E_FAILURE;
            }
        }
        else
        {
            return COINES_E_COMM_WRONG_RESPONSE;
        }
    }

    return ret;
}

int16_t coines_open_comm_intf(enum coines_comm_intf intf_type, void *arg)
{
    (void)arg;
    switch (intf_type)
    {
        case COINES_COMM_INTF_USB:
            return coines_connect_usb();
            break;
        case COINES_COMM_INTF_VCOM:
            return -1; /* TODO */
            break;
        case COINES_COMM_INTF_BLE:
            return -1; /* TODO */
            break;
        default:
            break;
    }

    return SCOM_OK;
}

int16_t coines_close_comm_intf(enum coines_comm_intf intf_type, void *arg)
{
    (void)arg;
    switch (intf_type)
    {
        case COINES_COMM_INTF_USB:
            return coines_disconnect();
            break;
        case COINES_COMM_INTF_VCOM:
            return -1; /* TODO */
            break;
        case COINES_COMM_INTF_BLE:
            return -1; /* TODO */
            break;
        default:
            break;
    }

    return SCOM_OK;
}

int16_t coines_echo_test(uint8_t *data, uint16_t length)
{
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_ECHO, data, length);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_ECHO, &resp_length);
    }

    if ((ret == COINES_SUCCESS) && (memcmp(data, &resp_buffer[COINES_PROTO_PAYLOAD_POS], length)))
    {
        return COINES_E_COMM_WRONG_RESPONSE;
    }

    return ret;
}

int16_t coines_get_board_info(struct coines_board_info *board_info)
{
    int8_t ret = SCOM_OK;
    uint16_t resp_length = 0;

    if (board_info == NULL)
    {
        return COINES_E_NULL_PTR;
    }

    ret = coines_send_packet(COINES_CMD_ID_GET_BOARD_INFO, NULL, 0);

    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_GET_BOARD_INFO, &resp_length);
    }

    if (ret == COINES_SUCCESS)
    {
        memcpy(&board_info->hardware_id, &resp_buffer[COINES_PROTO_PAYLOAD_POS], 2);
        memcpy(&board_info->software_id, &resp_buffer[COINES_PROTO_PAYLOAD_POS + 2], 2);
        memcpy(&board_info->board, &resp_buffer[COINES_PROTO_PAYLOAD_POS + 4], 1);
        memcpy(&board_info->shuttle_id, &resp_buffer[COINES_PROTO_PAYLOAD_POS + 5], 2);
    }

    return ret;
}

int16_t coines_set_pin_config(enum coines_multi_io_pin pin_number,
                              enum coines_pin_direction direction,
                              enum coines_pin_value pin_value)
{
    uint8_t payload[3] = { pin_number, direction, pin_value };
    int8_t ret;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_SET_PIN, payload, 3);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SET_PIN, &resp_length);
    }

    (void)resp_length;

    return ret;
}

int16_t coines_get_pin_config(enum coines_multi_io_pin pin_number,
                              enum coines_pin_direction *pin_direction,
                              enum coines_pin_value *pin_value)
{
    uint8_t payload[3] = { pin_number, *pin_direction, *pin_value };
    int8_t ret;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_GET_PIN, payload, 3);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_GET_PIN, &resp_length);
    }

    if ((resp_length >= 3) && (pin_number == resp_buffer[COINES_PROTO_PAYLOAD_POS]))
    {
        *pin_direction = resp_buffer[COINES_PROTO_PAYLOAD_POS + 1];
        *pin_value = resp_buffer[COINES_PROTO_PAYLOAD_POS + 2];
    }
    else
    {
        return COINES_E_COMM_WRONG_RESPONSE;
    }

    return ret;
}

int16_t coines_set_shuttleboard_vdd_vddio_config(uint16_t vdd_millivolt, uint16_t vddio_millivolt)
{
    uint8_t payload[4] = { 0 };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    memcpy(payload, &vdd_millivolt, 2);
    memcpy(&payload[2], &vddio_millivolt, 2);

    ret = coines_send_packet(COINES_CMD_ID_SET_VDD_VDDIO, payload, 4);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SET_VDD_VDDIO, &resp_length);
    }

    return ret;
}

int16_t coines_config_spi_bus(enum coines_spi_bus bus, enum coines_spi_speed spi_speed, enum coines_spi_mode spi_mode)
{
    uint8_t payload[3] = { bus, spi_speed, spi_mode };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_SPI_CONFIG, payload, 3);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SPI_CONFIG, &resp_length);
    }

    return ret;
}

int16_t coines_deconfig_spi_bus(enum coines_spi_bus bus)
{
    uint8_t payload = { bus };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_SPI_DECONFIG, &payload, 1);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SPI_DECONFIG, &resp_length);
    }

    return ret;
}

int16_t coines_config_word_spi_bus(enum coines_spi_bus bus,
                                   enum coines_spi_speed spi_speed,
                                   enum coines_spi_mode spi_mode,
                                   enum coines_spi_transfer_bits spi_transfer_bits)
{
    uint8_t payload[4] = { bus, spi_speed, spi_mode, spi_transfer_bits };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_SPI_WORD_CONFIG, payload, 4);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SPI_WORD_CONFIG, &resp_length);
    }

    return ret;
}

int16_t coines_config_i2c_bus(enum coines_i2c_bus bus, enum coines_i2c_mode i2c_mode)
{
    uint8_t payload[2] = { bus, i2c_mode };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_I2C_CONFIG, payload, 2);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_I2C_CONFIG, &resp_length);
    }

    return ret;
}

int16_t coines_deconfig_i2c_bus(enum coines_i2c_bus bus)
{
    uint8_t payload = bus;
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    ret = coines_send_packet(COINES_CMD_ID_I2C_DECONFIG, &payload, 1);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_I2C_DECONFIG, &resp_length);
    }

    return ret;
}

int8_t coines_write_i2c(enum coines_i2c_bus bus, uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    uint8_t payload[5] = { bus, dev_addr, reg_addr, 0, 0 };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    memcpy(&payload[3], &count, 2);

    ret = coines_send_multi_packet(COINES_CMD_ID_I2C_WRITE_REG, payload, 5, reg_data, count);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_I2C_WRITE_REG, &resp_length);
    }

    return ret;
}

int8_t coines_read_i2c(enum coines_i2c_bus bus, uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    uint8_t payload[5] = { bus, dev_addr, reg_addr, 0, 0 };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    memcpy(&payload[3], &count, 2);

    ret = coines_send_packet(COINES_CMD_ID_I2C_READ_REG, payload, 5);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_I2C_READ_REG, &resp_length);
    }

    if (resp_length != count)
    {
        return COINES_E_COMM_WRONG_RESPONSE;
    }

    memcpy(reg_data, &resp_buffer[COINES_PROTO_PAYLOAD_POS], resp_length);

    return ret;
}

int8_t coines_write_16bit_spi(enum coines_spi_bus bus, uint8_t cs, uint16_t reg_addr, void *reg_data, uint16_t count)
{
    return COINES_E_FAILURE;
}

int8_t coines_write_spi(enum coines_spi_bus bus, uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    uint8_t payload[5] = { bus, dev_addr, reg_addr, 0, 0 };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    memcpy(&payload[3], &count, 2);

    ret = coines_send_multi_packet(COINES_CMD_ID_SPI_WRITE_REG, payload, 5, reg_data, count);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SPI_WRITE_REG, &resp_length);
    }

    return ret;
}

int8_t coines_read_16bit_spi(enum coines_spi_bus bus, uint8_t cs, uint16_t reg_addr, void *reg_data, uint16_t count)
{
    return COINES_E_FAILURE;
}

int8_t coines_read_spi(enum coines_spi_bus bus, uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    uint8_t payload[5] = { bus, dev_addr, reg_addr, 0, 0 };
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    memcpy(&payload[3], &count, 2);

    ret = coines_send_packet(COINES_CMD_ID_SPI_READ_REG, payload, 5);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SPI_READ_REG, &resp_length);
    }

    if (resp_length != count)
    {
        return COINES_E_COMM_WRONG_RESPONSE;
    }

    memcpy(reg_data, &resp_buffer[COINES_PROTO_PAYLOAD_POS], resp_length);

    return ret;
}

void coines_delay_msec(uint32_t delay_ms)
{
#ifdef PLATFORM_LINUX
    uint32_t delay_microsec = (uint32_t)(delay_ms * 1000);
    usleep(delay_microsec);
#endif

#ifdef PLATFORM_WINDOWS
    Sleep(delay_ms);
#endif
}

void coines_delay_usec(uint32_t delay_us)
{
#ifdef PLATFORM_LINUX
    usleep(delay_us);
#endif

#ifdef PLATFORM_WINDOWS
    Sleep(delay_us < 1000 ? 1 : (delay_us / 1000));
#endif
}

uint32_t coines_get_millis(void)
{
    return (uint32_t)(clock());
}

uint64_t coines_get_micro_sec(void)
{
    return (uint64_t)(((uint64_t)clock()) * 1000);
}

char *coines_get_version()
{
    return COINES_VERSION;
}

int16_t coines_read_temp_data(float *temp_data)
{
    return COINES_E_FAILURE;
}

static int16_t coines_connect_usb(void)
{
    int8_t scom_ret = SCOM_OK;
    int16_t coines_ret = COINES_SUCCESS;

    scom_ret = scom_open();
    switch (scom_ret)
    {
        case SCOM_OK:
            coines_ret = COINES_SUCCESS;
            break;
        case SCOM_E_DEV_NOT_FOUND:
            coines_ret = COINES_E_DEVICE_NOT_FOUND;
            break;
        case SCOM_E_PORT_IN_USE:
            coines_ret = COINES_E_UNABLE_CLAIM_INTF;
            break;
        case SCOM_E_PORT_NOT_OPEN:
            coines_ret = COINES_E_FAILURE;
            break;
        default:
            coines_ret = COINES_E_FAILURE;
    }

    return coines_ret;
}

static int16_t coines_disconnect(void)
{
    int8_t scom_ret = SCOM_OK;
    int16_t coines_ret = COINES_SUCCESS;

    scom_ret = scom_close();
    switch (scom_ret)
    {
        case SCOM_OK:
            coines_ret = COINES_SUCCESS;
            break;
        case SCOM_E_DEV_NOT_FOUND:
            coines_ret = COINES_E_DEVICE_NOT_FOUND;
            break;
        case SCOM_E_PORT_IN_USE:
            coines_ret = COINES_E_UNABLE_CLAIM_INTF;
            break;
        case SCOM_E_PORT_NOT_OPEN:
            coines_ret = COINES_E_FAILURE;
            break;
        default:
            coines_ret = COINES_E_FAILURE;
    }

    return coines_ret;
}

int16_t coines_config_streaming(uint8_t channel_id,
                                struct coines_streaming_config *stream_config,
                                struct coines_streaming_blocks *data_blocks)
{
    int16_t rslt = COINES_SUCCESS;
    

    if ((stream_config != NULL) && (data_blocks != NULL))
    {
        if (coines_sensor_id_count > COINES_MAX_SENSOR_ID)
        {
            return COINES_E_FAILURE;
        }

        coines_streaming_cfg_buf[coines_sensor_id_count].channel_id = channel_id;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.intf = stream_config->intf;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.i2c_bus = stream_config->i2c_bus;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.spi_bus = stream_config->spi_bus;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.dev_addr = stream_config->dev_addr;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.cs_pin = stream_config->cs_pin;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.sampling_time = stream_config->sampling_time;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.sampling_units = stream_config->sampling_units;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.int_pin = stream_config->int_pin;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.int_timestamp = stream_config->int_timestamp;
        coines_streaming_cfg_buf[coines_sensor_id_count].data_blocks.no_of_blocks = data_blocks->no_of_blocks;
        memcpy(coines_streaming_cfg_buf[coines_sensor_id_count].data_blocks.reg_start_addr,
               data_blocks->reg_start_addr,
               data_blocks->no_of_blocks);
        memcpy(coines_streaming_cfg_buf[coines_sensor_id_count].data_blocks.no_of_data_bytes,
               data_blocks->no_of_data_bytes,
               data_blocks->no_of_blocks);
        coines_sensor_id_count++;
    }
    else
    {
        rslt = COINES_E_NULL_PTR;
    }

    return rslt;
}

int16_t config_streaming_mode(enum coines_streaming_mode stream_mode)
{
    int16_t ret = COINES_SUCCESS;
    double sampling_time[2] = { 0, 0 };
    double remaining = 0;
    uint16_t resp_length = 0;
    uint8_t sampling_unit[2];
    uint16_t gcd_sampling_time;
    enum coines_sampling_unit gcd_sampling_unit;
    uint32_t i;

    if (stream_mode == COINES_STREAMING_MODE_POLLING)
    {

        /*check if sensor id count is greater than 1*/
        if (coines_sensor_id_count > 1)
        {
            for (i = 0; i < coines_sensor_id_count; i++)
            {
                sampling_time[i] = (double)coines_streaming_cfg_buf[i].stream_config.sampling_time;
                sampling_unit[i] = coines_streaming_cfg_buf[i].stream_config.sampling_units;
                sampling_time[i] =
                                   (sampling_unit[i] ==
                                    COINES_SAMPLING_TIME_IN_MICRO_SEC) ? (sampling_time[i] / 1000.00) : sampling_time[i];
            }

            /* Calculate GCD */
            while (sampling_time[1] != 0)
            {
                remaining = (double)fmod(sampling_time[0], sampling_time[1]);
                sampling_time[0] = sampling_time[1];
                sampling_time[1] = remaining;
            }

            /* If decimal point is present, convert to microsecond */
            if ((sampling_time[0] - (int32_t)sampling_time[0]) != 0)
            {
                /* Need to convert to microsecond */
                gcd_sampling_time = (uint16_t)(sampling_time[0] * 1000);
                gcd_sampling_unit = COINES_SAMPLING_TIME_IN_MICRO_SEC;
            }
            else
            {
                gcd_sampling_time = (uint16_t)sampling_time[0];
                gcd_sampling_unit = COINES_SAMPLING_TIME_IN_MILLI_SEC;
            }
        }
        else
        {
            gcd_sampling_time = coines_streaming_cfg_buf[coines_sensor_id_count - 1].stream_config.sampling_time;
            gcd_sampling_unit = coines_streaming_cfg_buf[coines_sensor_id_count - 1].stream_config.sampling_units;
        }

        /*general streaming settings*/
        uint8_t payload[5];
        payload[0] = coines_sensor_id_count;
        payload[1] = 1; // Don't know need to update 
        payload[2] = gcd_sampling_time >> 8;
        payload[3] = gcd_sampling_time & 0xFF;
        payload[4] = gcd_sampling_unit;

        ret = coines_send_packet(COINES_CMD_ID_POLL_STREAM_COMMON, payload, 5);
        if(ret == COINES_SUCCESS)
        {
            ret = coines_receive_resp(COINES_CMD_ID_POLL_STREAM_COMMON,&resp_length);    
        }
        else
        {
            return ret;
        }
    }
    else
    {
        ret = COINES_SUCCESS;
    }

    return ret;
}

int16_t coines_start_stop_streaming(enum coines_streaming_mode stream_mode, uint8_t start_stop)
{
    int16_t ret = COINES_SUCCESS;
    uint32_t i, j;
    uint16_t no_of_bytes_read = 0;
    uint8_t payload[50];
    uint8_t index = 0;
    uint8_t payload_len = 0;
    uint16_t resp_length;
 
    /*check the if it is start request for polling streaming*/
    if (start_stop)
    {
        //samples = 50;//COINES_STREAM_INFINITE_SAMPLES;
        coines_sensor_info.no_of_sensors_enabled = coines_sensor_id_count;

        ret = config_streaming_mode(stream_mode);

        /* bridge streaming settings */
        for (i = 0; i < coines_sensor_id_count; i++)
        {
            memset(&payload[0], 0x0, sizeof(payload));
            payload[index++] = coines_streaming_cfg_buf[i].channel_id;
            payload[index++] = (uint8_t)coines_streaming_cfg_buf[i].stream_config.intf;
            payload[index++] = (uint8_t)coines_streaming_cfg_buf[i].stream_config.i2c_bus;
            payload[index++] = (uint8_t)coines_streaming_cfg_buf[i].stream_config.spi_bus;
            payload[index++] = coines_streaming_cfg_buf[i].stream_config.dev_addr;
            payload[index++] = (uint8_t)coines_streaming_cfg_buf[i].stream_config.cs_pin;
            payload[index++] = coines_streaming_cfg_buf[i].stream_config.sampling_time >> 8;
            payload[index++] = coines_streaming_cfg_buf[i].stream_config.sampling_time & 0xFF;
            payload[index++] = coines_streaming_cfg_buf[i].stream_config.sampling_units;
            payload[index++] = coines_streaming_cfg_buf[i].stream_config.int_pin;
            payload[index++] = coines_streaming_cfg_buf[i].stream_config.int_timestamp;
            payload[index++] = coines_streaming_cfg_buf[i].data_blocks.no_of_blocks >> 8;
            payload[index++] = coines_streaming_cfg_buf[i].data_blocks.no_of_blocks & 0xFF;
            for(j = 0; j < coines_streaming_cfg_buf[i].data_blocks.no_of_blocks; j++ )
            {
                payload[COINES_PROTO_REG_START_ADDR_POS + j] = coines_streaming_cfg_buf[i].data_blocks.reg_start_addr[j];
                payload[COINES_PROTO_REG_DATA_BYTES_LEN_POS + j] = coines_streaming_cfg_buf[i].data_blocks.no_of_data_bytes[j];
                no_of_bytes_read += coines_streaming_cfg_buf[i].data_blocks.no_of_data_bytes[j];
                
            }

            coines_sensor_info.sensors_byte_count[i] = no_of_bytes_read;

            payload_len = index + (10 * 2);
            
            ret = coines_send_packet(COINES_CMD_ID_POLL_STREAM_CONFIG, &payload[0], payload_len);
            if (ret == COINES_SUCCESS)
            {
                ret = coines_receive_resp(COINES_CMD_ID_POLL_STREAM_CONFIG, &resp_length);
                if(ret != COINES_SUCCESS)
                    return ret;
            }
            else
            {
                return ret;
            }
            index = 0;
            payload_len = 0;
            no_of_bytes_read = 0;
        }
        payload[0] = 0xFF;
        ret = coines_send_packet(COINES_CMD_ID_STREAM_START_STOP, &payload[0], 1);
        if(ret == COINES_SUCCESS)
        {
            ret = coines_receive_resp(COINES_CMD_ID_STREAM_START_STOP, &resp_length);
        }
    }   
    else
    {
        memset(&payload[0], 0x0, sizeof(payload));
        ret = coines_send_packet(COINES_CMD_ID_STREAM_START_STOP, &payload[0], 1);
        if(ret == COINES_SUCCESS)
        {
            ret = coines_receive_resp(COINES_CMD_ID_STREAM_START_STOP, &resp_length);
        }

    }   
    return ret;
}

int16_t coines_read_stream_sensor_data(uint8_t sensor_id,
                                       uint32_t number_of_samples,
                                       uint8_t *data,
                                       uint32_t *valid_samples_count)
{
    int16_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;
    uint16_t no_of_bytes = coines_sensor_info.sensors_byte_count[sensor_id - 1];
    uint16_t index = 0;
    uint16_t samples = 0;
    
    if ((data == NULL) || (valid_samples_count == NULL))
    {
        return COINES_E_NULL_PTR;
    }
    
    for(uint16_t i = 0; i < 65535; i++)
    {
        ret = coines_receive_resp(COINES_READ_SENSOR_DATA, &resp_length);
        if (ret == COINES_SUCCESS)
        {
            if(resp_buffer[4] == sensor_id)
            {
                memcpy((data + index),&resp_buffer[5], no_of_bytes);
                index += no_of_bytes;  
                if (++samples == number_of_samples)
                    break;
                
            }
        }  
        else
        {
            return ret;
        }
    }
    *valid_samples_count = samples;
    return ret;
}


void coines_soft_reset(void)
{
    uint8_t payload[4];
    int8_t ret = COINES_SUCCESS;
    uint16_t resp_length = 0;

    payload[0] = 0x00;
    payload[1] = 0x00;
    payload[2] = 0x0F;
    payload[3] = 0x00;

    ret = coines_send_packet(COINES_CMD_ID_SOFT_RESET, payload, 4);
    if (ret == COINES_SUCCESS)
    {
        ret = coines_receive_resp(COINES_CMD_ID_SOFT_RESET, &resp_length);
    }
}

/*!
 * @brief Read data over the specified interface
 *
 */

uint16_t coines_read_intf(enum coines_comm_intf intf, void *buffer, uint16_t len)
{
    uint32_t n_bytes_read = 0;

    if (intf == COINES_COMM_INTF_USB)
    {
        scom_read_status = scom_read(buffer, len, &n_bytes_read);
    }

    return n_bytes_read;
}

/*!
 * @brief Write data over the specified interface
 *
 */
void coines_write_intf(enum coines_comm_intf intf, void *buffer, uint16_t len)
{
    if (intf == COINES_COMM_INTF_USB)
    {
        scom_write_status = scom_write(buffer, len);
    }
}
