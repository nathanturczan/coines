/**
 * Copyright (C) 2020 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "coines.h"
#include "coines_bridge_client.h"
#include "decoder.h"
#include "stream.h"

#define COM_READ_BUFF       2060
#define STREAM_MAX_COUNT_T  3

static uint8_t timer_triggered = 0;

static coines_stream_settings_t stream_config =
{ .gst_period_us = 0, .ts_mode = COINES_STREAM_NO_TIMESTAMP, .stream_mode = COINES_STREAM_MODE_POLLING };

struct coines_streaming_settings
{
    uint8_t channel_id; /*< streaming channel 1/2 */
    struct coines_streaming_config stream_config; /*< streaming config */
    struct coines_streaming_blocks data_blocks; /*< streaming data blocks */
    uint32_t sampling_period_us;
    uint32_t gst_ticks_counter;
    uint32_t gst_multiplier;
};

static struct coines_streaming_settings streaming_settings[STREAM_MAX_COUNT_T];
uint8_t stream_count = 0;
bool read_stream_data = false;
static uint32_t life_led_error_period = 500;
static uint32_t life_led_normal_period = 2500;
static uint32_t life_led_blink_period = 50;
static uint16_t cmd_cntr = 0, resp_cntr = 0;
static bool soft_reset_triggered = false;

static enum coines_comm_intf comm_intf = COINES_COMM_INTF_BLE;
static bool error = false;

static int8_t echo_callback(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length);
static int8_t get_board_info_callback(uint8_t cmd,
                                      uint8_t *payload,
                                      uint16_t payload_length,
                                      uint8_t *resp,
                                      uint16_t *resp_length);
static int8_t set_pin_callback(uint8_t cmd,
                               uint8_t *payload,
                               uint16_t payload_length,
                               uint8_t *resp,
                               uint16_t *resp_length);
static int8_t get_pin_callback(uint8_t cmd,
                               uint8_t *payload,
                               uint16_t payload_length,
                               uint8_t *resp,
                               uint16_t *resp_length);
static int8_t set_vdd_vddio(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length);
static int8_t spi_config(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length);
static int8_t spi_deconfig(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                           uint16_t *resp_length);
static int8_t i2c_config(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length);
static int8_t i2c_deconfig(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                           uint16_t *resp_length);
static int8_t i2c_write_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length);
static int8_t i2c_read_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                           uint16_t *resp_length);
static int8_t spi_write_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length);
static int8_t spi_read_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                           uint16_t *resp_length);
static int8_t poll_streaming_common(uint8_t cmd,
                                    uint8_t *payload,
                                    uint16_t payload_length,
                                    uint8_t *resp,
                                    uint16_t *resp_length);
static int8_t poll_streaming_config(uint8_t cmd,
                                    uint8_t *payload,
                                    uint16_t payload_length,
                                    uint8_t *resp,
                                    uint16_t *resp_length);
static int8_t streaming_start_stop(uint8_t cmd,
                                   uint8_t *payload,
                                   uint16_t payload_length,
                                   uint8_t *resp,
                                   uint16_t *resp_length);
static int8_t soft_reset(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length);
static void read_sensor_data();
static int8_t streaming_start();
static int8_t streaming_stop();
void read_streaming_data();

int main(void)
{
    uint16_t packet_length = 0;
    uint8_t packet[COM_READ_BUFF] = { 0 };
    uint8_t resp[COM_READ_BUFF] = { 0 };
    uint16_t resp_length;
    uint32_t bytes_available = 0;
    uint32_t blink_on = 0, blink_off = 0;
    bool led_state = false;
    struct coines_cbt cbt = { 0 };
    enum coines_led led = COINES_LED_GREEN;
    int8_t cmd_resp = COINES_SUCCESS;
    bool intf_connected = false;

    coines_open_comm_intf(comm_intf, NULL); /* Wait here till USB is connected */

    coines_set_led(COINES_LED_RED, COINES_LED_STATE_OFF); /* Turn off the LED to indicate that the board is connected */
    cbt.cmd_callback[COINES_CMD_ID_ECHO] = echo_callback;
    cbt.cmd_callback[COINES_CMD_ID_GET_BOARD_INFO] = get_board_info_callback;
    cbt.cmd_callback[COINES_CMD_ID_SET_PIN] = set_pin_callback;
    cbt.cmd_callback[COINES_CMD_ID_GET_PIN] = get_pin_callback;
    cbt.cmd_callback[COINES_CMD_ID_SET_VDD_VDDIO] = set_vdd_vddio;
    cbt.cmd_callback[COINES_CMD_ID_SPI_CONFIG] = spi_config;
    cbt.cmd_callback[COINES_CMD_ID_SPI_DECONFIG] = spi_deconfig;
    cbt.cmd_callback[COINES_CMD_ID_I2C_CONFIG] = i2c_config;
    cbt.cmd_callback[COINES_CMD_ID_I2C_DECONFIG] = i2c_deconfig;
    cbt.cmd_callback[COINES_CMD_ID_I2C_WRITE_REG] = i2c_write_reg;
    cbt.cmd_callback[COINES_CMD_ID_I2C_READ_REG] = i2c_read_reg;
    cbt.cmd_callback[COINES_CMD_ID_SPI_WRITE_REG] = spi_write_reg;
    cbt.cmd_callback[COINES_CMD_ID_SPI_READ_REG] = spi_read_reg;
    cbt.cmd_callback[COINES_CMD_ID_POLL_STREAM_COMMON] = poll_streaming_common;
    cbt.cmd_callback[COINES_CMD_ID_POLL_STREAM_CONFIG] = poll_streaming_config;
    cbt.cmd_callback[COINES_CMD_ID_STREAM_START_STOP] = streaming_start_stop;
    cbt.cmd_callback[COINES_CMD_ID_SOFT_RESET] = soft_reset;

    while (1)
    {
        if (coines_intf_connected(COINES_COMM_INTF_USB))
        {
            comm_intf = COINES_COMM_INTF_USB;
            led = COINES_LED_GREEN;
            intf_connected = true;
        }
        else if (coines_intf_connected(COINES_COMM_INTF_BLE))
        {
            comm_intf = COINES_COMM_INTF_BLE;
            led = COINES_LED_BLUE;
            intf_connected = true;
        }

        if (intf_connected)
        {
            bytes_available = coines_intf_available(comm_intf);
            if (bytes_available >= 3)
            {
                coines_set_led(led, COINES_LED_STATE_ON); /* Turn on the LED to indicate command is being processed */

                coines_read_intf(comm_intf, packet, 3);
                if (packet[COINES_PROTO_HEADER_POS] == COINES_CMD_HEADER)
                {
                    packet_length = 0;
                    memcpy(&packet_length, &packet[COINES_PROTO_LENGTH_POS], 2);
                    if (packet_length)
                    {
                        while (coines_intf_available(comm_intf) < (packet_length - 3))
                            ;

                        coines_read_intf(comm_intf, &packet[COINES_PROTO_CMD_POS], packet_length - 3);
                        resp_length = 0;
                        cmd_cntr++;

                        if (packet_length >= 3)
                        {
                            cmd_resp = coines_process_packet(packet, packet_length, resp, &resp_length, &cbt);
                            if (cmd_resp != COINES_SUCCESS)
                            {
                                resp[COINES_PROTO_HEADER_POS] = COINES_RESP_NOK_HEADER;
                                resp[COINES_PROTO_CMD_POS] = packet[COINES_PROTO_CMD_POS];
                                resp_length = 5;
                                memcpy(&resp[COINES_PROTO_LENGTH_POS], &resp_length, 2);
                                resp[COINES_PROTO_PAYLOAD_POS] = (uint8_t)cmd_resp;
                                error = true;
                                life_led_error_period = 1000;
                            }

                            resp_cntr++;
                        }

                        if (resp_length)
                        {
                            coines_write_intf(comm_intf, resp, resp_length);
                            resp_length = 0;
                        }
                    }
                }
                else if ((packet[DECODER_BYTEPOS_HEADER] == DECODER_HEADER_VALUE) ||
                         (packet[DECODER_BYTEPOS_HEADER] == UINT8_C(0x55)))
                {
                    coines_set_led(led, COINES_LED_STATE_ON);
                    packet_length = packet[DECODER_BYTEPOS_SIZE];
                    if (packet_length)
                    {
                        while (coines_intf_available(comm_intf) < (packet_length - 3))
                            ;

                        coines_read_intf(comm_intf, &packet[COINES_PROTO_CMD_POS], packet_length - 3);

                        decoder_process_cmds(packet);
                    }

                    coines_set_led(led, COINES_LED_STATE_OFF);
                }
                else
                {
                    error = true;
                    life_led_error_period = 125;
                }

                coines_set_led(led, COINES_LED_STATE_OFF); /* Turn off the LED to indicate processing is done */
            }

            if (blink_on <= coines_get_millis())
            {
                if (error)
                {
                    blink_on = coines_get_millis() + life_led_error_period;
                }
                else
                {
                    blink_on = coines_get_millis() + life_led_normal_period;
                }

                blink_off = coines_get_millis() + life_led_blink_period;
                coines_set_led(COINES_LED_RED, COINES_LED_STATE_ON); /* Turn on the LED to indicate that the board is
                                                                      * connected */
                led_state = true;
            }

            if (led_state && (blink_off <= coines_get_millis()))
            {
                coines_set_led(COINES_LED_RED, COINES_LED_STATE_OFF); /* Turn off the LED to indicate processing is done
                                                                      */
                led_state = false;
            }

            read_sensor_data();
            send_old_protocol_streaming_response(); /* Look for configured streaming in the old-protocol and send the
                                                 * response */

            if (soft_reset_triggered)
            {
                coines_close_comm_intf(comm_intf, NULL);
                coines_soft_reset();
            }
        }
    }

    return 0;
}

void decoder_write_resp(void *buff, uint16_t len)
{
    coines_write_intf(comm_intf, buff, len);
}

static int8_t echo_callback(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length)
{
    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = payload_length + 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;
    memcpy(&resp[COINES_PROTO_PAYLOAD_POS], payload, payload_length);

    return COINES_SUCCESS;
}

static int8_t get_board_info_callback(uint8_t cmd,
                                      uint8_t *payload,
                                      uint16_t payload_length,
                                      uint8_t *resp,
                                      uint16_t *resp_length)
{
    struct coines_board_info board_info = { 0 };
    int8_t rslt;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4 + 7;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    rslt = coines_get_board_info(&board_info);
    if (rslt == COINES_SUCCESS)
    {
        memcpy(&resp[COINES_PROTO_PAYLOAD_POS], &board_info.hardware_id, 2);
        memcpy(&resp[COINES_PROTO_PAYLOAD_POS + 2], &board_info.software_id, 2);
        memcpy(&resp[COINES_PROTO_PAYLOAD_POS + 4], &board_info.board, 1);
        memcpy(&resp[COINES_PROTO_PAYLOAD_POS + 5], &board_info.shuttle_id, 2);
    }

    return rslt;
}

static int8_t set_pin_callback(uint8_t cmd,
                               uint8_t *payload,
                               uint16_t payload_length,
                               uint8_t *resp,
                               uint16_t *resp_length)
{
    enum coines_multi_io_pin pin_number;
    enum coines_pin_direction direction;
    enum coines_pin_value pin_value;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    if (payload_length >= 3)
    {
        pin_number = payload[0];
        direction = payload[1];
        pin_value = payload[2];
    }
    else
    {
        return COINES_E_FAILURE;
    }

    rslt = coines_set_pin_config(pin_number, direction, pin_value);
    if (rslt != COINES_SUCCESS)
    {
        return rslt;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    return COINES_SUCCESS;
}

static int8_t get_pin_callback(uint8_t cmd,
                               uint8_t *payload,
                               uint16_t payload_length,
                               uint8_t *resp,
                               uint16_t *resp_length)
{
    enum coines_multi_io_pin pin_number;
    enum coines_pin_direction direction;
    enum coines_pin_value pin_value;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    if (payload_length >= 3)
    {
        pin_number = payload[0];
        direction = payload[1];
        pin_value = payload[2];
    }
    else
    {
        return COINES_E_FAILURE;
    }

    rslt = coines_get_pin_config(pin_number, &direction, &pin_value);
    if (rslt != COINES_SUCCESS)
    {
        return rslt;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4 + 3;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;
    resp[COINES_PROTO_PAYLOAD_POS] = pin_number;
    resp[COINES_PROTO_PAYLOAD_POS + 1] = direction;
    resp[COINES_PROTO_PAYLOAD_POS + 2] = pin_value;

    return COINES_SUCCESS;
}

static int8_t set_vdd_vddio(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length)
{
    uint16_t vdd = 0, vddio = 0;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    if (payload_length >= 4)
    {
        memcpy(&vdd, payload, 2);
        memcpy(&vddio, &payload[2], 2);
        rslt = coines_set_shuttleboard_vdd_vddio_config(vdd, vddio);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return COINES_SUCCESS;
}

static int8_t spi_config(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length)
{
    enum coines_spi_bus bus;
    enum coines_spi_speed speed;
    enum coines_spi_mode mode;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    if (payload_length >= 3)
    {
        bus = payload[0];
        speed = payload[1];
        mode = payload[2];
        rslt = coines_config_spi_bus(bus, speed, mode);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return COINES_SUCCESS;
}

static int8_t spi_deconfig(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length)
{
    enum coines_spi_bus bus;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    if (payload_length >= 1)
    {
        bus = payload[0];
        rslt = coines_deconfig_spi_bus(bus);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return COINES_SUCCESS;
}

static int8_t i2c_config(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length)
{
    enum coines_i2c_bus bus;
    enum coines_i2c_mode mode;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    if (payload_length >= 2)
    {
        bus = payload[0];
        mode = payload[1];
        rslt = coines_config_i2c_bus(bus, mode);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return COINES_SUCCESS;
}

static int8_t i2c_deconfig(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length)
{
    enum coines_i2c_bus bus;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    if (payload_length >= 1)
    {
        bus = payload[0];
        rslt = coines_deconfig_i2c_bus(bus);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return COINES_SUCCESS;
}

static int8_t i2c_write_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length)
{
    uint8_t dev_addr, reg_addr;
    uint16_t data_length;
    enum coines_i2c_bus bus;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    if (payload_length >= 6)
    {
        bus = (enum coines_i2c_bus)payload[0];
        dev_addr = payload[1];
        reg_addr = payload[2];
        memcpy(&data_length, &payload[3], 2);
        rslt = coines_write_i2c(bus, dev_addr, reg_addr, &payload[5], data_length);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return COINES_SUCCESS;
}

static int8_t i2c_read_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length)
{
    uint8_t dev_addr, reg_addr;
    uint16_t data_length;
    enum coines_i2c_bus bus;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    if (payload_length >= 5)
    {
        bus = (enum coines_i2c_bus)payload[0];
        dev_addr = payload[1];
        reg_addr = payload[2];
        memcpy(&data_length, &payload[3], 2);
        rslt = coines_read_i2c(bus, dev_addr, reg_addr, &resp[COINES_PROTO_PAYLOAD_POS], data_length);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4 + data_length;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    return COINES_SUCCESS;
}

static int8_t spi_write_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp,
                            uint16_t *resp_length)
{
    uint8_t dev_addr, reg_addr;
    uint16_t data_length;
    enum coines_spi_bus bus;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    if (payload_length >= 6)
    {
        bus = (enum coines_spi_bus)payload[0];
        dev_addr = payload[1];
        reg_addr = payload[2];
        memcpy(&data_length, &payload[3], 2);
        rslt = coines_write_spi(bus, dev_addr, reg_addr, &payload[5], data_length);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return COINES_SUCCESS;
}

static int8_t spi_read_reg(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length)
{
    uint8_t dev_addr, reg_addr;
    uint16_t data_length;
    enum coines_spi_bus bus;
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    if (payload_length >= 5)
    {
        bus = (enum coines_spi_bus)payload[0];
        dev_addr = payload[1];
        reg_addr = payload[2];
        memcpy(&data_length, &payload[3], 2);
        rslt = coines_read_spi(bus, dev_addr, reg_addr, &resp[COINES_PROTO_PAYLOAD_POS], data_length);
        if (rslt != COINES_SUCCESS)
        {
            return rslt;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4 + data_length;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    return COINES_SUCCESS;
}

static int8_t soft_reset(uint8_t cmd, uint8_t *payload, uint16_t payload_length, uint8_t *resp, uint16_t *resp_length)
{
    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    if (payload_length >= 4)
    {
        soft_reset_triggered = true;
    }
    else
    {
        return COINES_E_FAILURE;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    return COINES_SUCCESS;

}

static int8_t poll_streaming_config(uint8_t cmd,
                                    uint8_t *payload,
                                    uint16_t payload_length,
                                    uint8_t *resp,
                                    uint16_t *resp_length)
{
    uint8_t index_t;
    uint16_t raw_time;
    struct coines_streaming_settings *stream_p = &streaming_settings[stream_count];

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    if (stream_count < STREAM_MAX_COUNT_T)
    {

        stream_p->channel_id = payload[0];
        stream_p->stream_config.intf = payload[1];
        stream_p->stream_config.i2c_bus = payload[2];
        stream_p->stream_config.spi_bus = payload[3];
        stream_p->stream_config.dev_addr = payload[4];
        stream_p->stream_config.cs_pin = payload[5];
        stream_p->stream_config.sampling_time = payload[6] >> 8 | payload[7];
        stream_p->stream_config.sampling_units = payload[8];
        stream_p->stream_config.int_pin = payload[9];
        stream_p->stream_config.int_timestamp = payload[10];
        stream_p->data_blocks.no_of_blocks = payload[11] >> 8 | payload[12];

        for (index_t = 0 ; index_t < stream_p->data_blocks.no_of_blocks; index_t++ )
        {
            stream_p->data_blocks.reg_start_addr[index_t] = payload[COINES_PROTO_REG_START_ADDR_POS + index_t];
            stream_p->data_blocks.no_of_data_bytes[index_t] = payload[COINES_PROTO_REG_DATA_BYTES_LEN_POS + index_t];
        }

        raw_time = stream_p->stream_config.sampling_time;
        if (stream_p->stream_config.sampling_units == (uint8_t)COINES_SAMPLING_TIME_IN_MICRO_SEC)
        {
            stream_p->sampling_period_us = raw_time;
        }
        else
        {
            stream_p->sampling_period_us = raw_time * 1000;
        }

        if (stream_p->sampling_period_us <= stream_config.gst_period_us)
        {
            stream_p->gst_multiplier = 1;
        }
        else
        {
            stream_p->gst_multiplier = stream_p->sampling_period_us / stream_config.gst_period_us;
        }

        stream_p->gst_ticks_counter = stream_p->gst_multiplier;

        stream_count++;
    }
    else
    {
        return COINES_E_FAILURE;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    return COINES_SUCCESS;
}

static int8_t poll_streaming_common(uint8_t cmd,
                                    uint8_t *payload,
                                    uint16_t payload_length,
                                    uint8_t *resp,
                                    uint16_t *resp_length)
{
    int8_t rslt = COINES_SUCCESS;
    uint16_t raw_time;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    raw_time = (payload[2] << 8) | payload[3];
    if (raw_time > 0)
    {
        if (payload[4] == (uint8_t)COINES_SAMPLING_TIME_IN_MICRO_SEC)
        {
            stream_config.gst_period_us = raw_time;
        }
        else if (payload[4] == (uint8_t)COINES_SAMPLING_TIME_IN_MILLI_SEC)
        {
            stream_config.gst_period_us = raw_time * 1000;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    stream_config.stream_mode = COINES_STREAM_MODE_POLLING;
    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;

    return rslt;

}

static int8_t streaming_start_stop(uint8_t cmd,
                                   uint8_t *payload,
                                   uint16_t payload_length,
                                   uint8_t *resp,
                                   uint16_t *resp_length)
{
    int8_t rslt = COINES_SUCCESS;

    if ((payload == NULL) || (resp == NULL) || (resp_length == NULL))
    {
        return COINES_E_NULL_PTR;
    }

    if (stream_count > 0)
    {
        if (payload[0] == 0x00)
        {
            rslt = streaming_stop();
        }
        else if (payload[0] == 0xFF)
        {
            rslt = streaming_start();
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    if (rslt != COINES_SUCCESS)
    {
        return rslt;
    }

    resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
    *resp_length = 4 + payload_length;
    memcpy(&resp[COINES_PROTO_LENGTH_POS], resp_length, 2);
    resp[COINES_PROTO_CMD_POS] = cmd;
    memcpy(&resp[COINES_PROTO_PAYLOAD_POS], payload, payload_length);

    return COINES_SUCCESS;
}

void read_streaming_data()
{
    if (timer_triggered == 0)
    {
        timer_triggered = 1;
    }
}

static int8_t streaming_start()
{
    int8_t rslt = COINES_SUCCESS;

    rslt = coines_timer_config(COINES_TIMER_INSTANCE_0, read_streaming_data);
    if (rslt == COINES_SUCCESS)
    {
        rslt = coines_timer_start(COINES_TIMER_INSTANCE_0, stream_config.gst_period_us);
        if (rslt != COINES_SUCCESS)
        {
            return COINES_E_FAILURE;
        }
    }
    else
    {
        return COINES_E_FAILURE;
    }

    return rslt;

}

static int8_t streaming_stop()
{
    int8_t rslt = COINES_SUCCESS;

    rslt = coines_timer_stop(COINES_TIMER_INSTANCE_0);
    if (rslt != COINES_SUCCESS)
    {
        return COINES_E_FAILURE;
    }

    return rslt;
}

void read_sensor_data(void)
{
    uint8_t resp[100];
    uint8_t read_buff[50];
    uint8_t byte_pos = 0;
    uint16_t resp_length;
    uint8_t total_bytes = 0;

    if ((stream_count > 0) && timer_triggered)
    {
        for (uint8_t i = 0; i < stream_count; i++)
        {
            if (streaming_settings[i].gst_ticks_counter > 0)
            {
                streaming_settings[i].gst_ticks_counter--;
            }

            if (streaming_settings[i].gst_ticks_counter == 0)
            {
                /* reload tick-counter */
                streaming_settings[i].gst_ticks_counter = streaming_settings[i].gst_multiplier;

                for (uint8_t index = 0; index < streaming_settings[i].data_blocks.no_of_blocks; index++)
                {
                    /* Numer of blocks will be read inside this loop */
                    if (streaming_settings[i].stream_config.intf == COINES_SENSOR_INTF_I2C)
                    {
                        coines_read_i2c(streaming_settings[i].stream_config.i2c_bus,
                                        streaming_settings[i].stream_config.dev_addr,
                                        streaming_settings[i].data_blocks.reg_start_addr[index],
                                        &read_buff[byte_pos],
                                        streaming_settings[i].data_blocks.no_of_data_bytes[index]);
                    }
                    else
                    {
                        coines_read_spi(streaming_settings[i].stream_config.spi_bus,
                                        streaming_settings[i].stream_config.cs_pin,
                                        (streaming_settings[i].data_blocks.reg_start_addr[index] | 0x80),
                                        &read_buff[byte_pos],
                                        streaming_settings[i].data_blocks.no_of_data_bytes[index]);
                    }

                    total_bytes += streaming_settings[i].data_blocks.no_of_data_bytes[index];
                    byte_pos += total_bytes;
                }

                resp[COINES_PROTO_HEADER_POS] = COINES_RESP_OK_HEADER;
                resp_length = 4 + 1 + total_bytes;
                memcpy(&resp[COINES_PROTO_LENGTH_POS], &resp_length, 2);
                resp[COINES_PROTO_CMD_POS] = COINES_READ_SENSOR_DATA;
                resp[COINES_PROTO_PAYLOAD_POS] = i + 1; /*Sensor_id */
                memcpy(&resp[COINES_PROTO_PAYLOAD_POS + 1], read_buff, total_bytes);
                coines_write_intf(comm_intf, resp, resp_length);

                byte_pos = 0;
                total_bytes = 0;
            }
        }

        timer_triggered = 0;

    }
}
