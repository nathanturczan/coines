/**
 * Copyright (C) 2022 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mcu_app30_support.c
 * @date    May 25, 2021
 * @brief   COINES support file for mcu_app30.c
 */

/**********************************************************************************/
/* system header includes */
/**********************************************************************************/
#include <stdint.h>
#include "coines.h"
#include "mcu_app30_support.h"

/**********************************************************************************/
/* own header files */
/**********************************************************************************/

/**********************************************************************************/
/* local macro definitions */
/**********************************************************************************/
#define RTC_PRESCALAR         0
#define RTC_COUNTER_BITS      24
#define RTC_TICKS_PER_SECOND  (32768 / (1 + RTC_PRESCALAR))
#define RTC_RESOLUTION_USEC   (1000000 / RTC_TICKS_PER_SECOND)
#define RTC_TICKS_TO_USEC(t)  (((uint64_t)t * UINT64_C(1000000)) / RTC_TICKS_PER_SECOND)

/**********************************************************************************/
/* constant definitions */
/**********************************************************************************/

/**********************************************************************************/
/* global variables */
/**********************************************************************************/

/**********************************************************************************/
/* static variables */
/**********************************************************************************/
/* Handle for RTC2 */
const nrfx_rtc_t rtc_handle = NRFX_RTC_INSTANCE(2);
volatile uint32_t rtc_count = 0;
static uint8_t volatile rtc_overflow = 0;

/* Timer instance loop-up table
NRFX_TIMER_INSTANCE(0) - used by softdevice
NRFX_TIMER_INSTANCE(1) - used by eeprom
*/
static const nrfx_timer_t timer_instance[COINES_TIMER_INSTANCE_MAX] = {
                                            #if NRFX_TIMER2_ENABLED
    NRFX_TIMER_INSTANCE(2),
                                            #endif/* NRFX_TIMER2_ENABLED */
                                            #if NRFX_TIMER3_ENABLED
    NRFX_TIMER_INSTANCE(3),
                                            #endif/* NRFX_TIMER3_ENABLED */
                                            #if NRFX_TIMER4_ENABLED
    NRFX_TIMER_INSTANCE(4),
                                            #endif/* NRFX_TIMER4_ENABLED */
};

/* Timer configuration */
static nrfx_timer_config_t timer_config = {
    .frequency = NRF_TIMER_FREQ_1MHz, .mode = NRF_TIMER_MODE_TIMER, .bit_width = NRF_TIMER_BIT_WIDTH_32,
    .interrupt_priority = 3, .p_context = NULL
};

/**********************************************************************************/
/* static function declaration */
/**********************************************************************************/

/*!
 *
 * @brief       : API to get shuttle ID
 *
 * @param[in]   : None
 * @return      : shuttle id
 */
static uint16_t get_shuttle_id()
{
    uint16_t shuttle_id = 0;

    NRFX_IRQ_DISABLE(USBD_IRQn);
    (void)app30_eeprom_read(0x01, (uint8_t *)&shuttle_id, 2);
    NRFX_IRQ_ENABLE(USBD_IRQn);

    return shuttle_id;
}

/*!
 * @brief This API is RTC interrupt handler which will be used to handle the interrupt events
 */
static void rtc_handler(nrfx_rtc_int_type_t int_type)
{

    switch (int_type)
    {
        case NRF_DRV_RTC_INT_COMPARE0:
        case NRF_DRV_RTC_INT_COMPARE1:
        case NRF_DRV_RTC_INT_COMPARE2:
        case NRF_DRV_RTC_INT_COMPARE3:

            break;
        case NRF_DRV_RTC_INT_OVERFLOW:
            /* RTC overflow event */
            rtc_overflow = (rtc_overflow != 255) ? (rtc_overflow + 1) : 0;
            break;

        case NRF_DRV_RTC_INT_TICK:
            /* RTC tick event */
            rtc_count = (rtc_overflow << RTC_COUNTER_BITS) | nrf_drv_rtc_counter_get(&rtc_handle);
            break;

        default:
            /* Unknown RTC event. Added for completeness */
            break;
    }
}

/**********************************************************************************/
/* functions */
/**********************************************************************************/

/*!
 * @brief This API is used to close the active communication(USB,COM or BLE).
 */
int16_t coines_close_comm_intf(enum coines_comm_intf intf_type, void *arg)
{
    (void)intf_type;
    (void)arg;

    return COINES_SUCCESS;
}

/*!
 *  @brief This API function is used to get the pin direction and pin state.
 */
int16_t coines_set_shuttleboard_vdd_vddio_config(uint16_t vdd_millivolt, uint16_t vddio_millivolt)
{
    if (vdd_millivolt == 0)
    {
        nrf_gpio_pin_write(VDD_PS_EN, 0);
        nrf_gpio_pin_write(VDD_SEL, 0);
    }
    else if ((vdd_millivolt > 0) && (vdd_millivolt <= 1800))
    {
        nrf_gpio_pin_write(VDD_PS_EN, 1);
        nrf_gpio_pin_write(VDD_SEL, 0);
    }
    else
    {
        nrf_gpio_pin_write(VDD_PS_EN, 1);
        nrf_gpio_pin_write(VDD_SEL, 1);
    }

    if (vddio_millivolt == 0)
    {
        nrf_gpio_pin_write(VDDIO_PS_EN, 0);
    }
    else
    {
        nrf_gpio_pin_write(VDDIO_PS_EN, 1);
    }

    return COINES_SUCCESS;
}

void coines_deconfig_i2s_bus()
{
    nrf_drv_i2s_stop();
    nrfx_i2s_uninit();
}

/*!
 *  @brief This API is used for introducing a delay in milliseconds
 */
void coines_delay_msec(uint32_t delay_ms)
{
    nrf_delay_ms(delay_ms);
}

/*!
 *  @brief This API is used for introducing a delay in microseconds
 */
void coines_delay_usec(uint32_t delay_us)
{
    nrf_delay_us(delay_us);
}

/*!
 * @brief This API is used to send the streaming settings to the board.
 */
int16_t coines_config_streaming(uint8_t channel_id,
                                struct coines_streaming_config *stream_config,
                                struct coines_streaming_blocks *data_blocks)
{
    (void)channel_id;
    (void)stream_config;
    (void)data_blocks;

    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief This API is used to send the streaming settings to the board.
 */
int16_t coines_start_stop_streaming(enum coines_streaming_mode stream_mode, uint8_t start_stop)
{
    (void)stream_mode;
    (void)start_stop;

    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief This API is used to read the streaming sensor data.
 */
int16_t coines_read_stream_sensor_data(uint8_t sensor_id,
                                       uint32_t number_of_samples,
                                       uint8_t *data,
                                       uint32_t *valid_samples_count)
{
    (void)sensor_id;
    (void)number_of_samples;
    (void)data;
    (void)valid_samples_count;

    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief This API is used to trigger the timer in firmware and enable or disable system time stamp
 */
int16_t coines_trigger_timer(enum coines_timer_config tmr_cfg, enum coines_time_stamp_config ts_cfg)
{
    (void)tmr_cfg;
    (void)ts_cfg;

    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief Get COINES library version
 *
 * @return pointer to version string
 */
char* coines_get_version()
{
    return COINES_VERSION;
}

/*!
 * @brief Resets the device
 *
 * @note  After reset device jumps to the address specified in makefile (APP_START_ADDRESS).
 *
 * @return void
 */
void coines_soft_reset(void)
{
    memcpy((uint32_t *)MAGIC_LOCATION, "COIN", 4); /* *MAGIC_LOCATION = 0x4E494F43; // 'N','O','I','C' */
    APP_START_ADDR = APP_START_ADDRESS; /* Application start address; */

    NVIC_SystemReset();
}

/*!
 *  @brief This API is used to get the board information.
 */
int16_t coines_get_board_info(struct coines_board_info *data)
{

    if (data != NULL)
    {
        data->board = 5;
        data->hardware_id = 0x11;
        data->shuttle_id = get_shuttle_id();
        data->software_id = 0x10;

        return COINES_SUCCESS;
    }
    else
    {
        return COINES_E_NULL_PTR;
    }
}

/**@brief Function for converting battery voltage to percentage.
 *
 * @details This is just an estimated percentage considering Maximum charging voltage as 4.2 and cut-off voltage as 3.0.
 *          It will vary between different batteries
 */
uint8_t battery_level_in_percentage(const uint16_t mvolts)
{
    float output_volt;
    uint8_t battery_level;

    const float battery_max = 4.200f; /*maximum voltage of battery */
    const float battery_min = 3.000f; /*minimum voltage of battery before shutdown */
    float input_volt = mvolts;

    output_volt = (((input_volt / 1000) - battery_min) / (battery_max - battery_min)) * 100;
    battery_level = (uint8_t)output_volt;

    return battery_level;
}

/*!
 * @brief  Callback to read battery voltage
 *
 * @return None
 */
void bat_status_read_callback(void)
{
    (void)nrfx_saadc_sample();
}

/*!
 *
 * @brief       : USB event callback handler
 *
 * @param[in]   : type of usb event
 *
 * @return      : None
 */
void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SUSPEND:
            break;
        case APP_USBD_EVT_DRV_RESUME:
            break;
        case APP_USBD_EVT_STARTED:
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }

            break;
        case APP_USBD_EVT_POWER_REMOVED:
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            app_usbd_start();
            break;
        default:
            break;
    }
}

/*!
 * @brief This API is used to get the current counter(RTC) reference time in usec
 *
 * @param[in]   : None
 * @return      : counter(RTC) reference time in usec
 * */
uint32_t coines_get_realtime_usec(void)
{
    return RTC_TICKS_TO_USEC(rtc_count);
}

/*!
 * @brief This API is used to introduce delay based on high precision RTC(LFCLK crystal)
 * with the resolution of of 30.517 usec
 *
 * @param[in]   : required delay in microseconds
 * @return      : None
 */
void coines_delay_realtime_usec(uint32_t period)
{
    uint32_t tick_count;
    uint32_t num_ticks;

    /*lint -e653 -e524 */
    num_ticks = (period < RTC_RESOLUTION_USEC) ? 1 : (uint32_t)roundf(((float)period / RTC_RESOLUTION_USEC));

    tick_count = ((rtc_overflow << RTC_COUNTER_BITS) | nrf_drv_rtc_counter_get(&rtc_handle));

    while ((((rtc_overflow << RTC_COUNTER_BITS) | nrf_drv_rtc_counter_get(&rtc_handle)) - tick_count) < num_ticks)
        ;
}

/*!
 * @brief This API is used to config RTC
 */
uint32_t rtc_config(void)
{
    uint32_t err_code;

    nrfx_rtc_config_t rtc_config_struct = NRFX_RTC_DEFAULT_CONFIG;

    /* Configure the prescaler to generate ticks for a specific time unit */
    /* if prescalar = 1, tick resolution =  32768 / (1 + 1) = 16384Hz = 61.035us */
    rtc_config_struct.prescaler = RTC_PRESCALAR;

    /* Initialize the RTC and pass the configurations along with the interrupt handler */
    err_code = nrfx_rtc_init(&rtc_handle, &rtc_config_struct, rtc_handler);
    if (err_code == NRFX_SUCCESS)
    {
        /* Generate a tick event on each tick */
        nrfx_rtc_tick_enable(&rtc_handle, true);

        nrfx_rtc_overflow_enable(&rtc_handle, true);

        /* start the RTC */
        nrfx_rtc_enable(&rtc_handle);
    }

    return err_code;
}

/*!
 * @brief This API is used to config the hardware timer in firmware
 */
int16_t coines_timer_config(enum coines_timer_instance instance, void* handler)
{
    if (instance < COINES_TIMER_INSTANCE_MAX)
    {
        if (nrfx_timer_init(&timer_instance[instance], &timer_config, (nrfx_timer_event_handler_t)handler) != NRFX_SUCCESS)//lint !e611
        {
            return COINES_E_TIMER_INIT_FAILED;
        }
    }
    else
    {
        return COINES_E_TIMER_INVALID_INSTANCE;
    }

    return COINES_SUCCESS;
}

/*!
 * @brief This API is used to start the hardware timer in firmware
 */
int16_t coines_timer_start(enum coines_timer_instance instance, uint32_t timeout)
{

    if (instance < COINES_TIMER_INSTANCE_MAX)
    {
        nrfx_timer_extended_compare(&timer_instance[instance],
                                    NRF_TIMER_CC_CHANNEL0,
                                    nrfx_timer_us_to_ticks(&timer_instance[instance], timeout),
                                    NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                    (bool)true);
        nrfx_timer_enable(&timer_instance[instance]);
    }
    else
    {
        return COINES_E_TIMER_INVALID_INSTANCE;
    }

    return COINES_SUCCESS;
}

/*!
 * @brief This API is used to start the hardware timer in firmware
 */
int16_t coines_timer_stop(enum coines_timer_instance instance)
{
    if (instance < COINES_TIMER_INSTANCE_MAX)
    {
        if (nrfx_timer_is_enabled(&timer_instance[instance]))
        {
            nrfx_timer_disable(&timer_instance[instance]);
        }
    }
    else
    {
        return COINES_E_TIMER_INVALID_INSTANCE;
    }

    return COINES_SUCCESS;
}

/** @}*/
