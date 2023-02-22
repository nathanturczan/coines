/**
 * Copyright (C) 2020 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file        stream.h
 *
 * @brief		This file defines the data structures and macros for sensor data streaming
 *
 */

#ifndef APPLICATION_STREAM_H_
#define APPLICATION_STREAM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************************************************************/
/* header includes */
/**********************************************************************************/

/**********************************************************************************/
/* macro definitions */
/**********************************************************************************/
/*! Maximum allowed sensors to stream */
#define STREAM_MAX_COUNT                    UINT8_C(32)

/*! Maximum IO numbers */
#define STREAM_MAX_IO_COUNT                 UINT8_C(32)

/*! Maximum data size to read from sensors */
#define STREAM_MAX_PACKET_DATA_SIZE         UINT16_C(2048)

/**********************************************************************************/
/* type definitions */
/**********************************************************************************/
/*!
 *
 * @brief : Enum for time stamp updation in response packets
 *
 */
typedef enum
{
    STREAM_NO_TIMESTAMP = 0, /*< no timestamp */
    STREAM_USE_TIMESTAMP = 1 /*< Timestamp is present */
} stream_timestamp_t;

/*!
 *
 * @brief : Enum for streaming mode
 *
 */
typedef enum
{
    STREAM_MODE_INTERRUPT, /*< interrupt*/
    STREAM_MODE_POLLING, /*< polling*/
    STREAM_MODE_FIFO_POLLING /*<fifo polling*/
} stream_mode_t;

/*!
 *
 * @brief : Enum for read mode in case of fifo
 *
 */
typedef enum
{
    STREAM_FIFO_BURST_READ, /*< burst read mode*/
    STREAM_FIFO_FRAME_READ /*< frame read mode */
} stream_fifo_read_t;

/*!
 *
 * @brief : Enum for time unit
 *
 */
typedef enum
{
    STREAM_TIMEUNIT_US = 0x01, /*< time unit as US */
    STREAM_TIMEUNIT_MS = 0x02 /*< time unit as MS*/
} stream_timeunit_t;

/*!
 *
 * @brief : Enum for interfaces
 *
 */
typedef enum
{
    STREAM_IF_I2C = 0, /*< I2C */
    STREAM_IF_CS_SENSOR, /*< CS sensor */
    STREAM_IF_MULTIIO_0, /*< mutiplt IO 0 */
    STREAM_IF_MULTIIO_1, /*< mutiplt IO 1 */
    STREAM_IF_MULTIIO_2, /*< mutiplt IO 2 */
    STREAM_IF_MULTIIO_3, /*< mutiplt IO 3 */
    STREAM_IF_MULTIIO_4, /*< mutiplt IO 4 */
    STREAM_IF_MULTIIO_5, /*< mutiplt IO 5 */
    STREAM_IF_MULTIIO_6, /*< mutiplt IO 6 */
    STREAM_IF_MULTIIO_7, /*< mutiplt IO 7 */
    STREAM_IF_MULTIIO_8, /*< mutiplt IO 8 */
    STREAM_IF_PRESS_SENS, /*< pressure sensor */
    STREAM_IF_PLUG_ALGO, /*< algorithm*/
    STREAM_IF_THRIDPARTY_SENSOR, /*< thirdparty sensor */
    STREAM_IF_ALGODATA_IN_SENSOR,/*< algodata in sensor */
    STREAM_IF_READDATA_CHUNKS, /*< read data chunks */
    STREAM_IF_CURRENT_SENSOR, /*< current sensor */
    STREAM_IF_GPIO_0,
    STREAM_IF_GPIO_1,
    STREAM_IF_GPIO_2,
    STREAM_IF_GPIO_3,
    STREAM_IF_GPIO_4,
    STREAM_IF_GPIO_5,
} stream_if_t;

/*!
 *
 * @brief : Enum for read mode type in streaming
 *
 */
typedef enum
{
    STREAM_READ_N_CHUNKS = 1, /*< read chunks */
    STREAM_READ_2_CHUNKS_LITTLEENDIAN, /*<little endian */
    STREAM_READ_2_CHUNKS_BIGENDIAN, /*< bid endian */
    STREAM_READ_2_CHUNKS_LITTLEENDIAN_FRAMELEN, /*< little endian framelen */
    STREAM_READ_2_CHUNKS_BIGENDIAN_FRAMELEN, /*< big endian framelen */
    STREAM_READ_N_CHUNKS_INTERRUPT_STATUS_FRAMELEN,/*< interrupt status framelen */
    STREAM_READ_2_CHUNKS_POLLING /*< chunk polling */
} stream_read_t;

/*!
 *
 * @brief : Structure which holds the sensor specific information for reading
 *
 */
typedef struct
{
    uint8_t startaddress; /*< starting address */
    uint16_t num_bytes_to_read; /* < No. of bytes to read */
    uint8_t* DATA_chunk; /*< data chunks */
} stream_chunkinfo_t;

/*!
 *
 * @brief : Structure which holds the sensor specific information for clearing on write
 *
 */
typedef struct
{
    uint8_t dummy_byte; /*< dummy byte count */
    uint8_t startaddress; /*< starting address */
    uint16_t num_bytes_to_clear; /* < No. of bytes to clear */
    uint8_t* data_buf; /*< data chunks */
} int_status_reginfo_t;

/*!
 *
 * @brief : Structure which hold the settings for poll/interrupt streaming
 *
 */
typedef struct
{
    uint8_t channel_id; /*< channel id */
    stream_mode_t  mode; /*<stream mode */

    //stream_param_timestamp_t param_ts;
    //TODO: improve naming
    stream_if_t param_interface; /*< parameter interface */
    uint32_t sampling_period_us; /*< sampling periond in micro seconds */
    uint32_t GST_multiplier; /*< GST multiplier */
    uint32_t GST_ticks_counter; /*< GST ticks counter */
    uint8_t data_ready_int; /*< data ready initialization */
    uint16_t dev_address; /*< device address */
    stream_read_t read_mode; /*< read mode */
    uint16_t read_period; /*< read period */

    //Payload info and tracking
    uint8_t chunk_count; /*< chunk count */
    uint32_t total_data_size; /*< total data size */
    stream_chunkinfo_t *chunks; /*< chunk information */
    uint8_t *chunk_mask; /*< chunk mask */
    uint8_t spi_type; /*< spi type */
    uint8_t clear_on_write; /*< clear on write */
    uint8_t hw_pin_state; /*< hardware pin state active low/high */
    int_status_reginfo_t reg_info;/*< register info for clearing on write */

    uint8_t intline_count; /*< inline count */
    uint8_t* intline_info; /*< inline information*/
    uint8_t* DATA_intline; /*< data inline */

    uint32_t data_packet_counter; /*< data packet counter */
    uint64_t packet_timestamp_us; /*< packet timestamp */
    uint16_t framelength; /*< frame lemgth */
} stream_descriptor_t;

/*!
 *
 * @brief : Structure which holds setting for fifo streaming
 *
 */
typedef struct
{
    uint8_t channel_id; /*< channel id */
    stream_fifo_read_t feature; /*< feature */
    stream_mode_t mode; /*<stream mode */
    stream_if_t param_interface; /*< parameter interface */
    uint32_t sampling_period_us; /*< sampling periond in micro seconds */
    uint32_t GST_multiplier; /*< GST multiplier */
    uint32_t GST_ticks_counter; /*< GST ticks counter */
    uint16_t dev_address; /*< device address */
    uint16_t read_period; /*< read period */
    uint8_t fifo_reg_address; /*< fifo register address */
    uint16_t number_bytes; /*< number of butes */
    uint8_t spi_type; /*< type of spi(8bit or 16bit) */
    uint8_t intline_count; /*< inline count */
    uint8_t *fifo_data; /*< fifo data */
    uint8_t* intline_info; /*< inline information */
    uint8_t* DATA_intline; /*< data inline */
    uint8_t fifo_frame_size; /*<fifo frame size */
} stream_fifo_descriptor_t;

/*!
 *
 * @brief : Structure which holds GCD and stream mode type
 *
 */
typedef struct
{
    uint32_t GST_period_us; /*< Global Sampling Timer period (in polling mode, all sensor sampling periods are multiple of this period)*/
    stream_timestamp_t ts_mode; /*< ts mode */
    stream_mode_t stream_mode; /*< stream mode */
} stream_settings_t;

/**********************************************************************************/
/* (extern) variable declarations */
/**********************************************************************************/
extern stream_settings_t stream_settings;
extern uint32_t stream_active_count;
extern stream_descriptor_t stream_descriptors[STREAM_MAX_COUNT];
extern stream_fifo_descriptor_t stream_fifo_descriptors;

/**********************************************************************************/
/* function prototype declarations */
/**********************************************************************************/
/*!
 *
 * @brief       : Function will start streaming
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void stream_start(void);
/*!
 *
 * @brief       : Function will stop streaming
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void stream_stop(void);
/*!
 *
 * @brief       : Function will acquire the sensor data in polling
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void stream_polling_data_acq(void);
/*!
 *
 * @brief       : Function will acquire the sensor data in interrupt
 *
 * @param[in]   : pin - Gives on which pin interrupt is raised
 *
 * @return      : None
 */
void stream_interrupt_data_acq(uint32_t pin);
/*!
 *
 * @brief       : API will allocate the buffer
 *
 * @param[in]   : size - Gives the amount buffer to allocate
 *
 * @return      : None
 */
void* stream_memory_alloc(uint32_t size);

/*!
 *
 * @brief       : API will check whether polling/interrupt data is available and send the acquired data
 
 * @return      : None
 */
void send_old_protocol_streaming_response(void);

/**********************************************************************************/
/* inline function definitions */
/**********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_STREAM_H_ */
