/**
 * Copyright (C) 2022 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mcu_app30.h
 * @date    Mar 1, 2021
 * @brief   This file contains COINES layer function prototypes, variable declarations and Macro definitions
 *
 */
#ifndef MCU_APP30_H_
#define MCU_APP30_H_

#include "mcu_app30_support.h"
#include "coines.h"

#include <sys/stat.h>

#define VBAT_MON_EN                   NRF_GPIO_PIN_MAP(0, 02)

#define  GPIO_0                       NRF_GPIO_PIN_MAP(0, 14) /* SB1_4 - P0.14 (I2C1_SCL) */
#define  GPIO_1                       NRF_GPIO_PIN_MAP(0, 13) /* SB1_5 - P0.13 (I2C1_SDA) */
#define  GPIO_2                       NRF_GPIO_PIN_MAP(1, 1) /* INT1 - SB1_6 - P1.01 */
#define  GPIO_3                       NRF_GPIO_PIN_MAP(1, 8) /* INT2 - SB1_7 - P1.08 */
#define  GPIO_CS                      NRF_GPIO_PIN_MAP(0, 24) /* SB2_1 - P0.24 */
#define  GPIO_SDO                     NRF_GPIO_PIN_MAP(0, 15) /* SB2_3 - P0.15*/
#define  GPIO_4                       NRF_GPIO_PIN_MAP(1, 3) /* SB2_5 - P1.03 */
#define  GPIO_5                       NRF_GPIO_PIN_MAP(1, 2) /* SB2_6 - P1.02 */
#define  GPIO_6                       NRF_GPIO_PIN_MAP(1, 11) /* SB2_7 - P1.11 */
#define  GPIO_7                       NRF_GPIO_PIN_MAP(1, 10) /* SB2_8 - P1.10 */
#define  GPIO_SDI                     NRF_GPIO_PIN_MAP(0, 6) /* SB2_4 - P0.6*/
#define  GPIO_SCK                     NRF_GPIO_PIN_MAP(0, 16) /* SB2_2 - P0.16*/

#define MAX_FILE_DESCRIPTORS          5

#define CDC_ACM_COMM_INTERFACE        0
#define CDC_ACM_COMM_EPIN             NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE        1
#define CDC_ACM_DATA_EPIN             NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT            NRF_DRV_USBD_EPOUT1

#define MCU_LED_R                     NRF_GPIO_PIN_MAP(0, 7)
#define MCU_LED_G                     NRF_GPIO_PIN_MAP(0, 11)
#define MCU_LED_B                     NRF_GPIO_PIN_MAP(0, 12)

#define SWITCH1                       NRF_GPIO_PIN_MAP(1, 9)
#define SWITCH2                       NRF_GPIO_PIN_MAP(0, 25)

#define LED_BLINK_MAX_DELAY           (64)

#define NRF_CLOCK_INIT_FAILED         1
#define NRF_CLOCK_INIT_FAILED_MASK    (1 << NRF_CLOCK_INIT_FAILED)
#define NRF_POWER_INIT_FAILED         2
#define NRF_POWER_INIT_FAILED_MASK    (1 << NRF_POWER_INIT_FAILED)
#define NRF_FLASH_INIT_FAILED         3
#define NRF_FLASH_INIT_FAILED_MASK    (1 << NRF_FLASH_INIT_FAILED)
#define NRF_GPIO_INIT_FAILED          4
#define NRF_GPIO_INIT_FAILED_MASK     (1 << NRF_GPIO_INIT_FAILED)
#define NRF_RTC_INIT_FAILED           5
#define NRF_RTC_INIT_FAILED_MASK      (1 << NRF_RTC_INIT_FAILED)
#define NRF_SYSTICK_INIT_FAILED       6
#define NRF_SYSTICK_INIT_FAILED_MASK  (1 << NRF_SYSTICK_INIT_FAILED)
#define NRF_ADC_INIT_FAILED           7
#define NRF_ADC_INIT_FAILED_MASK      (1 << NRF_ADC_INIT_FAILED)
#define NRF_USB_INIT_FAILED           8
#define NRF_USB_INIT_FAILED_MASK      (1 << NRF_USB_INIT_FAILED)

#define EEPROM_READ_LEN               (10)
#define EEPROM_CS_BYTE_INDEX          (9)

#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE                2060
#endif

/**********************************************************************************/
/* functions */
/**********************************************************************************/
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst, app_usbd_cdc_acm_user_event_t event);

/*lint -e778 -e845 -e746 -e785 -e446 -e734 */
APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            cdc_acm_user_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_NONE);

typedef void (*ISR_CB)(uint32_t pin, uint32_t polarity);
static ISR_CB isr_cb[COINES_SHUTTLE_PIN_MAX];
static void gpiohandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif /* MCU_APP30_H_ */
