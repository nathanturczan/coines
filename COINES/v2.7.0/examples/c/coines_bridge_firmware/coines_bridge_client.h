/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef COINES_BRIDGE_CLIENT_H
#define COINES_BRIDGE_CLIENT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern 'C' {
#endif

#ifndef COINES_PACKET_SIZE
#define COINES_PACKET_SIZE                   2060
#endif

#ifndef COINES_BUFFER_SIZE
#define COINES_BUFFER_SIZE                   2060
#endif

#define COINES_CMD_HEADER                    UINT8_C(0xA5)
#define COINES_RESP_OK_HEADER                UINT8_C(0x5A)
#define COINES_RESP_NOK_HEADER               UINT8_C(0x55)

#define COINES_PROTO_HEADER_POS              (0)
#define COINES_PROTO_LENGTH_POS              (1)
#define COINES_PROTO_CMD_POS                 (3)
#define COINES_PROTO_PAYLOAD_POS             (4)
#define COINES_PROTO_REG_START_ADDR_POS      (13)
#define COINES_PROTO_REG_DATA_BYTES_LEN_POS  (23)

enum coines_cmds {
    COINES_CMD_ID_ECHO,
    COINES_CMD_ID_GET_BOARD_INFO,
    COINES_CMD_ID_SET_PIN,
    COINES_CMD_ID_GET_PIN,
    COINES_CMD_ID_SET_VDD_VDDIO,
    COINES_CMD_ID_SPI_CONFIG,
    COINES_CMD_ID_SPI_DECONFIG,
    COINES_CMD_ID_SPI_WORD_CONFIG,
    COINES_CMD_ID_SPI_WRITE_REG_16,
    COINES_CMD_ID_SPI_WRITE_REG,
    COINES_CMD_ID_SPI_READ_REG_16,
    COINES_CMD_ID_SPI_READ_REG,
    COINES_CMD_ID_I2C_CONFIG,
    COINES_CMD_ID_I2C_DECONFIG,
    COINES_CMD_ID_I2C_WRITE_REG,
    COINES_CMD_ID_I2C_READ_REG,
    COINES_CMD_ID_I2C_WRITE,
    COINES_CMD_ID_I2C_READ,
    COINES_CMD_ID_GET_TEMP,
    COINES_CMD_ID_GET_BATTERY,
    COINES_CMD_ID_RESET,
    COINES_CMD_ID_SET_LED,
    COINES_CMD_ID_POLL_STREAM_COMMON,
    COINES_CMD_ID_POLL_STREAM_CONFIG,
    COINES_CMD_ID_INT_STREAM_CONFIG,
    COINES_CMD_ID_FIFO_STREAM_CONFIG,
    COINES_CMD_ID_STREAM_START_STOP,
    COINES_READ_SENSOR_DATA,
    COINES_CMD_ID_SOFT_RESET,
    COINES_N_CMDS
};

typedef enum sensor_interface {
    COINES_I2C,
    COINES_SPI
} sensor_interface_t;

typedef enum
{
    COINES_STREAM_NO_TIMESTAMP = 0, /*< no timestamp */
    COINES_STREAM_USE_TIMESTAMP = 1 /*< Timestamp is present */
} coines_stream_timestamp_t;

typedef enum
{
    COINES_STREAM_MODE_INTERRUPT, /*< interrupt*/
    COINES_STREAM_MODE_POLLING, /*< polling*/
    COINES_STREAM_MODE_FIFO_POLLING /*<fifo polling*/
} coines_stream_mode_t;

typedef struct
{
    uint32_t gst_period_us; /*< Global Sampling Timer period (in polling mode, all sensor sampling periods are multiple
                             * of this period)*/
    coines_stream_timestamp_t ts_mode; /*< ts mode */
    coines_stream_mode_t stream_mode; /*< stream mode */
} coines_stream_settings_t;

typedef int8_t (*coines_cmd_callback)(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                                      uint16_t *resp_length);

struct coines_cbt
{
    coines_cmd_callback cmd_callback[COINES_N_CMDS];
};

int8_t coines_process_packet(uint8_t *packet,
                             uint16_t packet_length,
                             uint8_t *resp,
                             uint16_t *resp_length,
                             struct coines_cbt *cbt);

#ifdef __cplusplus
}
#endif

#endif
