/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    common.c
 * @brief   Common file for BMI090L examples
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

/*! BMI090L shuttle id */
#define BMI090L_SHUTTLE_ID  UINT16_C(0x86)

uint8_t acc_dev_add;
uint8_t gyro_dev_add;

/*!
 * I2C read function map to COINES platform
 */
BMI090L_INTF_RET_TYPE bmi090l_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;

    return coines_read_i2c(COINES_I2C_BUS_0, dev_addr, reg_addr, reg_data, (uint16_t)len);
}

/*!
 * I2C write function map to COINES platform
 */
BMI090L_INTF_RET_TYPE bmi090l_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;

    return coines_write_i2c(COINES_I2C_BUS_0, dev_addr, reg_addr, (uint8_t *)reg_data, (uint16_t)len);
}

/*!
 * SPI read function map to COINES platform
 */
BMI090L_INTF_RET_TYPE bmi090l_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;

    return coines_read_spi(COINES_SPI_BUS_0, dev_addr, reg_addr, reg_data, (uint16_t)len);
}

/*!
 * SPI write function map to COINES platform
 */
BMI090L_INTF_RET_TYPE bmi090l_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;

    return coines_write_spi(COINES_SPI_BUS_0, dev_addr, reg_addr, (uint8_t *)reg_data, (uint16_t)len);
}

/*!
 * Delay function map to COINES platform
 */
void bmi090l_delay_us(uint32_t period, void *intf_ptr)
{
    coines_delay_usec(period);
}

void bmi090l_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BMI090L_OK:

            /* Do nothing */
            break;
        case BMI090L_E_NULL_PTR:
            printf("API [%s] Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BMI090L_E_COM_FAIL:
            printf("API [%s] Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BMI090L_E_INVALID_CONFIG:
            printf("API [%s] Error [%d] : Invalid configuration\r\n", api_name, rslt);
            break;
        case BMI090L_E_DEV_NOT_FOUND:
            printf("API [%s] Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BMI090L_E_OUT_OF_RANGE:
            printf("API [%s] Error [%d] : Out of Range\r\n", api_name, rslt);
            break;
        case BMI090L_E_INVALID_INPUT:
            printf("API [%s] Error [%d] : Invalid Input\r\n", api_name, rslt);
            break;
        case BMI090L_E_CONFIG_STREAM_ERROR:
            printf("API [%s] Error [%d] : Config Stream error\r\n", api_name, rslt);
            break;
        case BMI090L_E_RD_WR_LENGTH_INVALID:
            printf("API [%s] Error [%d] : Invalid Read-write length\r\n", api_name, rslt);
            break;
        case BMI090L_E_FEATURE_NOT_SUPPORTED:
            printf("API [%s] Error [%d] : Feature not supported\r\n", api_name, rslt);
            break;
        case BMI090L_W_FIFO_EMPTY:
            printf("API [%s] Warning [%d] : FIFO empty\r\n", api_name, rslt);
            break;
        default:
            printf("API [%s] Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
    }
}

int8_t bmi090l_interface_init(struct bmi090l_dev *bmi090ldev, uint8_t intf)
{
    int8_t rslt = BMI090L_OK;
    struct coines_board_info board_info;

    if (bmi090ldev != NULL)
    {
        int16_t result = coines_open_comm_intf(COINES_COMM_INTF_USB, NULL);

        if (result < 0)
        {
            printf(
                "\n Unable to connect with Application Board ! \n" " 1. Check if the board is connected and powered on. \n" " 2. Check if Application Board USB driver is installed. \n"
                " 3. Check if board is in use by another application. (Insufficient permissions to access USB) \n");
            exit(result);
        }

        result = coines_get_board_info(&board_info);

        if (result != COINES_SUCCESS)
        {
            printf("\n Unable to retrieve board information ! \n");
            exit(COINES_E_FAILURE);
        }

        if (board_info.shuttle_id != BMI090L_SHUTTLE_ID)
        {
            printf("! Warning invalid sensor shuttle \n ," "This application will not support this sensor \n");
            exit(COINES_E_FAILURE);
        }

        /* Switch VDD for sensor off */
        coines_set_shuttleboard_vdd_vddio_config(0, 0);
        coines_delay_msec(200);

        /* Bus configuration : I2C */
        if (intf == BMI090L_I2C_INTF)
        {
            printf("I2C Interface \n");

            bmi090ldev->write = bmi090l_i2c_write;
            bmi090ldev->read = bmi090l_i2c_read;

            acc_dev_add = (unsigned char) BMI090L_ACCEL_I2C_ADDR_PRIMARY;
            gyro_dev_add = (unsigned char) BMI090L_GYRO_I2C_ADDR_PRIMARY;
            bmi090ldev->intf = BMI090L_I2C_INTF;
			/* SDO pin is made low */
			coines_set_pin_config(COINES_SHUTTLE_PIN_SDO, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_LOW);

            result = coines_config_i2c_bus(COINES_I2C_BUS_0, COINES_I2C_STANDARD_MODE);
            coines_delay_msec(100);

            /* PS pin is made high for selecting I2C protocol*/
            coines_set_pin_config(COINES_SHUTTLE_PIN_9, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
        }
        /* Bus configuration : SPI */
        else if (intf == BMI090L_SPI_INTF)
        {
            printf("SPI Interface \n");

            bmi090ldev->write = bmi090l_spi_write;
            bmi090ldev->read = bmi090l_spi_read;

            bmi090ldev->intf = BMI090L_SPI_INTF;
            acc_dev_add = COINES_SHUTTLE_PIN_8;
            gyro_dev_add = COINES_SHUTTLE_PIN_14;

            /* CS pin is made high for selecting SPI protocol*/
            coines_set_pin_config(COINES_SHUTTLE_PIN_8, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);

            /* CS pin is made high for selecting SPI protocol*/
            coines_set_pin_config(COINES_SHUTTLE_PIN_14, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);

            /* PS pin is made low for selecting SPI protocol*/
            coines_set_pin_config(COINES_SHUTTLE_PIN_9, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_LOW);

            coines_delay_msec(100);
            result = coines_config_spi_bus(COINES_SPI_BUS_0, COINES_SPI_SPEED_5_MHZ, COINES_SPI_MODE3);
        }

        if(COINES_SUCCESS != result)
        {
            rslt = COINES_E_COMM_INIT_FAILED;
        }

        bmi090ldev->intf_ptr_accel = &acc_dev_add;
        bmi090ldev->intf_ptr_gyro = &gyro_dev_add;
        bmi090ldev->delay_us = bmi090l_delay_us;
        bmi090ldev->read_write_len = 32;

        coines_delay_msec(200);

        /* Switch VDD for sensor on */
        coines_set_shuttleboard_vdd_vddio_config(3300, 3300);

        /* after sensor init introduce 200 msec sleep */
        coines_delay_msec(200);
    }
    else
    {
        rslt = BMI090L_E_NULL_PTR;
    }

    return rslt;
}

void bmi090l_coines_deinit(void)
{
    fflush(stdout);

    coines_set_shuttleboard_vdd_vddio_config(0, 0);
    coines_delay_msec(100);

    /* Coines interface reset */
    coines_soft_reset();
    coines_delay_msec(100);

    coines_close_comm_intf(COINES_COMM_INTF_USB, NULL);
}
