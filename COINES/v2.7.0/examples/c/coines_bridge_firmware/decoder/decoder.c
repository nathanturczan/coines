/**
 * Copyright (C) 2020 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    decoder.c
 * @brief This layer provides functions for processing the commands from the host.
 *
 */

/*!
 * @ingroup APPLICATION
 *
 * @defgroup DECODER decoder
 * @{
 *
 *
 *
 **/

/**********************************************************************************/
/* system header includes */
/**********************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stream.h"
#include "coines.h"
//#include "../com_coines.h"
#include "app30_eeprom.h"

/**********************************************************************************/
/* own header files */
/**********************************************************************************/
#include "decoder.h"

/**********************************************************************************/
/* local macro definitions */
/**********************************************************************************/

/**********************************************************************************/
/* constant definitions */
/**********************************************************************************/

#define SHUTTLE_BOARD_EEPROM_TOTAL_LEN              UINT8_C(112)
/**********************************************************************************/
/* global variables */
/**********************************************************************************/

/*! Variable to store the stream samples count. 1- Single Sample, 255 - Infinity samples */
uint8_t decoder_stream_samples_count = 0;

/**********************************************************************************/
/* static variables */
/**********************************************************************************/

/*! Buffer used to update the command responses to transmit to the host */
static uint8_t xmit_buffer[DECODER_MAX_PKT_SZ];
/*! Variable for time out in case of interrupt not trigger from sensor, TO-DO : Currently not implemented */
static uint16_t timeout_required;
/*! Variable used in write/read function */
static uint32_t sensor_cmd_size = 0;
/*! Buffer used to hold the read/write data */
static uint8_t sensor_cmd_buffer[2054];
/*! Flag to check whether MCU time is needed or not */
static uint8_t use_timestamp = 0;
static uint16_t last_vdd = 0, last_vddio = 0;
/**********************************************************************************/
/* static function declaration */
/**********************************************************************************/

static int8_t sensor_i2c_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t length)
{
	return coines_read_i2c(COINES_I2C_BUS_0, i2c_addr, reg_addr, reg_data, length);
}

static int8_t sensor_i2c_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t length)
{
	return coines_write_i2c(COINES_I2C_BUS_0, i2c_addr, reg_addr, reg_data, length);
}

static int8_t sensor_spi_read(uint8_t spi_cs, uint8_t reg_addr, uint8_t *reg_data, uint32_t length)
{
	return coines_read_spi(COINES_SPI_BUS_0, spi_cs, reg_addr | 0x80, reg_data, length);
}

static int8_t sensor_spi_write(uint8_t spi_cs, uint8_t reg_addr, uint8_t *reg_data, uint32_t length)
{
	return coines_write_spi(COINES_SPI_BUS_0, spi_cs, reg_addr, reg_data, length);
}

/*!
 *
 * @brief       : API will read and update the MCU time in response packets,
 *                based on the use_timestamp variable
 *
 * return       : void
 */
static void decoder_update_timestamp(void);
/*!
 *
 * @brief       : API will sets/gets sensor vdd voltage
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x01_vdd(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API will sets/gets sensor vddio voltage
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x02_vddio(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API will sets/gets asw pin
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x03_asw(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API will sets/gets i2c speed
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x09_i2cspeed(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set cs pin
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x0a_spics(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set/get the interfaces to communicate with sensors
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x11_interface(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set/clear the LED
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x0b_led_control(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set shuttle vdd and vddio voltages
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x14_shuttle_pwr_cfg(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set/get IO pins
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x15_multiioconfig(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API used to write-to/read-from sensor
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x16_writeread(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set/get spi configurations
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x19_spiconfig(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to provide delay
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x1a_delayms(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API used to write-to/read-from sensor with delay
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x22_writedelayread(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API used to get the board information
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x1f_boardinfo(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set use_timestamp
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x29_timestamp(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API used to write-to/read-from third party sensor
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x28_thirdparty_writeread(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to set switch from application to BTL
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x30_app_switch(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to invoke BTL
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x31_invoke_btl(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to write-to/read-from shuttle eeprom
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_0x32_shuttle_eeprom_rw(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to read the global sampling timer period - used for all polling streaming
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 */
static decoder_rsp_t decoder_ext0x03_pollstream_common(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to get and configure polling settings for old sensors
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 *
 */
static decoder_rsp_t decoder_ext0x04_pollstream_old_read(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to get and configure fifo stream settings
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 *
 */
static decoder_rsp_t decoder_ext0x07_fifostream_read(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to start/stop streaming
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 *
 */
static decoder_rsp_t decoder_ext0x0a_ext0x06_ext0x08_stream_startstop(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to get and configure interrupt stream settings
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 *
 */
static decoder_rsp_t decoder_ext0x0e_intstream_read(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to get and configure polling stream settings
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 *
 */
static decoder_rsp_t decoder_ext0x0f_pollstream_read(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       : API to get and configure interrupt stream settings for old sensors
 *
 * @param[in]   : buffer - pointer to the command
 * @pram[in]    : dir -  Tells about set or get
 *
 * @return      : returns decoder_rsp_t enum type
 *
 */
static decoder_rsp_t decoder_ext0x09_intstream_old_read(const uint8_t *buffer, decoder_dir_t dir);
/*!
 *
 * @brief       :API to parse and update the error response
 *
 * @param[in]   : error_code - To hold error response
 * @param[in]   : feature_id - To hold feature, applicable only set/get commands
 * @pram[in]    : cmd -  Command for which error triggered
 *
 * @return      : returns decoder_rsp_t enum type
 *
 */
static decoder_rsp_t decoder_error_fuction(decoder_rsp_t error_code, int8_t feature_id, int8_t cmd);
//static decoder_rsp_t decoder_error_fuction(decoder_rsp_t error_code, int8_t stream_setting);

#ifdef MCU_APP20
bool app30_eeprom_write(uint8_t address, uint8_t *buffer, uint8_t length)
{
	return false;
}

bool app30_eeprom_read(uint16_t address, uint8_t *buffer, uint8_t length)
{
	return false;
}

#endif


/**********************************************************************************/
/* functions */
/**********************************************************************************/
typedef int8_t (*comm_function)(uint8_t, uint8_t, uint8_t*, uint32_t);

/*lint -save -e485 */
// Suppressed duplicate initialization of object element
//TODO: fill in lengths
static const decoder_cmd_t command_list[_DECODER_ID_MAX_] = { [DECODER_ID_VDD] = { .handler = decoder_0x01_vdd,
		.getter_length = 6, .setter_length = 9 }, [DECODER_ID_VDDIO] = { .handler = decoder_0x02_vddio,
		.getter_length = 6, .setter_length = 9 }, [DECODER_ID_ASW] = { .handler = decoder_0x03_asw,
		.getter_length = 7, .setter_length = 8 }, [DECODER_ID_I2CSPEED] = { .handler = decoder_0x09_i2cspeed,
		.getter_length = 7, .setter_length = 8 }, [DECODER_ID_SPICS] = { .handler = decoder_0x0a_spics,
		.getter_length = DECODER_UNKNOWN_SIZE, .setter_length = 8 }, [DECODER_ID_INTERFACE] = { .handler =
		decoder_0x11_interface, .getter_length = 7, .setter_length = 8 }, [DECODER_ID_LED_CONTROL] = {
		.handler = decoder_0x0b_led_control, .getter_length = 8, .setter_length = 8 },
		[DECODER_ID_SHUTTLE_PWR_CFG] = { .handler = decoder_0x14_shuttle_pwr_cfg, .getter_length = 6,
				.setter_length = 12 }, [DECODER_ID_MULTIIOCONFIG] = { .handler =
				decoder_0x15_multiioconfig, .getter_length = 8, .setter_length = 12 },
		[DECODER_ID_WRITEREAD] = { .handler = decoder_0x16_writeread, .getter_length = 18, .setter_length =
		DECODER_UNKNOWN_SIZE }, [DECODER_ID_SPICONFIG] = { .handler = decoder_0x19_spiconfig, .getter_length =
		DECODER_UNKNOWN_SIZE, .setter_length = 10 }, [DECODER_ID_DELAY] = { .handler = decoder_0x1a_delayms,
				.getter_length = DECODER_UNKNOWN_SIZE, .setter_length = 10 }, [DECODER_ID_WRITEDELAYREAD
				] = { .handler = decoder_0x22_writedelayread, .getter_length =
				DECODER_UNKNOWN_SIZE, .setter_length =
				DECODER_UNKNOWN_SIZE }, [DECODER_ID_BOARDINFO] = { .handler = decoder_0x1f_boardinfo,
				.getter_length = 6, .setter_length = DECODER_UNKNOWN_SIZE },
		[DECODER_ID_THIRDPARTYWRITEREAD]= { .handler = decoder_0x28_thirdparty_writeread, .getter_length =
		DECODER_UNKNOWN_SIZE, .setter_length = 40 }, //assumption
		[DECODER_ID_TIMESTAMP] = { .handler = decoder_0x29_timestamp, .getter_length = 7, .setter_length = 7 },
		[DECODER_ID_APP_SWITCH] = { .handler = decoder_0x30_app_switch, .getter_length = DECODER_UNKNOWN_SIZE,
				.setter_length = 10 }, [DECODER_ID_INVOKE_BTL] = { .handler = decoder_0x31_invoke_btl,
				.getter_length = DECODER_UNKNOWN_SIZE, .setter_length = 6 },
		[DECODER_ID_SHUTTLE_EEPROM_RW] = { .handler = decoder_0x32_shuttle_eeprom_rw, .getter_length = 9,
				.setter_length = DECODER_UNKNOWN_SIZE },

		[DECODER_EXTID_POLLSTREAM_COMMON] = { .handler = decoder_ext0x03_pollstream_common, .getter_length =
		DECODER_UNKNOWN_SIZE, .setter_length = 10 }, [DECODER_EXTID_POLLSTREAM_OLD_READ] = { .handler =
				decoder_ext0x04_pollstream_old_read, .getter_length = DECODER_UNKNOWN_SIZE,
				.setter_length = DECODER_UNKNOWN_SIZE }, [DECODER_EXTID_POLLSTREAM_STARTSTOP]= {
				.handler = decoder_ext0x0a_ext0x06_ext0x08_stream_startstop, .getter_length =
				DECODER_UNKNOWN_SIZE, .setter_length = 6 }, [DECODER_EXTID_INTSTREAM_STARTSTOP] = {
				.handler = decoder_ext0x0a_ext0x06_ext0x08_stream_startstop, .getter_length =
				DECODER_UNKNOWN_SIZE, .setter_length = 6 }, [DECODER_EXTID_FIFOSTREAM_READ] = {
				.handler = decoder_ext0x07_fifostream_read, .getter_length =
				DECODER_UNKNOWN_SIZE, .setter_length = 6 }, [DECODER_EXTID_FIFOSTREAM_STARTSTOP]= {
				.handler = decoder_ext0x0a_ext0x06_ext0x08_stream_startstop, .getter_length =
				DECODER_UNKNOWN_SIZE, .setter_length = 6 }, [DECODER_EXTID_INTSTREAM_OLD_READ] = {
				.handler = decoder_ext0x09_intstream_old_read, .getter_length =
				DECODER_UNKNOWN_SIZE, .setter_length = DECODER_UNKNOWN_SIZE },
		[DECODER_EXTID_INTSTREAM_READ] = { .handler = decoder_ext0x0e_intstream_read, .getter_length =
		DECODER_UNKNOWN_SIZE, .setter_length = DECODER_UNKNOWN_SIZE }, [DECODER_EXTID_POLLSTREAM_READ] = {
				.handler = decoder_ext0x0f_pollstream_read, .getter_length =
				DECODER_UNKNOWN_SIZE, .setter_length = DECODER_UNKNOWN_SIZE } };
/*lint -restore */
/*!
 *
 * @brief       : API to parse and update the error response
 */
static decoder_rsp_t decoder_error_fuction(decoder_rsp_t error_code, int8_t feature_id, int8_t cmd)
{
	decoder_rsp_t rsp = error_code;
	uint8_t terminating_pos = 0;

	xmit_buffer[0] = DECODER_HEADER_VALUE;          // Start Command
	xmit_buffer[2] = 1;             // No of Packets
	xmit_buffer[3] = (uint8_t) error_code;     // Type of Error
	xmit_buffer[4] = (uint8_t) cmd | DECODER_RSP_ID_MASK;     // feature

	if ((cmd == (int8_t) DECODER_DIR_SET) || (cmd == (int8_t) DECODER_DIR_GET))
	{
		xmit_buffer[1] = 8;             // Size of the command
		//decoder_update_timestamp();
		xmit_buffer[5] = (uint8_t) feature_id;     // control
		terminating_pos = 6;
	} else
	{
		xmit_buffer[1] = 7;
		//decoder_update_timestamp();
		terminating_pos = 5;
	}

	if (use_timestamp == 1)
	{
		decoder_update_timestamp();
		terminating_pos += 6;
	}
	xmit_buffer[terminating_pos] = DECODER_CR_MACRO;                 // \r
	xmit_buffer[terminating_pos + 1] = DECODER_LF_MACRO;                 // \n

	/* transmit data to PC */
	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;
}
/*!
 *
 * @brief       : API to calculate the length
 */
static inline uint8_t decoder_calc_len(const uint8_t *cmd)
{
	uint8_t length = 0;
	{
		// Length of the command is updated
		length = cmd[DECODER_BYTEPOS_SIZE];

		// check the length is greater than end indicators size
		if (length > DECODER_END_INDICATORS_SIZE)
		{
			/* search for the CRLF cmd terminator in the received buffer */
			if ((cmd[length - 2] == DECODER_CR_MACRO) && (cmd[length - 1] == DECODER_LF_MACRO))
			{
				/* terminator found, command size is now determined */
				//Actual length received in command is correct
			} else
			{
				length = 0;
			}
		} else
		{
			length = 0;
		}

		return length;
	}
}
/*!
 *
 * @brief       : API will read and update the MCU time in response packets,
 *                based on the use_timestamp variable
 */
static void decoder_update_timestamp(void)
{
	if (use_timestamp == 1)
	{
		uint8_t timestamp_location = xmit_buffer[1] - 2;
		uint64_t current_ts = coines_get_micro_sec();

		xmit_buffer[timestamp_location++] = (uint8_t) (current_ts >> 40);
		xmit_buffer[timestamp_location++] = (uint8_t) (current_ts >> 32);
		xmit_buffer[timestamp_location++] = (uint8_t) (current_ts >> 24);
		xmit_buffer[timestamp_location++] = (uint8_t) (current_ts >> 16);
		xmit_buffer[timestamp_location++] = (uint8_t) (current_ts >> 8);
		xmit_buffer[timestamp_location++] = (uint8_t) (current_ts);

		/* update length */
		xmit_buffer[1] += 6;
	}
}
/*!
 *
 * @brief       : API will sets/gets sensor vdd voltage
 */
static decoder_rsp_t decoder_0x01_vdd(const uint8_t *buffer, decoder_dir_t dir)

{
	uint16_t vdd_voltage = 0xFFFF;	//TODO: temporary workaround
	uint8_t vdd_state = 0;
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	if (dir == DECODER_DIR_SET)
	{
		vdd_state = buffer[4];
		vdd_voltage = (uint16_t) (((buffer[5]) << 8) | (buffer[6]));

		//TODO workaround after the voltage is updated 3300 mv support should be removed.
		if ((vdd_voltage <= VDD_VDDIO_3V3) && (vdd_state <= 1))
		{
			if (vdd_state == 1)
			{
				//Workaround to fix DD inputs
				if (vdd_voltage == 0)
				{
					vdd_voltage = VDD_VDDIO_2V8;
				}
				/* pass the voltage value*/
				coines_set_shuttleboard_vdd_vddio_config(vdd_voltage, last_vddio);
				last_vdd = vdd_voltage;

			} else
			{
				coines_set_shuttleboard_vdd_vddio_config(0, last_vddio);
				last_vdd = 0;
			}

		} else
			rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;

	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 11; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_VDD; /* Feature of the Command */
	xmit_buffer[6] = vdd_state; /* vdd state */
	xmit_buffer[7] = (uint8_t) (vdd_voltage >> 8); /* vdd MSB value */
	xmit_buffer[8] = (uint8_t) (vdd_voltage); /* vdd LSB value */

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API will sets/gets sensor vddio voltage
 */
static decoder_rsp_t decoder_0x02_vddio(const uint8_t *buffer, decoder_dir_t dir)

{
	uint16_t vddio_voltage = 0xFFFF;	//TODO: temporary workaround
	uint8_t vddio_state = 0;
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	if (dir == DECODER_DIR_SET)
	{
		vddio_state = buffer[4];
		vddio_voltage = (uint16_t) (((buffer[5]) << 8) | (buffer[6]));

		//TODO workaround after the voltage is updated 3300 mv support should be removed.
		if ((vddio_voltage <= VDD_VDDIO_3V3) && (vddio_state <= 1))
		{
			//TODO:

			if (vddio_state == 1)
			{
				//Workaround to fix DD inputs
				if (vddio_voltage == 0)
				{
					vddio_voltage = VDD_VDDIO_1V8;
				}
				coines_set_shuttleboard_vdd_vddio_config(last_vdd, vddio_voltage);
				last_vddio = vddio_voltage;

			} else
			{
				coines_set_shuttleboard_vdd_vddio_config(last_vdd, 0);
				last_vddio = 0;
			}
		} else
			rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;
	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 11; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_VDDIO; /* Feature of the Command */

	xmit_buffer[6] = vddio_state; /* vdd state */
	xmit_buffer[7] = (uint8_t) (vddio_voltage >> 8); /* vdd MSB value */
	xmit_buffer[8] = (uint8_t) (vddio_voltage); /* vdd LSB value */

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;
}
/*!
 *
 * @brief       : API will sets/gets asw pin
 */
static decoder_rsp_t decoder_0x03_asw(const uint8_t *buffer, decoder_dir_t dir)
{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 10; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) DECODER_RSP_SUCCESS; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_ASW; /* Feature of the Command */
	xmit_buffer[6] = 1; 					// As there is no switch we should send ON always
	xmit_buffer[7] = buffer[5];

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API will sets/gets i2c speed
 */
static decoder_rsp_t decoder_0x09_i2cspeed(const uint8_t *buffer, decoder_dir_t dir)

{
	uint8_t bus_index = 0, bus_speed = 0;
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	if (dir == DECODER_DIR_SET)
	{
		bus_index = buffer[4];
		bus_speed = buffer[5];

		if (bus_speed <= (uint8_t) COINES_I2C_SPEED_1_7_MHZ)
		{
			if (bus_index == 0)
			{
				coines_config_i2c_bus(COINES_I2C_BUS_0, bus_speed);
			} else
				rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;
		} else
		{
			rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;
		}
	} else
		rsp = DECODER_RSP_UNKNOWN_INSTR;

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 10; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_I2CSPEED;
	xmit_buffer[6] = bus_index;
	xmit_buffer[7] = bus_speed;

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API to set cs pin
 */
static decoder_rsp_t decoder_0x0a_spics(const uint8_t *buffer, decoder_dir_t dir)

{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t pin_state;
	uint8_t terminating_char_pos;

	if (dir == DECODER_DIR_SET)
	{
		pin_state = buffer[4];
		/* configure cs pin
		 * 0->OFF-> Make the CS pin high
		 * 1->ON -> Make the CS pin low, so that SPI transaction is valid, Source:APP2.0 fw */
		coines_set_pin_config(COINES_SHUTTLE_PIN_7, COINES_PIN_DIRECTION_OUT, !pin_state);
#ifdef MCU_APP30
        coines_set_pin_config(COINES_MINI_SHUTTLE_PIN_2_1, COINES_PIN_DIRECTION_OUT, !pin_state);
#endif
	} else
		rsp = DECODER_RSP_UNKNOWN_INSTR;

	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 8; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_SPICS;

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API to set/get the interfaces to communicate with sensors
 */
static decoder_rsp_t decoder_0x11_interface(const uint8_t *buffer, decoder_dir_t dir)

{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;
	uint8_t sdo_level = 0;
	bool retval = true;

	//TODO: workaround for PCLint
	//(void)buffer;
	/* Need to decide if the interface has to be set, whether we can keep the SPI interface enabled always*/

	//TODO: Is it needed?
	uint8_t interface = 0;
	if (dir == DECODER_DIR_SET)
	{
		interface = buffer[4]; /* Get the interface number*/

		if (interface == 0) /* SPI*/
		{
			if (coines_config_spi_bus(COINES_SPI_BUS_0, COINES_SPI_SPEED_10_MHZ,
					COINES_SPI_MODE0) != COINES_SUCCESS)
			{
				rsp = DECODER_RSP_FAIL;
			}
			coines_deconfig_i2c_bus(COINES_I2C_BUS_0);

		} else if (interface == 1) /* I2C*/
		{
			sdo_level = buffer[5];
			coines_deconfig_spi_bus(COINES_SPI_BUS_0);

			if (coines_config_i2c_bus(COINES_I2C_BUS_0, COINES_I2C_STANDARD_MODE))
			{
				rsp = DECODER_RSP_FAIL;
			}

#ifdef MCU_APP30
			coines_set_pin_config(COINES_MINI_SHUTTLE_PIN_2_3, COINES_PIN_DIRECTION_OUT, sdo_level);
#else
			(void)sdo_level;
#endif
		} else /* SPI & I2C*/
		{
			/* TODO: Add COINES support for this feature. Multiple SPI & I2C */
		}
	}

	if (!retval)
		rsp = DECODER_RSP_FAIL;

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 8; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_INTERFACE; /* Feature of the Command */

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API to set/clear the LED
 */
static decoder_rsp_t decoder_0x0b_led_control(const uint8_t *buffer, decoder_dir_t dir)

{
	uint8_t led_state = 0, led_nr = 0;
	decoder_rsp_t rsp = DECODER_RSP_FAIL;
	uint8_t terminating_char_pos;

	if (dir == DECODER_DIR_SET)
	{
		led_state = buffer[4];
		led_nr = buffer[5];
		if ((led_nr >= COINES_LED_RED) && (led_nr <= COINES_LED_BLUE))
		{
			/* based on the request, turn LED on or off (led number has to be translated from user index (1-4) to board index (0-3) */
			coines_set_led(led_nr, led_state);
			rsp = DECODER_RSP_SUCCESS;
		}
	} else
		rsp = DECODER_RSP_UNKNOWN_INSTR;

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 8; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_LED_CONTROL; /* Feature of the Command */

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	(void) led_state;
	return rsp;

}
/*!
 *
 * @brief       : API used to get the board information
 */
static decoder_rsp_t decoder_0x1f_boardinfo(const uint8_t *buffer, decoder_dir_t dir)
{
	struct coines_board_info board_info;

	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	(void) buffer;

	if (dir == DECODER_DIR_GET)
	{
		coines_get_board_info(&board_info);
		/* Forming the Response */
		xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
		xmit_buffer[1] = 15; /* Number  of bytes in the packet */
		xmit_buffer[2] = 1; /* Packet Number */
		xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
		xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
		xmit_buffer[5] = (uint8_t) DECODER_ID_BOARDINFO; /* Feature of the Command */

		xmit_buffer[6] = (uint8_t) (board_info.shuttle_id >> 8); /* SID MSB */
		xmit_buffer[7] = (uint8_t) (board_info.shuttle_id); /* SID LSB */
		xmit_buffer[8] = (uint8_t) (board_info.hardware_id >> 8); /* HWID MSB */
		xmit_buffer[9] = (uint8_t) (board_info.hardware_id); /* HWID LSB */
		xmit_buffer[10] = (uint8_t) (board_info.software_id >> 8); /* SWID MSB */
		xmit_buffer[11] = (uint8_t) (board_info.software_id); /* SWID LSB */
		xmit_buffer[12] = board_info.board; /* Board Info */

		/* adds timestamp to the xmit_buffer if required */
		decoder_update_timestamp();

		terminating_char_pos = xmit_buffer[1] - 2;
		xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
		xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

		decoder_write_resp(xmit_buffer, xmit_buffer[1]);
	} else
		rsp = DECODER_RSP_UNKNOWN_INSTR;

	return rsp;

}
/*!
 *
 * @brief       : API to set shuttle vdd and vddio voltages
 */
static decoder_rsp_t decoder_0x14_shuttle_pwr_cfg(const uint8_t *buffer, decoder_dir_t dir)

{
	uint8_t vdd_state = 0, vddio_state = 0;
	uint16_t vdd_voltage = 0, vddio_voltage = 0;
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	// TODO vdd set and get actual implementation
	if (dir == DECODER_DIR_SET)
	{
		vdd_voltage = (uint16_t) ((buffer[4] << 8) | buffer[5]);
		vdd_state = buffer[6];
		vddio_voltage = (uint16_t) ((buffer[7] << 8) | buffer[8]);
		vddio_state = buffer[9];

		if ((vdd_voltage <= VDD_VDDIO_3V3) && (vdd_state <= 1) && (vddio_voltage <= VDD_VDDIO_3V3)
				&& (vddio_state <= 1))
		{
			//TODO:
			if (vdd_state == 1)
			{
				//Workaround to fix DD inputs
				if (vdd_voltage == 0)
				{
					vdd_voltage = VDD_VDDIO_2V8;
				}
				coines_set_shuttleboard_vdd_vddio_config(vdd_voltage, last_vddio);
				last_vdd = vdd_voltage;
			} else
			{
				coines_set_shuttleboard_vdd_vddio_config(0, last_vddio);
				last_vdd = 0;
			}

			if (vddio_state == 1)
			{
				//Workaround to fix DD inputs
				if (vddio_voltage == 0)
					vddio_voltage = VDD_VDDIO_1V8;
				coines_set_shuttleboard_vdd_vddio_config(last_vdd, vddio_voltage);
				last_vddio = vddio_voltage;
			} else
			{
				coines_set_shuttleboard_vdd_vddio_config(last_vdd, 0);
				last_vddio = 0;
			}
		} else
			rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;
	} else
	{
		/* workaround implementation */
		//TODO: Get command not listed in commands list, do we need to keep this ?
		if (dir == DECODER_DIR_GET)
		{
			vdd_voltage = 0;
			vdd_state = 0;
			vddio_voltage = 0;
			vddio_state = 0;
		}
	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 14; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) (rsp); /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_SHUTTLE_PWR_CFG; /* Feature of the Command */

	xmit_buffer[6] = (uint8_t) (vdd_voltage >> 8); /* vdd voltage MSB */
	xmit_buffer[7] = (uint8_t) (vdd_voltage); /* vdd voltage LSB */
	xmit_buffer[8] = vdd_state; /* vdd state */
	xmit_buffer[9] = (uint8_t) (vddio_voltage >> 8); /* vddio voltage MSB */
	xmit_buffer[10] = (uint8_t) (vddio_voltage); /* vddio voltage LSB */
	xmit_buffer[11] = vddio_state; /* vddio state */

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API to set/get IO pins
 *                APP2.0 - set/get multiple IOs status at once
 *                APP3.0 - set/get only one pin at a time !
 *                         If 15th bit is set, then pin is of APP3.0's !
 */
static decoder_rsp_t decoder_0x15_multiioconfig(const uint8_t *buffer, decoder_dir_t dir)

{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint16_t gpio_sel_mask, gpio_dir_mask, gpio_state_mask, pin_mask;
	uint8_t i;
	enum coines_pin_direction pin_dir, gpio_dir;
	enum coines_pin_value pin_state, gpio_level;

	uint8_t terminating_char_pos;
	int16_t retval = COINES_SUCCESS;

	gpio_sel_mask = (buffer[4] << 8) | buffer[5];

	if (dir == DECODER_DIR_SET)
	{
		gpio_dir_mask = (buffer[6] << 8) | buffer[7];
		gpio_state_mask = (buffer[8] << 8) | buffer[9];

		if ((gpio_sel_mask & APP30_SHUTTLE_PIN_ID) == 0)
		{

			for (i = 0; i <= DECODER_MAX_CS_PIN; i++)
			{
				pin_mask = (1 << (i & 0x0F));
				if (gpio_sel_mask & pin_mask)
				{
					/* GPIO i is requested, determine direction and state */
					pin_dir = (gpio_dir_mask & pin_mask) ?
							COINES_PIN_DIRECTION_OUT : COINES_PIN_DIRECTION_IN;
					pin_state = (gpio_state_mask & pin_mask) ?
							COINES_PIN_VALUE_HIGH : COINES_PIN_VALUE_LOW;

					/* configure GPIO i */
					retval = coines_set_pin_config(i, pin_dir, pin_state);

					if (retval == COINES_SUCCESS)
					{
						/* read back the pin state to insure pin was correctly configured */
						retval = coines_get_pin_config(i, &gpio_dir, &gpio_level);
						if ((retval != COINES_SUCCESS) || (gpio_dir != pin_dir)
								|| (gpio_level != pin_state))
						{
							rsp = DECODER_RSP_FAIL;
						}
					} else
					{
						rsp = DECODER_RSP_FAIL;
					}
				}
			}
		} else /* APP3.0 shuttle pin */
		{
			gpio_sel_mask &= ~APP30_SHUTTLE_PIN_ID;
			pin_dir = gpio_dir_mask ? COINES_PIN_DIRECTION_OUT : COINES_PIN_DIRECTION_IN;
			pin_state = gpio_state_mask ? COINES_PIN_VALUE_HIGH : COINES_PIN_VALUE_LOW;
			if (coines_set_pin_config(gpio_sel_mask, pin_dir, pin_state) != COINES_SUCCESS)
			{
				rsp = DECODER_RSP_FAIL;
			}
		}
	} else
	{
		gpio_dir_mask = 0;
		gpio_state_mask = 0;
		if ((gpio_sel_mask & APP30_SHUTTLE_PIN_ID) == 0)
		{

			for (i = 0; i < DECODER_MAX_CS_PIN; i++)
			{
				pin_mask = (1 << (i & 0x0F));
				if (gpio_sel_mask & pin_mask)
				{
					/* GPIO i is requested, determine direction and state */

					/* read the pin state */
					retval = coines_get_pin_config(i, &pin_dir, &pin_state);
					if (retval == COINES_SUCCESS)
					{
						if (pin_dir == COINES_PIN_DIRECTION_OUT)
							gpio_dir_mask |= pin_mask;

						if (pin_state == COINES_PIN_VALUE_HIGH)
							gpio_state_mask |= pin_mask;
					} else
					{
						rsp = DECODER_RSP_FAIL;
					}
				}
			}
		} else /* APP3.0 shuttle pin */
		{
			gpio_sel_mask &= ~APP30_SHUTTLE_PIN_ID;
			if (coines_get_pin_config(gpio_sel_mask, &pin_dir, &pin_state) == COINES_SUCCESS)
			{
				gpio_dir_mask = (uint16_t) pin_dir;
				gpio_state_mask = (uint16_t) pin_state;
			} else
				rsp = DECODER_RSP_FAIL;
		}
	}
	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */

	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_MULTIIOCONFIG; /* Feature of the Command */

	if (dir == DECODER_DIR_SET)
	{
		xmit_buffer[1] = 8; /* Number  of bytes in the packet */
	} else
	{
		xmit_buffer[1] = 14; /* Number  of bytes in the packet */
		xmit_buffer[6] = (uint8_t) (gpio_sel_mask >> 8);
		xmit_buffer[7] = (uint8_t) gpio_sel_mask;
		xmit_buffer[8] = (uint8_t) (gpio_dir_mask >> 8);
		xmit_buffer[9] = (uint8_t) gpio_dir_mask;
		xmit_buffer[10] = (uint8_t) (gpio_state_mask >> 8);
		xmit_buffer[11] = (uint8_t) gpio_state_mask;
	}

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;
}

/*!
 *
 * @brief       : API used to write-to/read-from sensor
 */
static decoder_rsp_t decoder_0x16_writeread(const uint8_t *buffer, decoder_dir_t dir)

{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	int8_t comm_result = DECODER_RET_COMM_FAILED;
	comm_function comm_read, comm_write;
	uint8_t deviceaddress, pkt;
	uint32_t tx_sent_count, tx_remaining_count, tx_chunk_size; /* Count to check the number of packets */
	uint8_t i, iter;

	uint8_t mode = buffer[4];
	uint8_t bus_select = buffer[5];
	uint8_t sensorID = buffer[6];
	uint8_t regaddress = buffer[10];
	uint8_t repeatcount = buffer[13]; /* No of times to write particular register */
	uint8_t delaycount = buffer[14]; /* Delay between write(ms) */
	uint8_t responsetype = buffer[15];
	uint16_t regcount = (uint16_t) ((buffer[11] << 8) | buffer[12]); /* number of bytes of data to read/write */
	uint64_t current_ts;
	uint8_t last_packet = 0;

	//TODO: handle ASW byte

	/* pre-validation of parameter values */

	if ((mode < 1) || (mode > 3))
		return DECODER_RSP_PARAM_OUT_OF_RANGE;
	if (bus_select > 32)
		return DECODER_RSP_PARAM_OUT_OF_RANGE;

	if (bus_select == 0) /* I2C bus */
	{
		deviceaddress = buffer[9]; /* I2C address, considering only the LSB (lower 8 bits) */
		comm_read = sensor_i2c_read;
		comm_write = sensor_i2c_write;
	} else if (bus_select == 1) /* SPI (CS Sensor) */
	{
		deviceaddress = 9; /* CS pin number */
		comm_read = sensor_spi_read;
		comm_write = sensor_spi_write;
	} else if (1 < bus_select && bus_select < 16)/* SPI (CS_MULTIOx) */
	{
		deviceaddress = bus_select - 2; /* CS pin number */
		comm_read = sensor_spi_read;
		comm_write = sensor_spi_write;
	} else /* APP3.0 shuttle pin */
	{
		deviceaddress = bus_select;
		comm_read = sensor_spi_read;
		comm_write = sensor_spi_write;
	}

	if (dir == DECODER_DIR_SET)
	{
		/* Dynamic lenght check - base size is 18 bytes + sensor_command payload */
		if (decoder_calc_len(buffer) != (18 + regcount))
			return DECODER_RSP_SIZE_MISMATCH;
	} else if (dir == DECODER_DIR_GET)
	{
		/* Dynamic lenght check - base size is 18 bytes + sensor_command payload */
		if (decoder_calc_len(buffer) != 18)
			return DECODER_RSP_SIZE_MISMATCH;
	} else
	{
		return DECODER_RSP_PARAM_OUT_OF_RANGE;
	}

	/* handle the SET command */
	if (dir == DECODER_DIR_SET)
	{
		/* Copy/accumulate the sensor command payload in the global buffer */
		memcpy(sensor_cmd_buffer + sensor_cmd_size, &buffer[16], regcount);
		sensor_cmd_size += regcount;

		/* in case of continuous burst mode, return after data was accumulated */
		if (mode == DECODER_RW_MODE_BURST_ACCUMULATE)
			return decoder_error_fuction(DECODER_RSP_SUCCESS, (int8_t) DECODER_ID_WRITEREAD, (int8_t) dir);

		/* execute the command for as many times as requested (according to writecount parameter) */
		for (iter = 0; iter < repeatcount; iter++)
		{
			if ((mode == DECODER_RW_MODE_NORMAL) || (delaycount > 0))
			{
				/* write byte-by-byte, with an optional delay in-between the bytes (note: the entire accumulated buffer is written) */
				for (i = 0; i < sensor_cmd_size; i++)
				{
					comm_result = comm_write(deviceaddress, regaddress + i, sensor_cmd_buffer + i,
							1);
					coines_delay_msec(delaycount);
				}
			} else
			{
				/* write the entire length in one chunk (note: the entire accumulated buffer is written) */
				comm_result = comm_write(deviceaddress, regaddress, sensor_cmd_buffer, sensor_cmd_size);
			}
		}
	}

	/* if the command is a GET or if the previous SET command has requested read-back, then execute a read operation */
	if ((dir == DECODER_DIR_GET) || ((dir == DECODER_DIR_SET) && (responsetype == 1)))
	{
		/* continuous burst mode not possible for reads */
		if (mode == DECODER_RW_MODE_BURST_ACCUMULATE)
			return DECODER_RSP_PARAM_OUT_OF_RANGE;

		sensor_cmd_size = regcount;

		/* execute the command for as many times as requested (according to repeatcount parameter) */
		for (iter = 0; iter < repeatcount; iter++)
		{
			if ((mode == DECODER_RW_MODE_NORMAL) || (delaycount > 0))
			{
				/* read byte-by-byte, with a delay in-between the bytes */
				for (i = 0; i < sensor_cmd_size; i++)
				{
					comm_result = comm_read(deviceaddress, regaddress + i, &sensor_cmd_buffer[i],
							1);
					coines_delay_msec(delaycount);
				}
			} else
			{
				/* read the entire length in one chunk */
				comm_result = comm_read(deviceaddress, regaddress, sensor_cmd_buffer, sensor_cmd_size);
			}
		}
	}

	if (comm_result == 0)
		rsp = DECODER_RSP_SUCCESS;
	else
		rsp = DECODER_RSP_FAIL;

	/* TODO: Need to handle timestamp request */
	if (use_timestamp == 1)
	{
		current_ts = coines_get_micro_sec();
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 40);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 32);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 24);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 16);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 8);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts);
	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */

	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_WRITEREAD; /* Feature of the Command */
	xmit_buffer[6] = sensorID; /* Sensor ID */
	xmit_buffer[9] = repeatcount; /* No of times to write particular register*/
	xmit_buffer[10] = delaycount; /* Delay between write(ms)  */

	pkt = 1;
	tx_sent_count = 0;
	while (tx_sent_count < sensor_cmd_size)
	{
		xmit_buffer[2] = pkt; /* Packet Number */

		if (sensor_cmd_size > 128)
			xmit_buffer[7] = regaddress; /* done for read more than 128 bytes, where starting register address will be sent*/
		else
			xmit_buffer[7] = (uint8_t) (regaddress + tx_sent_count);

		tx_remaining_count = sensor_cmd_size - tx_sent_count;
		if (tx_remaining_count > 51)
			tx_chunk_size = 51;
		else
		{
			tx_chunk_size = tx_remaining_count;
			last_packet = 1;

		}

		/* copy payload */
		memcpy(&xmit_buffer[11], &sensor_cmd_buffer[tx_sent_count], tx_chunk_size);

		xmit_buffer[8] = (uint8_t) tx_chunk_size;
		if (use_timestamp == 1)
		{
			/* fill-in size information for payload and package */
			if (last_packet == 1)
			{
				xmit_buffer[8] = (uint8_t) tx_chunk_size - 6; /* Payload size */
			}
		}

		xmit_buffer[1] = (uint8_t) (11 + tx_chunk_size + 2); /* Total number of bytes in the packet */

		/* add terminator CR+LF */
		xmit_buffer[11 + tx_chunk_size] = DECODER_CR_MACRO; /* \r */
		xmit_buffer[11 + tx_chunk_size + 1] = DECODER_LF_MACRO; /* \n */

		/* transmit data to PC */
		decoder_write_resp(xmit_buffer, xmit_buffer[1]);

		tx_sent_count += tx_chunk_size;
		pkt++;
	}

	sensor_cmd_size = 0;

	return rsp;

}

/*!
 *
 * @brief       : API to set/get spi configurations
 */
static decoder_rsp_t decoder_0x19_spiconfig(const uint8_t *buffer, decoder_dir_t dir)
{

	uint8_t bus_index = 0, bus_speed = 0, bus_mode = 0, bus_length = 0;
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	/*check if it is a command set*/
	if (dir == DECODER_DIR_SET)
	{
		/* get the bus index, spi mode and spi speed*/
		bus_index = buffer[4];
		bus_mode = buffer[5];
		bus_length = buffer[6];
		bus_speed = buffer[7];

		/* check if the bus index is 1*/
		if (bus_index == 1)
		{
			/* check if it is valid SPI bus mode*/
			if (bus_mode < 4)
			{
				if (coines_config_spi_bus(COINES_SPI_BUS_0, bus_speed, bus_mode) != COINES_SUCCESS)
				{
					rsp = DECODER_RSP_FAIL;
				}
			} else
				rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;
		} else
			rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;
	} else
		rsp = DECODER_RSP_UNKNOWN_INSTR;

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 12; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_SPICONFIG;
	xmit_buffer[6] = bus_index;
	xmit_buffer[7] = bus_mode;
	xmit_buffer[8] = bus_length;
	xmit_buffer[9] = bus_speed;

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	/* Function is void, since possibility of overflowing for non-streaming command is less*/
	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API to provide delay
 */
static decoder_rsp_t decoder_0x1a_delayms(const uint8_t *buffer, decoder_dir_t dir)
{

	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;
	uint32_t delay = ((uint32_t) buffer[4] << 24 | buffer[5] << 16 | buffer[6] << 8 | buffer[7]);

	coines_delay_msec(delay);

	xmit_buffer[0] = 0xAA;
	xmit_buffer[1] = 8;
	xmit_buffer[2] = 1;
	xmit_buffer[3] = 0;
	xmit_buffer[4] = DECODER_RSP_ID_MASK | (uint8_t) dir;
	xmit_buffer[5] = (uint8_t) DECODER_ID_DELAY;

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;

}
/*!
 *
 * @brief       : API used to write-to/read-from sensor with delay
 */
static decoder_rsp_t decoder_0x22_writedelayread(const uint8_t *buffer, decoder_dir_t dir)
{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	int8_t comm_result = DECODER_RET_COMM_FAILED;
	comm_function comm_read, comm_write;
	uint8_t deviceaddress, pkt;
	uint32_t tx_sent_count; /* Count to check the number of packets */
	uint8_t i;

	uint8_t mode = buffer[4];
	uint8_t bus_select = buffer[5];
	uint8_t sensorID = buffer[6];
	uint8_t regaddress = buffer[10];
	uint16_t delaycount = (uint16_t) ((buffer[13] << 8) | buffer[14]); /* Delay between write(ms) */
	uint8_t responsetype = buffer[15];
	uint16_t regcount = (uint16_t) ((buffer[11] << 8) | buffer[12]); /* number of bytes of data to read/write */
	uint64_t current_ts;

	//TODO: handle ASW byte

	/* pre-validation of parameter values */

	if ((mode < 1) || (mode > 3))
		return decoder_error_fuction(DECODER_RSP_PARAM_OUT_OF_RANGE, (int8_t) DECODER_ID_WRITEDELAYREAD,
				(int8_t) dir);
	if (bus_select > 32)
		return decoder_error_fuction(DECODER_RSP_PARAM_OUT_OF_RANGE, (int8_t) DECODER_ID_WRITEDELAYREAD,
				(int8_t) dir);

	if (bus_select == 0) /* I2C bus */
	{
		deviceaddress = buffer[9]; /* I2C address, considering only the LSB (lower 8 bits) */
		comm_read = sensor_i2c_read;
		comm_write = sensor_i2c_write;
	} else if (bus_select == 1) /* SPI (CS Sensor) */
	{
		deviceaddress = 9; /* CS pin number */
		comm_read = sensor_spi_read;
		comm_write = sensor_spi_write;
	} else if (1 < bus_select && bus_select < 16)/* SPI (CS_MULTIOx) */
	{
		deviceaddress = bus_select - 2; /* CS pin number */
		comm_read = sensor_spi_read;
		comm_write = sensor_spi_write;
	} else /* APP3.0 only */
	{
		deviceaddress = bus_select;
		comm_read = sensor_spi_read;
		comm_write = sensor_spi_write;
	}

	if (dir == DECODER_DIR_SET)
	{
		/* Dynamic lenght check - base size is 18 bytes + sensor_command payload */
		if (decoder_calc_len(buffer) != (18 + regcount))
			return decoder_error_fuction(DECODER_RSP_SIZE_MISMATCH, (int8_t) DECODER_ID_WRITEDELAYREAD,
					(int8_t) dir);
	} else
	{
		return decoder_error_fuction(DECODER_RSP_UNKNOWN_INSTR, (int8_t) DECODER_ID_WRITEDELAYREAD,
				(int8_t) dir);

	}

	/* handle the SET command */
	if (dir == DECODER_DIR_SET)
	{
		/* Copy/accumulate the sensor command payload in the global buffer */
		memcpy(sensor_cmd_buffer + sensor_cmd_size, &buffer[16], regcount);
		sensor_cmd_size = regcount;

		/* execute the command for as many times as requested (according to writecount parameter) */
		if (mode == DECODER_RW_MODE_NORMAL)
		{
			for (i = 0; i < sensor_cmd_size; i++)
			{
				/* write byte-by-byte, with an optional delay in-between the bytes (note: the entire accumulated buffer is written) */
				comm_result = comm_write(deviceaddress, regaddress + i, sensor_cmd_buffer + i, 1);
			}

		} else
		{
			/* write the entire length in one chunk (note: the entire accumulated buffer is written) */
			comm_result = comm_write(deviceaddress, regaddress, sensor_cmd_buffer, sensor_cmd_size);
		}

		coines_delay_msec(delaycount);

		if ((responsetype == 1) && (comm_result == 0))
		{
			/* read the entire length in one chunk */
			comm_result = comm_read(deviceaddress, regaddress, sensor_cmd_buffer, sensor_cmd_size);
		}

		if (comm_result != 0)
			return decoder_error_fuction(DECODER_RSP_FAIL, (int8_t) DECODER_ID_WRITEDELAYREAD, (int8_t) dir);

	}

	/* adds timestamp to the xmit_buffer if required */
	// process_timestamp();
	/* TODO: Need to handle timestamp request */
	if (use_timestamp == 1)
	{
		current_ts = coines_get_micro_sec();
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 40);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 32);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 24);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 16);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 8);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts);
	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */

	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_WRITEDELAYREAD; /* Feature of the Command */
	xmit_buffer[6] = sensorID; /* Sensor ID */
	xmit_buffer[9] = (delaycount >> 8) & 0xff; /* No of times to write particular register*/
	xmit_buffer[10] = delaycount & 0xff; /* Delay between write(ms)  */

	pkt = 1;
	tx_sent_count = 0;
	xmit_buffer[2] = pkt; /* Packet Number */

	xmit_buffer[7] = (uint8_t) (regaddress + tx_sent_count);

	/* copy payload */
	memcpy(&xmit_buffer[11], &sensor_cmd_buffer[tx_sent_count], sensor_cmd_size);

	if (use_timestamp == 1)
		xmit_buffer[8] = (uint8_t) sensor_cmd_size - 6;
	else
		xmit_buffer[8] = (uint8_t) sensor_cmd_size;
	xmit_buffer[1] = (uint8_t) (11 + sensor_cmd_size + 2); /* Total number of bytes in the packet */

	/* add terminator CR+LF */
	xmit_buffer[11 + sensor_cmd_size] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[11 + sensor_cmd_size + 1] = DECODER_LF_MACRO; /* \n */

	/* transmit data to PC */
	decoder_write_resp(xmit_buffer, xmit_buffer[1]);
	sensor_cmd_size = 0;

	return rsp;
}
/*!
 *
 * @brief       : API used to write-to/read-from third party sensor
 */
static decoder_rsp_t decoder_0x28_thirdparty_writeread(const uint8_t *buffer, decoder_dir_t dir)
{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	int8_t comm_result = DECODER_RET_COMM_FAILED;
	uint8_t mode = buffer[4];
	uint8_t bus_select = buffer[5];
	uint8_t sensorID = buffer[6];
	uint16_t deviceaddress = 0;
	uint8_t number_of_opcodes = buffer[10];
	uint8_t operation_mode = buffer[11];
	uint8_t i, pkt;
	uint32_t tx_sent_count, tx_remaining_count, tx_chunk_size; /* Count to check the number of packets */
	uint8_t last_packet = 0;
	uint64_t current_ts;
	/* pre-validation of parameter values */

	if ((mode < 1) || (mode > 3))
		return decoder_error_fuction(DECODER_RSP_PARAM_OUT_OF_RANGE, (int8_t) DECODER_ID_THIRDPARTYWRITEREAD,
				(int8_t) dir);
	if (bus_select > 10)
		return decoder_error_fuction(DECODER_RSP_PARAM_OUT_OF_RANGE, (int8_t) DECODER_ID_THIRDPARTYWRITEREAD,
				(int8_t) dir);

	if (bus_select == 0) /* I2C bus */
	{
		deviceaddress = (buffer[8] << 8) | buffer[9]; /* I2C address, considering only the LSB (lower 8 bits) */
	}

	if (dir == DECODER_DIR_SET)
	{
		/* Dynamic lenght check - base size is 15 bytes + sensor_command payload */
		if (decoder_calc_len(buffer) != (17 + number_of_opcodes))
			return decoder_error_fuction(DECODER_RSP_SIZE_MISMATCH, (int8_t) DECODER_ID_THIRDPARTYWRITEREAD,
					(int8_t) dir);

		/* Copy/accumulate the sensor command payload in the global buffer */
		memcpy(sensor_cmd_buffer + sensor_cmd_size, &buffer[15], number_of_opcodes);
		sensor_cmd_size += number_of_opcodes;

		if (operation_mode == 4)
		{
			if (mode == DECODER_RW_MODE_NORMAL)
			{
				for (i = 0; i < sensor_cmd_size; i++)
				{
					comm_result = coines_i2c_set(COINES_I2C_BUS_0, deviceaddress, &sensor_cmd_buffer[0], i);
				}
			} else
			{
				comm_result = coines_i2c_set(COINES_I2C_BUS_0, deviceaddress, &sensor_cmd_buffer[0],
						sensor_cmd_size);
			}
		}

		if (comm_result == 0)
		{
			if (coines_i2c_get(COINES_I2C_BUS_0, deviceaddress, &sensor_cmd_buffer[0], sensor_cmd_size) != 0)
			{
				sensor_cmd_size = 0;
				return decoder_error_fuction(DECODER_RSP_FAIL, (int8_t) DECODER_ID_THIRDPARTYWRITEREAD,
						(int8_t) dir);
			}
		} else
		{
			sensor_cmd_size = 0;
			return decoder_error_fuction(DECODER_RSP_FAIL, (int8_t) DECODER_ID_THIRDPARTYWRITEREAD,
					(int8_t) dir);
		}
	}

	/* TODO: Need to handle timestamp request */
	if (use_timestamp == 1)
	{
		current_ts = coines_get_micro_sec();
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 40);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 32);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 24);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 16);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts >> 8);
		sensor_cmd_buffer[sensor_cmd_size++] = (uint8_t) (current_ts);
	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */

	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_THIRDPARTYWRITEREAD; /* Feature of the Command */
	xmit_buffer[6] = sensorID; /* Sensor ID */
	//xmit_buffer[7] = sensor_cmd_size; /* No of bytes*/

	pkt = 1;
	tx_sent_count = 0;

	while (tx_sent_count < sensor_cmd_size)
	{
		xmit_buffer[2] = pkt; /* Packet Number */

		tx_remaining_count = sensor_cmd_size - tx_sent_count;
		if (tx_remaining_count > 54)
			tx_chunk_size = 54;
		else
		{
			tx_chunk_size = tx_remaining_count;
			last_packet = 1;

		}

		/* copy payload */
		memcpy(&xmit_buffer[8], &sensor_cmd_buffer[tx_sent_count], tx_chunk_size);

		xmit_buffer[7] = (uint8_t) tx_chunk_size;
		if (use_timestamp == 1)
		{
			/* fill-in size information for payload and package */
			if (last_packet == 1)
			{
				xmit_buffer[7] = (uint8_t) tx_chunk_size - 6; /* Payload size */
			}
		}

		xmit_buffer[1] = (uint8_t) (8 + tx_chunk_size + 2); /* Total number of bytes in the packet */

		/* add terminator CR+LF */
		xmit_buffer[8 + tx_chunk_size] = DECODER_CR_MACRO; /* \r */
		xmit_buffer[8 + tx_chunk_size + 1] = DECODER_LF_MACRO; /* \n */

		/* transmit data to PC */
		decoder_write_resp(xmit_buffer, xmit_buffer[1]);

		tx_sent_count += tx_chunk_size;
		pkt++;
	}

	sensor_cmd_size = 0;

	return rsp;
}

/*!
 *
 * @brief       : API to set use_timestamp
 */
static decoder_rsp_t decoder_0x29_timestamp(const uint8_t *buffer, decoder_dir_t dir)
{
	//TODO:
	uint8_t timestamp_parameter;
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	//TODO: check if DIR_SET is correct here
	if (dir == DECODER_DIR_GET)
	{
		timestamp_parameter = buffer[4];

		//TODO: implement
		if (timestamp_parameter == 0x03)
		{
			use_timestamp = 1;
			stream_settings.ts_mode = STREAM_USE_TIMESTAMP;
		} else if (timestamp_parameter == 0x04)
		{
			use_timestamp = 0;
			stream_settings.ts_mode = STREAM_NO_TIMESTAMP;
		} else
		{
			rsp = DECODER_RSP_UNKNOWN_INSTR;
		}
	} else
	{
		timestamp_parameter = buffer[4];
		//TODO: Needs to be done after integration of Time capture.
		if (timestamp_parameter == 0x00)
		{
			//TODO
		} else if ((timestamp_parameter == 0x01))
		{
			//TODO
		} else
		{
			if (timestamp_parameter == 0x02)
			{
				//TODO
			} else
				rsp = DECODER_RSP_UNKNOWN_INSTR;
		}

	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 8; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
	xmit_buffer[5] = (uint8_t) DECODER_ID_TIMESTAMP;

	/* adds timestamp to the xmit_buffer if required */
	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	/* Function is void, since possibility of overflowing for non-streaming command is less*/
	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;
}

/* Regarding info. on Application Switch,
 * see https://bcds02.de.bosch.com/confluence/display/DEV/USB+DFU+Bootloader
 * (Invoking Bootloader from software)
 */
/*!
 *
 * @brief       : API to set switch from application to BTL
 */
static decoder_rsp_t decoder_0x30_app_switch(const uint8_t *buffer, decoder_dir_t dir)
{
	uint32_t appstart_addr = 0;

	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;

	if (dir == DECODER_DIR_SET)
	{
		appstart_addr = buffer[4] << 24;
		appstart_addr |= buffer[5] << 16;
		appstart_addr |= buffer[6] << 8;
		appstart_addr |= buffer[7];

		if (appstart_addr == 0)
			appstart_addr = 0xF0000;

		if (((appstart_addr >= 0x28000 && appstart_addr < 0x100000) || /*Check for valid flash address*/
		(appstart_addr >= 0x20000000 && appstart_addr < 0x20040000)) && /*Check for valid RAM address*/
		appstart_addr % 2 == 0 /*Check if address if even*/
		)
		{
#ifndef UNIT_TEST
			memcpy(DECODER_BTL_MAGIC_INFO_ADDR, DECODER_BTL_MAGIC_DATA, 4); /* Write Magic info. "COIN" */
			DECODER_BTL_APP_START_ADDR = appstart_addr;
			if (DECODER_APP_SP_VALUE > 0x20000000 && DECODER_APP_SP_VALUE < 0x20040000) /*Check if the application has vaild Stack pointer value*/
			{
				coines_delay_msec(10);
				coines_soft_reset();
			} else
				rsp = decoder_error_fuction(DECODER_RSP_INVALID_APPLICATION,
						(int8_t) DECODER_ID_APP_SWITCH, (int8_t) dir); //Invaild application
#endif
		} else
		{
			rsp = decoder_error_fuction(DECODER_RSP_INVALID_ADDRESS, (int8_t) DECODER_ID_APP_SWITCH,
					(int8_t) dir); //Invaild address
		}
	} else
	{
		rsp = decoder_error_fuction(DECODER_RSP_UNKNOWN_INSTR, (int8_t) DECODER_ID_APP_SWITCH, (int8_t) dir);
	}

	return rsp;
}

/*!
 *
 * @brief       : API to invoke BTL
 */
static decoder_rsp_t decoder_0x31_invoke_btl(const uint8_t *buffer, decoder_dir_t dir)
{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	(void) buffer;

	if (dir != DECODER_DIR_SET)
	{
		rsp = decoder_error_fuction(DECODER_RSP_UNKNOWN_INSTR, (int8_t) DECODER_ID_INVOKE_BTL, (int8_t) dir);
	} else
	{
#ifndef UNIT_TEST
		memcpy(DECODER_BTL_MAGIC_INFO_ADDR, DECODER_BTL_MAGIC_DATA, 4); /* Write Magic info. "COIN" */
		DECODER_BTL_APP_START_ADDR = 0;
		coines_delay_msec(10);
		coines_soft_reset();
#endif
	}
	return rsp;
}
/*!
 *
 * @brief       : API to write-to/read-from shuttle eeprom
 */
static decoder_rsp_t decoder_0x32_shuttle_eeprom_rw(const uint8_t *buffer, decoder_dir_t dir)
{
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;

	uint8_t pkts_to_send = 0;
	uint8_t remainingbytes = 0;
	uint8_t tx_sent_count = 0;
	uint8_t pkt = 1;
	uint8_t eeprom_data_len = buffer[6];
	uint8_t eeprom_addr = buffer[5]; /* eeprom address of read/write */

	bool eepromRetVal = 0;

	if (dir == DECODER_DIR_SET)
	{
		eepromRetVal = app30_eeprom_write(eeprom_addr, (uint8_t*) buffer + 7, eeprom_data_len);
		if (!eepromRetVal)
		{
			rsp = DECODER_RSP_EEPROM_ERROR;
		}
		return decoder_error_fuction(rsp, (int8_t) DECODER_ID_SHUTTLE_EEPROM_RW, (int8_t) dir);
	} else if (dir == DECODER_DIR_GET)
	{
		/* eeprom_data_len = 0 : In case of read 0 bytes */
		if ((eeprom_addr + eeprom_data_len > SHUTTLE_BOARD_EEPROM_TOTAL_LEN) || (eeprom_data_len == 0))
		{
			rsp = DECODER_RSP_SIZE_MISMATCH;
			return decoder_error_fuction(rsp, (int8_t) DECODER_ID_SHUTTLE_EEPROM_RW, (int8_t) dir);
		} else
		{
			pkts_to_send = eeprom_data_len / 53;
			remainingbytes = eeprom_data_len % 53;

			while (tx_sent_count < eeprom_data_len)
			{
				/* Forming the Response */
				xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
				xmit_buffer[2] = pkt; /* Packet of Command */
				xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
				xmit_buffer[4] = (uint8_t) dir | DECODER_RSP_ID_MASK; /* Direction of the Command */
				xmit_buffer[5] = (uint8_t) DECODER_ID_SHUTTLE_EEPROM_RW; /* Feature of the Command */
				xmit_buffer[6] = 0x00;
				xmit_buffer[7] = eeprom_addr; /* Address */
				xmit_buffer[1] = 64;
				xmit_buffer[9 + eeprom_data_len] = DECODER_CR_MACRO; /* \r */
				xmit_buffer[9 + eeprom_data_len + 1] = DECODER_LF_MACRO; /* \n */

				if (pkts_to_send)
				{
					eepromRetVal = app30_eeprom_read(eeprom_addr, xmit_buffer + 9, 53);
					pkts_to_send--;
					eeprom_addr += 53;
					tx_sent_count = tx_sent_count + 53;
					xmit_buffer[8] = 53; /* Data Length */
				} else if (remainingbytes)
				{
					eepromRetVal = app30_eeprom_read(eeprom_addr, xmit_buffer + 9, remainingbytes);
					xmit_buffer[1] = 11 + remainingbytes;
					tx_sent_count = tx_sent_count + remainingbytes;
					xmit_buffer[8] = remainingbytes; /* Data Length */
				}
				if (!eepromRetVal)
				{
					rsp = DECODER_RSP_EEPROM_ERROR;
					return decoder_error_fuction(rsp, (int8_t) DECODER_ID_SHUTTLE_EEPROM_RW,
							(int8_t) dir);
				}
				/* transmit data to PC */
				decoder_write_resp(xmit_buffer, xmit_buffer[1]);
				pkt++;
			}
		}
	} else
	{
		rsp = DECODER_RSP_PARAM_OUT_OF_RANGE;
	}
	return rsp;
}
/*!
 *
 * @brief       : API to read the global sampling timer period - used for all polling streaming
 */
static decoder_rsp_t decoder_ext0x03_pollstream_common(const uint8_t *buffer, decoder_dir_t dir)
{

	(void) dir;
	uint8_t terminating_char_pos;
	uint16_t raw_time;
	decoder_rsp_t ret_val = DECODER_RSP_SUCCESS;

	/* read the global sampling timer period - used for all polling sensors */
	raw_time = (buffer[5] << 8) | buffer[6];
	if (buffer[7] == (uint8_t) STREAM_TIMEUNIT_US)
		stream_settings.GST_period_us = raw_time;
	else if (buffer[7] == (uint8_t) STREAM_TIMEUNIT_MS)
		stream_settings.GST_period_us = raw_time * 1000;
	else
		return decoder_error_fuction(DECODER_RSP_PARAM_OUT_OF_RANGE, 0, buffer[2]);

	stream_settings.stream_mode = STREAM_MODE_POLLING;
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 7; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) ret_val; /* Success message*/
	xmit_buffer[4] = 0x43; /* Stream command ID */

	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return ret_val;

}
/*!
 *
 * @brief       : API to start/stop streaming
 */
static decoder_rsp_t decoder_ext0x0a_ext0x06_ext0x08_stream_startstop(const uint8_t *buffer, decoder_dir_t dir)
{
	(void) dir;
	decoder_rsp_t rsp = DECODER_RSP_SUCCESS;
	uint8_t terminating_char_pos;

	if (stream_active_count > 0)
	{
		if (buffer[3] == 0)
		{
			/* Stop Command */
			stream_stop();
			decoder_stream_samples_count = 0;
			timeout_required = 0;
		} else if (buffer[3] == 0xFF)
		{
			decoder_stream_samples_count = 0xff;
			stream_start();
		} else if (buffer[3] == 1)
		{
			decoder_stream_samples_count = 1;
			stream_start();
		} else
		{
			if (buffer[3] == 2)
			{
				//TODO see where this use case is used
			}
		}

	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 7; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) rsp; /* Success message*/
	xmit_buffer[4] = DECODER_RSP_ID_MASK | buffer[2]; /* Stream command ID */

	decoder_update_timestamp();

	terminating_char_pos = xmit_buffer[1] - 2;
	xmit_buffer[terminating_char_pos++] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[terminating_char_pos] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return rsp;
}
/*!
 *
 * @brief       : API to get and configure polling settings for old sensors
 */
static decoder_rsp_t decoder_ext0x04_pollstream_old_read(const uint8_t *buffer, decoder_dir_t dir)
{
	decoder_rsp_t ret_val = DECODER_RSP_SUCCESS;
	(void) dir;
	uint8_t stream_idx;
	uint16_t base_idx, raw_time;
	uint32_t total_chunk_sz = 0;
	stream_descriptor_t *desc_p = stream_descriptors;
	stream_chunkinfo_t chunk_detail[2] = { { 0 } };
	//TODO: Need to use this flag at later point of time
	//uint8_t prepost_operation_flag;

	if (stream_active_count >= STREAM_MAX_COUNT)
	{
		ret_val = DECODER_RSP_NO_FREE_STREAM_CHANNEL;
		goto pollstream_response;
	}

	stream_idx = stream_active_count;
	desc_p = &stream_descriptors[stream_idx];

	desc_p->channel_id = buffer[3] - 1;
	desc_p->mode = STREAM_MODE_POLLING;
	desc_p->param_interface = (stream_if_t) buffer[5];
	if (desc_p->param_interface == STREAM_IF_CS_SENSOR)
	{
		desc_p->param_interface = (stream_if_t) 9;
	} else if ((desc_p->param_interface >= STREAM_IF_MULTIIO_0) && (desc_p->param_interface <= STREAM_IF_MULTIIO_8))
	{
		/*lint -save -e656 arithmetic operation uses enum's*/
		desc_p->param_interface -= (stream_if_t) 2;
		/*lint -restore*/
	}
	desc_p->dev_address = (buffer[7] << 8) | buffer[8];

	/* read streaming_period and its timebase */
	raw_time = (buffer[9] << 8) | buffer[10];
	if (buffer[11] == (uint8_t) STREAM_TIMEUNIT_US)
		desc_p->sampling_period_us = raw_time;
	else if (buffer[11] == (uint8_t) STREAM_TIMEUNIT_MS)
		desc_p->sampling_period_us = raw_time * 1000;
	else
	{
		ret_val = DECODER_RSP_PARAM_OUT_OF_RANGE;
		goto pollstream_response;
	}

	/* This is default legacy read mode used for polling streaming*/
	desc_p->read_mode = STREAM_READ_2_CHUNKS_POLLING;

	/* Use the same concept of chunk here, but check only 2 chunks Read chunk descriptors */
	//TODO: think about a common pool of chunk data structs instead of dynamic alloc
	desc_p->chunk_count = 0;

	for (uint8_t chunk_idx = 0; chunk_idx < 2; chunk_idx++)
	{
		/* calculate chunk data offset and get chunk pointer */
		base_idx = (uint16_t) (12 + chunk_idx * 2);

		/* read address and size of chunk */
		chunk_detail[chunk_idx].startaddress = buffer[base_idx];
		chunk_detail[chunk_idx].num_bytes_to_read = (uint8_t) buffer[base_idx + 1];

		if (chunk_detail[chunk_idx].num_bytes_to_read)
		{
			desc_p->chunk_count++;
		}
		/* accumulate chuck data size to a global size */
		total_chunk_sz += chunk_detail[chunk_idx].num_bytes_to_read;
	}
	desc_p->total_data_size = total_chunk_sz;
	desc_p->chunks = (stream_chunkinfo_t*) stream_memory_alloc(desc_p->chunk_count * sizeof(stream_chunkinfo_t));

	for (uint8_t chunk_idx = 0; chunk_idx < desc_p->chunk_count; chunk_idx++)
	{
		memcpy(&desc_p->chunks[chunk_idx], &chunk_detail[chunk_idx], sizeof(stream_chunkinfo_t));
	}

	base_idx += 2;
	desc_p->intline_count = buffer[base_idx++];
	if (desc_p->intline_count > 0)
	{
		/* read intline descriptors */
		desc_p->intline_info = (uint8_t*) stream_memory_alloc(desc_p->intline_count * sizeof(uint8_t));

		memcpy(desc_p->intline_info, &buffer[base_idx], desc_p->intline_count);
	}
	/* add the interrupt data to the total data size */
	desc_p->total_data_size += desc_p->intline_count;

	//TODO:prepost operation flag operation
	// prepost_operation_flag = buffer[19];

	pollstream_response:
	if (ret_val == DECODER_RSP_SUCCESS)
	{
		/* increase count at the end, if everything was successful */
		++stream_active_count;
	} else
	{
		//TODO: deallocate memory and do a graceful exit (maybe call a function to handle a complete channel delete, called also on stop streaming
	}

	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 8; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) ret_val; /* Success message*/
	xmit_buffer[4] = 0x44; /* Stream command ID */
	xmit_buffer[5] = desc_p->channel_id + 1;

	decoder_update_timestamp();

	uint8_t term_pos = xmit_buffer[1] - 2;
	xmit_buffer[term_pos] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[term_pos + 1] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return ret_val;
}
/*!
 *
 * @brief       : API to get and configure fifo stream settings
 */
static decoder_rsp_t decoder_ext0x07_fifostream_read(const uint8_t *buffer, decoder_dir_t dir)
{
	// uint8_t stream_idx;
	uint16_t raw_time;

	stream_fifo_descriptor_t *desc_p;
	decoder_rsp_t ret_val = DECODER_RSP_SUCCESS;

	if (stream_active_count >= STREAM_MAX_COUNT)
	{
		ret_val = DECODER_RSP_NO_FREE_STREAM_CHANNEL;
		goto fifostream_response;
	}

	/* Make the stream setting mode as FIFO stream*/
	stream_settings.stream_mode = STREAM_MODE_FIFO_POLLING;
	// stream_idx = active_stream_count;
	desc_p = &stream_fifo_descriptors;

	desc_p->feature = (stream_fifo_read_t) buffer[3];
	desc_p->mode = STREAM_MODE_FIFO_POLLING;
	desc_p->param_interface = (stream_if_t) buffer[4];
	if (desc_p->param_interface == STREAM_IF_CS_SENSOR)
	{
		desc_p->param_interface = (stream_if_t) 9;
	} else if ((desc_p->param_interface >= STREAM_IF_MULTIIO_0) && (desc_p->param_interface <= STREAM_IF_MULTIIO_8))
	{
		/*lint -save -e656 arithmetic operation uses enum's*/
		desc_p->param_interface -= (stream_if_t) 2;
		/*lint -restore*/
	}
	desc_p->dev_address = (buffer[6] << 8) | buffer[7];

	/* read streaming_period and its timebase */
	raw_time = (buffer[8] << 8) | buffer[9];
	if (buffer[10] == (uint8_t) STREAM_TIMEUNIT_US)
		desc_p->sampling_period_us = raw_time;
	else if (buffer[10] == (uint8_t) STREAM_TIMEUNIT_MS)
		desc_p->sampling_period_us = raw_time * 1000;
	else
	{
		ret_val = DECODER_RSP_PARAM_OUT_OF_RANGE;
		goto fifostream_response;
	}
	stream_settings.GST_period_us = desc_p->sampling_period_us;
	desc_p->fifo_reg_address = buffer[11];

	desc_p->number_bytes = (buffer[12] << 8) | buffer[13];
	/* update spi type 8 or 16 bit */
    desc_p->spi_type = buffer[14];
	/* number of interrupt lines*/
	desc_p->intline_count = buffer[15];

	if (desc_p->intline_count > 0)
	{
		/* read intline descriptors */
		desc_p->intline_info = (uint8_t*) stream_memory_alloc(desc_p->intline_count * sizeof(uint8_t));

		memcpy(desc_p->intline_info, &buffer[16], desc_p->intline_count);
	}

	desc_p->fifo_frame_size = buffer[18];

	fifostream_response:
	if (ret_val == DECODER_RSP_SUCCESS)
	{
		/* increase count at the end, if everything was successful */
		++stream_active_count;
	} else
	{
		//TODO: deallocate memory and do a graceful exit (maybe call a function to handle a complete channel delete, called also on stop streaming
	}

	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 7; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) ret_val; /* Success message*/
	xmit_buffer[4] = 0x47; /* Stream command ID */

	decoder_update_timestamp();

	uint8_t term_pos = xmit_buffer[1] - 2;
	xmit_buffer[term_pos] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[term_pos + 1] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return ret_val;
}
/*!
 *
 * @brief       : API to get and configure polling stream settings
 */
static decoder_rsp_t decoder_ext0x0f_pollstream_read(const uint8_t *buffer, decoder_dir_t dir)
{
	(void) dir;
	uint8_t stream_idx;
	uint16_t base_idx, raw_time;
	uint32_t total_chunk_sz = 0;
	stream_descriptor_t *desc_p = stream_descriptors;
	decoder_rsp_t ret_val = DECODER_RSP_SUCCESS;

	if (stream_active_count >= STREAM_MAX_COUNT)
	{
		ret_val = DECODER_RSP_NO_FREE_STREAM_CHANNEL;
		goto pollstream_response;
	}

	stream_idx = (uint8_t)stream_active_count;
	desc_p = &stream_descriptors[stream_idx];

	desc_p->channel_id = buffer[3] - 1;
	desc_p->mode = STREAM_MODE_POLLING;
	desc_p->param_interface = (stream_if_t) buffer[5];
	if (desc_p->param_interface == STREAM_IF_CS_SENSOR)
	{
		desc_p->param_interface = (stream_if_t) 9;
	}

	else if ((desc_p->param_interface >= STREAM_IF_MULTIIO_0) && (desc_p->param_interface <= STREAM_IF_MULTIIO_8))
	{
		/*lint -save -e656 arithmetic operation uses enum's*/
		desc_p->param_interface -= (stream_if_t) 2;
		/*lint -restore*/
	}
	desc_p->dev_address = (buffer[7] << 8) | buffer[8];

	/* read streaming_period and its timebase */
	raw_time = (buffer[9] << 8) | buffer[10];
	if (buffer[11] == (uint8_t) STREAM_TIMEUNIT_US)
		desc_p->sampling_period_us = raw_time;
	else if (buffer[11] == (uint8_t) STREAM_TIMEUNIT_MS)
		desc_p->sampling_period_us = raw_time * 1000;
	else
	{
		ret_val = DECODER_RSP_PARAM_OUT_OF_RANGE;
		goto pollstream_response;
	}

	desc_p->read_mode = (stream_read_t) buffer[12];

	/* Read chunk descriptors */
	//TODO: think about a common pool of chunk data structs instead of dynamic alloc
	desc_p->chunk_count = buffer[13];

	desc_p->chunks = (stream_chunkinfo_t*) stream_memory_alloc(desc_p->chunk_count * sizeof(stream_chunkinfo_t));

	for (uint8_t chunk_idx = 0; chunk_idx < desc_p->chunk_count; chunk_idx++)
	{
		/* calculate chunk data offset and get chunk pointer */
		base_idx = (uint16_t) (14 + chunk_idx * 3);
		stream_chunkinfo_t *chunk_p = &desc_p->chunks[chunk_idx];

		/* read address and size of chunk */
		chunk_p->startaddress = buffer[base_idx];
		chunk_p->num_bytes_to_read = (buffer[base_idx + 1] << 8) | buffer[base_idx + 2];

		/* accumulate chuck data size to a global size */
		total_chunk_sz += chunk_p->num_bytes_to_read;
	}
	desc_p->total_data_size = total_chunk_sz;

	base_idx = 14 + desc_p->chunk_count * 3;
	/* update spi type 8 or 16 bit */
    desc_p->spi_type = buffer[base_idx++];
    //If MSB bit set, clear on write is enabled
    desc_p->clear_on_write = (buffer[base_idx] & DECODER_CLEAR_ON_WRITE_MASK);
    //if 6th bit is set, interrupt pin is active high
    desc_p->hw_pin_state = (buffer[base_idx] & DECODER_HW_PIN_STATE_MASK);

    if (desc_p->clear_on_write)
    {
        //5..0 bits are used to inform dummy byte count
        desc_p->reg_info.dummy_byte = (buffer[base_idx++] & DECODER_DUMMY_BYTE_COUNT_MASK);
        desc_p->reg_info.startaddress = buffer[base_idx++];
        desc_p->reg_info.num_bytes_to_clear = buffer[base_idx++];
        desc_p->reg_info.data_buf = (uint8_t*)stream_memory_alloc((desc_p->reg_info.num_bytes_to_clear + desc_p->reg_info.dummy_byte));
    }
    else
    {
        base_idx = base_idx + 1;
    }
	desc_p->intline_count = buffer[base_idx++];
	if (desc_p->intline_count > 0)
	{
		/* read intline descriptors */
		desc_p->intline_info = (uint8_t*) stream_memory_alloc(desc_p->intline_count * sizeof(uint8_t));

		memcpy(desc_p->intline_info, &buffer[base_idx], desc_p->intline_count);
	}
	/* add the interrupt data to the total data size */
	desc_p->total_data_size += desc_p->intline_count;

	if (desc_p->total_data_size > STREAM_MAX_PACKET_DATA_SIZE)
	{
		// Maximum possible data size is 1024 bytes because of dynamic memory limits.
		ret_val = DECODER_RSP_PARAM_OUT_OF_RANGE;
	}

	pollstream_response:
	if (ret_val == DECODER_RSP_SUCCESS)
	{
		/* increase count at the end, if everything was successful */
		++stream_active_count;
	} else
	{
		//TODO: deallocate memory and do a graceful exit (maybe call a function to handle a complete channel delete, called also on stop streaming
	}

	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 8; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) ret_val; /* Success message*/
	xmit_buffer[4] = 0x4F; /* Stream command ID */
	xmit_buffer[5] = desc_p->channel_id + 1;

	decoder_update_timestamp();

	uint8_t term_pos = xmit_buffer[1] - 2;
	xmit_buffer[term_pos] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[term_pos + 1] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return ret_val;
}
/*!
 *
 * @brief       : API to get and configure interrupt stream settings for old sensors
 */
static decoder_rsp_t decoder_ext0x09_intstream_old_read(const uint8_t *buffer, decoder_dir_t dir)
{
	uint8_t stream_idx;
	uint16_t idx;
	stream_descriptor_t *desc_p;
	uint32_t total_chunk_sz = 0;
	uint16_t timeoutvalue;
	decoder_rsp_t ret_val = DECODER_RSP_SUCCESS;
	stream_chunkinfo_t *chunk_p;
	if (stream_active_count >= STREAM_MAX_COUNT)
	{
		ret_val = DECODER_RSP_NO_FREE_STREAM_CHANNEL;
		goto intstream_response;
	}

	stream_idx = stream_active_count;
	desc_p = &stream_descriptors[stream_idx];

	desc_p->channel_id = buffer[3];
	desc_p->mode = STREAM_MODE_INTERRUPT;
	desc_p->param_interface = (stream_if_t) buffer[5];
	if (desc_p->param_interface == STREAM_IF_CS_SENSOR)
	{
		desc_p->param_interface = (stream_if_t) 9;
	} else if ((desc_p->param_interface >= STREAM_IF_MULTIIO_0) && (desc_p->param_interface <= STREAM_IF_MULTIIO_8))
	{
		/*lint -save -e656 arithmetic operation uses enum's*/
		desc_p->param_interface -= (stream_if_t) 2;
		/*lint -restore*/
	}
	desc_p->data_ready_int = buffer[6];
	//int_pin_usage_native_emulated[desc_p->data_ready_int] = true; /* Workaround */
	//update_interrupt_pin_list(desc_p->data_ready_int);
	desc_p->dev_address = (buffer[7] << 8) | buffer[8];
	desc_p->read_mode = (stream_read_t) 0; //since for this read mode is not applicable, but we will align this old interrupt stream read
	// with new interrupt stream read

	stream_settings.stream_mode = STREAM_MODE_INTERRUPT;
	/* Read chunk descriptors */
	//TODO: think about a common pool of chunk data structs instead of dynamic alloc
	desc_p->chunk_count = 3;

	desc_p->chunks = (stream_chunkinfo_t*) stream_memory_alloc(desc_p->chunk_count * sizeof(stream_chunkinfo_t));

	/* Copy the first 2 chunks, as the third chunk is not available in contingous location*/
	for (uint8_t chunk_idx = 0; chunk_idx < 2; chunk_idx++)
	{
		/* calculate chunk data offset and get chunk pointer */
		uint16_t base_idx = (uint16_t) (9 + chunk_idx * 3);
		chunk_p = &desc_p->chunks[chunk_idx];

		/* read address and size of chunk */
		chunk_p->startaddress = buffer[base_idx];
		chunk_p->num_bytes_to_read = (buffer[base_idx + 1] << 8) | buffer[base_idx + 2];

		/* accumulate chuck data size to a global size */
		total_chunk_sz += chunk_p->num_bytes_to_read;
	}
	/* Read IntLines descriptors */
	desc_p->intline_count = buffer[15];
	if (desc_p->intline_count > 0)
	{
		/* read intline descriptors */
		desc_p->intline_info = (uint8_t*) stream_memory_alloc(desc_p->intline_count * sizeof(uint8_t));

		memcpy(desc_p->intline_info, &buffer[16], desc_p->intline_count);
	}

	idx = 16 + desc_p->intline_count;
	/* add the interrupt data to the total data size */
	desc_p->total_data_size = desc_p->intline_count;

	/* update the sensor time read address and size of chunk */
	chunk_p = &desc_p->chunks[2];
	chunk_p->startaddress = buffer[idx];
	chunk_p->num_bytes_to_read = buffer[idx + 1];
	/* update the total chunk size*/
	total_chunk_sz += chunk_p->num_bytes_to_read;

	desc_p->total_data_size += total_chunk_sz;
	idx += 2;
	timeoutvalue = buffer[idx] << 8 | buffer[idx + 1];

	if (timeoutvalue > timeout_required)
		timeout_required = timeoutvalue;

	if (desc_p->total_data_size > STREAM_MAX_PACKET_DATA_SIZE)
	{
		// Maximum possible data size is 1024 bytes because of dynamic memory limits.
		ret_val = DECODER_RSP_PARAM_OUT_OF_RANGE;
	}

	intstream_response:
	if (ret_val == DECODER_RSP_SUCCESS)
	{
		/* increase count at the end, if everything was successful */
		++stream_active_count;
	} else
	{
		//TODO: deallocate memory and do a graceful exit (maybe call a function to handle a complete channel delete, called also on stop streaming
	}

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 7; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) ret_val; /* Success message*/
	xmit_buffer[4] = 0x49; /* Stream command ID */

	decoder_update_timestamp();

	uint8_t term_pos = xmit_buffer[1] - 2;
	xmit_buffer[term_pos] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[term_pos + 1] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return ret_val;
}
/*!
 *
 * @brief       : API to get and configure interrupt stream settings
 */
static decoder_rsp_t decoder_ext0x0e_intstream_read(const uint8_t *buffer, decoder_dir_t dir)
{
	uint8_t stream_idx;
	uint16_t idx;
	stream_descriptor_t *desc_p;
	uint32_t total_chunk_sz = 0;
	uint16_t timeoutvalue;
	decoder_rsp_t ret_val = DECODER_RSP_SUCCESS;

	if (stream_active_count >= STREAM_MAX_COUNT)
	{
		ret_val = DECODER_RSP_NO_FREE_STREAM_CHANNEL;
		goto intstream_response;
	}

	stream_idx = stream_active_count;
	desc_p = &stream_descriptors[stream_idx];

	desc_p->channel_id = buffer[3];
	desc_p->mode = STREAM_MODE_INTERRUPT;
	desc_p->param_interface = (stream_if_t) buffer[5];

	if (desc_p->param_interface == STREAM_IF_CS_SENSOR)
	{
		desc_p->param_interface = (stream_if_t) 9;
	} 
	else if ((desc_p->param_interface >= STREAM_IF_MULTIIO_0) && (desc_p->param_interface <= STREAM_IF_MULTIIO_8))
	{
		/*lint -save -e656 arithmetic operation uses enum's*/
		desc_p->param_interface -= (stream_if_t) 2;
		/*lint -restore*/
	}

	desc_p->data_ready_int = buffer[6];
	//int_pin_usage_native_emulated[desc_p->data_ready_int] = true; /* Workaround */
	desc_p->dev_address = (buffer[7] << 8) | buffer[8];
	desc_p->read_mode = (stream_read_t) buffer[9];

	stream_settings.stream_mode = STREAM_MODE_INTERRUPT;
	/* Read chunk descriptors */
	//TODO: think about a common pool of chunk data structs instead of dynamic alloc
	desc_p->chunk_count = buffer[10];
	desc_p->chunks = (stream_chunkinfo_t*) stream_memory_alloc(desc_p->chunk_count * sizeof(stream_chunkinfo_t));

	for (uint8_t chunk_idx = 0; chunk_idx < desc_p->chunk_count; chunk_idx++)
	{
		/* calculate chunk data offset and get chunk pointer */
		uint16_t base_idx = (uint16_t) (11 + chunk_idx * 3);
		stream_chunkinfo_t *chunk_p = &desc_p->chunks[chunk_idx];

		/* read address and size of chunk */
		chunk_p->startaddress = buffer[base_idx];
		chunk_p->num_bytes_to_read = (buffer[base_idx + 1] << 8) | buffer[base_idx + 2];

		/* accumulate chuck data size to a global size */
		total_chunk_sz += chunk_p->num_bytes_to_read;
	}
	desc_p->total_data_size = total_chunk_sz;
	idx = 11 + desc_p->chunk_count * 3;

	/* Check if the data read is based on Interrupt status*/
	if (desc_p->read_mode == STREAM_READ_N_CHUNKS_INTERRUPT_STATUS_FRAMELEN)
	{
		/* Get the number of Interrupt mask in the command*/
		/* Interrupt status + Number of bytes to read*/
		/* Wakeup FIFO Size register1 + Number of bytes to read */
		/* Wakeup FIFO Data register 1+ Number of bytes to read from the previous read of size register1 */
		/* Non-Wakeup FIFO Size register2 + Number of bytes to read */
		/* Non-Wakeup FIFO Data register 2+ Number of bytes to read from the previous read of size register2 */
		/* In this interrupt mask is applicable only to the data register*/
		/* In our example number of chunks is 5, we implement this following formula to calcuate the number of interrupt mask is available */
		uint8_t num_interrupt_mask = (desc_p->chunk_count - 1) / 2;
		/* Allocate the memory for storing the chunk mask and read it */
		desc_p->chunk_mask = (uint8_t*) stream_memory_alloc(num_interrupt_mask * sizeof(uint8_t));

		memcpy(desc_p->chunk_mask, &buffer[idx], num_interrupt_mask);
		idx += num_interrupt_mask;
		desc_p->framelength = (buffer[idx] << 8) | buffer[idx + 1];
		/* Skip the framelength parameter*/
		idx += 2;

	} else if (desc_p->read_mode == STREAM_READ_2_CHUNKS_LITTLEENDIAN_FRAMELEN
			|| desc_p->read_mode == STREAM_READ_2_CHUNKS_BIGENDIAN_FRAMELEN)
	{
		/* Skip the framelength parameter*/
		//TODO: check if this is always the case (these 2 bytes are maybe always present?)
		desc_p->framelength = (buffer[17] << 8) | buffer[18];
		idx += 2;
	}

    /* update Timeout */
	timeoutvalue = buffer[idx] << 8 | buffer[idx + 1];
	idx += 2;
	if (timeoutvalue > timeout_required)
		timeout_required = timeoutvalue;

    /* update spi type 8 or 16 bit */
    desc_p->spi_type = buffer[idx++];
    //If MSB bit set, clear on write is enabled
    desc_p->clear_on_write = (buffer[idx] & DECODER_CLEAR_ON_WRITE_MASK);
    //if 6th bit is set, interrupt pin is active high
    desc_p->hw_pin_state = (buffer[idx] & DECODER_HW_PIN_STATE_MASK);

    if (desc_p->clear_on_write)
    {
        //5..0 bits are used to inform dummy byte count
        desc_p->reg_info.dummy_byte = (buffer[idx++] & DECODER_DUMMY_BYTE_COUNT_MASK);
        desc_p->reg_info.startaddress = buffer[idx++];
        desc_p->reg_info.num_bytes_to_clear = buffer[idx++];
        desc_p->reg_info.data_buf = (uint8_t*)stream_memory_alloc((desc_p->reg_info.num_bytes_to_clear + desc_p->reg_info.dummy_byte));
    }
    else
    {
        idx = idx + 1;
    }
    /* Read IntLines descriptors */
    desc_p->intline_count = buffer[idx++];
    if (desc_p->intline_count > 0)
    {
        /* read intline descriptors */
        desc_p->intline_info = (uint8_t*)stream_memory_alloc(desc_p->intline_count * sizeof(uint8_t));

        memcpy(desc_p->intline_info, &buffer[idx], desc_p->intline_count);
    }
    /* add the interrupt data to the total data size */
    desc_p->total_data_size += desc_p->intline_count;

    desc_p->read_period = buffer[idx] << 8 | buffer[idx + 1];
    //idx += 2; // Last value assigned to variable 'idx' not used so commenting it out.

    if (desc_p->total_data_size > STREAM_MAX_PACKET_DATA_SIZE)
    {
        // Maximum possible data size is 1024 bytes because of dynamic memory limits.
        ret_val = DECODER_RSP_PARAM_OUT_OF_RANGE;
    }

    intstream_response:
    if (ret_val == DECODER_RSP_SUCCESS)
    {
        /* increase count at the end, if everything was successful */
        ++stream_active_count;
    }
    else
    {
        //TODO: deallocate memory and do a graceful exit (maybe call a function to handle a complete channel delete, called also on stop streaming
    }

	/* Forming the Response */
	xmit_buffer[0] = DECODER_HEADER_VALUE; /* Start of Command */
	xmit_buffer[1] = 7; /* Number  of bytes in the packet */
	xmit_buffer[2] = 1; /* Packet Number */
	xmit_buffer[3] = (uint8_t) ret_val; /* Success message*/
	xmit_buffer[4] = 0x4e; /* Stream command ID */

	decoder_update_timestamp();

	uint8_t term_pos = xmit_buffer[1] - 2;
	xmit_buffer[term_pos] = DECODER_CR_MACRO; /* \r */
	xmit_buffer[term_pos + 1] = DECODER_LF_MACRO;/* \n */

	decoder_write_resp(xmit_buffer, xmit_buffer[1]);

	return ret_val;
}
/*!
 *
 * @brief       : API will decodes the command from host and call appropriate handlers to process
 */
void decoder_process_cmds(const uint8_t *cmd_host)
{
	uint8_t expected_cmd_size;
	decoder_dir_t cmd_dir;
	uint16_t cmd_feature;

	if (cmd_host == NULL)
		return;

	if (cmd_host[DECODER_BYTEPOS_HEADER] != DECODER_HEADER_VALUE)
		return;

	if (cmd_host[DECODER_BYTEPOS_FEATURE] == 0x22)
	{
		cmd_feature = 0x22;
	}

	if (cmd_host[DECODER_BYTEPOS_DIR] == (uint8_t) DECODER_DIR_SET)
	{
		cmd_feature = cmd_host[DECODER_BYTEPOS_FEATURE];
		if (cmd_feature >= (uint8_t) _DECODER_ID_MAX_)
			return;

		cmd_dir = DECODER_DIR_SET;
		expected_cmd_size = command_list[cmd_feature].setter_length;
	} else if (cmd_host[DECODER_BYTEPOS_DIR] == (uint8_t) DECODER_DIR_GET)
	{
		cmd_feature = cmd_host[DECODER_BYTEPOS_FEATURE];
		if (cmd_feature >= (uint8_t) _DECODER_ID_MAX_)
			return;

		cmd_dir = DECODER_DIR_GET;
		expected_cmd_size = command_list[cmd_feature].getter_length;
	} else
	{
		/* extended command set, invoke directly without size check or direction (use default SET) */
		cmd_dir = DECODER_DIR_SET;

		/* for extended commands, the ID comes in the slot of the direction byte */
		cmd_feature = (uint8_t) _DECODER_EXT_ID_BASE_ + cmd_host[DECODER_BYTEPOS_DIR];
		if (cmd_feature >= (uint8_t) _DECODER_ID_MAX_)
			return;

		if (command_list[cmd_feature].handler)
		{
			//TODO: use ret val and report error
			(void) command_list[cmd_feature].handler(cmd_host, cmd_dir);
		}
		return;
	}

	/* do the size check only if expected command size is known */
	if (expected_cmd_size != DECODER_UNKNOWN_SIZE)
	{
		/* check if expected command size matches the calculated command size */
		uint8_t cmd_size = decoder_calc_len(cmd_host);
		if (cmd_size != expected_cmd_size)
			return;
	}

	/* invoke the corresponding command handler if it's supported */
	if (cmd_feature < (uint8_t) _DECODER_ID_MAX_)
	{
		if (command_list[cmd_feature].handler)
		{
			//TODO: use ret val and report error
			(void) command_list[cmd_feature].handler(cmd_host, cmd_dir);
		}
	}
}

/**@} */
