/**
 * Copyright (C) 2022 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mcu_app30_support.h
 * @date    Mar 1, 2021
 * @brief   This file contains COINES layer function prototypes, variable declarations and Macro definitions
 *
 */
#ifndef MCU_APP30_SUPPORT_H_
#define MCU_APP30_SUPPORT_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>

#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_saadc.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrf_delay.h"
#include "nrf_drv_power.h"
#include "nrfx_spim.h"
#include "nrfx_twim.h"
#include "nrf_drv_i2s.h"
#include "nrfx_timer.h"
#include "nrf_timer.h"



#include "app_error.h"
#include "app_util.h"
#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"

#include "app30_eeprom.h"
#include "flogfs.h"
#include "w25_common.h"
#include "w25m02gw.h"
#include "w25n02jw.h"
#include "ble_service.h"

#define VDD_SEL                 NRF_GPIO_PIN_MAP(0, 27)
#define VDD_PS_EN               NRF_GPIO_PIN_MAP(0, 3)
#define VDDIO_PS_EN             NRF_GPIO_PIN_MAP(0, 28)

/**********************************************************************************/
/* functions */
/**********************************************************************************/
/****** Reserved Memory Area for performing application switch - 16 bytes******/
#define  MAGIC_LOCATION          (0x2003FFF4)
#define  MAGIC_INFO_ADDR         ((int8_t *)(MAGIC_LOCATION))
#define  APP_START_ADDR          (*(uint32_t *)(MAGIC_LOCATION + 4))
#define  APP_SP_VALUE            (*(uint32_t *)APP_START_ADDR)
#define  APP_RESET_HANDLER_ADDR  (*(uint32_t *)(APP_START_ADDR + 4))

/**@brief Function for converting battery voltage to percentage.
 *
 * @details This is just an estimated percentage considering Maximum charging voltage as 4.2 and cut-off voltage as 3.0.
 *          It will vary between different batteries
 *
 * @param[in] mvolts voltage(in milli volts) to be converted into percentage.
 *
 * @retval battery level in percentage.
 */
uint8_t battery_level_in_percentage(const uint16_t mvolts);


/*!
 * @brief  Callback to read battery voltage
 *
 * @return None
 */
void bat_status_read_callback(void);

/*!
 *
 * @brief       : USB event callback handler
 *
 * @param[in]   : type of usb event
 *
 * @return      : None
 */
void usbd_user_ev_handler(app_usbd_event_type_t event);

/*!
 * @brief This API is used to config RTC
 */
uint32_t rtc_config(void);

#endif /* MCU_APP30_SUPPORT_H_ */

