/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    data_sync.c
 * @brief   Test code to read synchronized sensor from BMI090L
 *
 */

/*********************************************************************/
/*                     System header files                           */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*********************************************************************/
/*                       Own header files                            */
/*********************************************************************/
#include "bmi090l.h"
#include "common.h"

/*********************************************************************/
/*                         Global variables                          */
/*********************************************************************/
unsigned char data_sync_int = false;
unsigned int count = 0;

/*********************************************************************/
/*                       Function Declarations                       */
/*********************************************************************/

/*!
 * @brief    This internal API is used to initialize the bmi090l sensor
 */
static void init_bmi090l(struct bmi090l_dev *bmi090ldev);

/*!
 * @brief    bmi090l data sync. interrupt callback
 */
void bmi090l_data_sync_int(uint32_t param1, uint32_t param2);

/*********************************************************************/
/*                          Functions                                */
/*********************************************************************/

/*!
 *  @brief This internal API is used to initializes the bmi090l sensor
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static void init_bmi090l(struct bmi090l_dev *bmi090ldev)
{
    int8_t rslt = BMI090L_OK;
    struct bmi090l_data_sync_cfg sync_cfg = { };

    /* Initialize bmi090l sensors (accel & gyro)*/
    if (bmi090la_init(bmi090ldev) == BMI090L_OK && bmi090lg_init(bmi090ldev) == BMI090L_OK)
    {
        printf("BMI090L initialization success!\n");
        printf("Accel chip ID - 0x%x\n", bmi090ldev->accel_chip_id);
        printf("Gyro chip ID - 0x%x\n", bmi090ldev->gyro_chip_id);

        /* Reset the accelerometer */
        rslt = bmi090la_soft_reset(bmi090ldev);
    }
    else
    {
        printf("BMI090L initialization failure!\n");
        exit(COINES_E_FAILURE);
    }

    /*! Max read/write length (maximum supported length is 32).
     * To be set by the user */
    bmi090ldev->read_write_len = 32;

    /* Set accel power mode */
    bmi090ldev->accel_cfg.power = BMI090L_ACCEL_PM_ACTIVE;
    rslt = bmi090la_set_power_mode(bmi090ldev);

    if (rslt == BMI090L_OK)
    {
        bmi090ldev->gyro_cfg.power = BMI090L_GYRO_PM_NORMAL;
        bmi090lg_set_power_mode(bmi090ldev);
    }

    if ((bmi090ldev->accel_cfg.power != BMI090L_ACCEL_PM_ACTIVE) ||
        (bmi090ldev->gyro_cfg.power != BMI090L_GYRO_PM_NORMAL))
    {
        printf("Accel/gyro sensor in suspend mode\nUse in active/normal mode !!");
        exit(EXIT_FAILURE);
    }

    printf("Uploading BMI090L data synchronization feature config !\n");

    /*API uploads the bmi090l config file onto the device*/
    if (rslt == BMI090L_OK)
    {
        rslt = bmi090la_apply_config_file(bmi090ldev);

        /* Wait for 150ms to enable the data synchronization --delay taken care inside the function */
        if (rslt == BMI090L_OK)
        {
            bmi090ldev->accel_cfg.range = BMI090L_ACCEL_RANGE_24G;

            /* Assign gyro range setting*/
            bmi090ldev->gyro_cfg.range = BMI090L_GYRO_RANGE_2000_DPS;

            /*! Mode (0 = off, 1 = 400Hz, 2 = 1kHz, 3 = 2kHz) */
            sync_cfg.mode = BMI090L_ACCEL_DATA_SYNC_MODE_400HZ;
            rslt = bmi090la_configure_data_synchronization(sync_cfg, bmi090ldev);
        }
    }

    if (rslt == BMI090L_OK)
    {
        printf("BMI090L data synchronization feature configured !\n");
    }
    else
    {
        printf("BMI090L data synchronization feature configuration failure!\n");
        exit(COINES_E_FAILURE);
    }
}

/*!
 *  @brief This internal API is used to enable data synchronization interrupt.
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static void enable_bmi090l_data_synchronization_interrupt(struct bmi090l_dev *bmi090ldev)
{
    int8_t rslt = BMI090L_OK;
    struct bmi090l_int_cfg int_config = { };

    /* Set accel interrupt pin configuration */
    /* Configure host data ready interrupt */
    #if defined(MCU_APP20)
    int_config.accel_int_config_1.int_channel = BMI090L_INT_CHANNEL_1;
    #elif defined(MCU_APP30)
    int_config.accel_int_config_1.int_channel = BMI090L_INT_CHANNEL_2;
    #endif
    int_config.accel_int_config_1.int_type = BMI090L_ACCEL_SYNC_INPUT;
    int_config.accel_int_config_1.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
    int_config.accel_int_config_1.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    int_config.accel_int_config_1.int_pin_cfg.enable_int_pin = BMI090L_ENABLE;

    /* Configure Accel syncronization input interrupt pin */
    #if defined(MCU_APP20)
    int_config.accel_int_config_2.int_channel = BMI090L_INT_CHANNEL_2;
    #elif defined(MCU_APP30)
    int_config.accel_int_config_2.int_channel = BMI090L_INT_CHANNEL_1;
    #endif
    int_config.accel_int_config_2.int_type = BMI090L_ACCEL_SYNC_DATA_RDY_INT;
    int_config.accel_int_config_2.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
    int_config.accel_int_config_2.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    int_config.accel_int_config_2.int_pin_cfg.enable_int_pin = BMI090L_ENABLE;

    /* Set gyro interrupt pin configuration */
    #if defined(MCU_APP20)
    int_config.gyro_int_config_1.int_channel = BMI090L_INT_CHANNEL_3;
    #elif defined(MCU_APP30)
    int_config.gyro_int_config_1.int_channel = BMI090L_INT_CHANNEL_4;
    #endif
    int_config.gyro_int_config_1.int_type = BMI090L_GYRO_INT_DATA_RDY;
    int_config.gyro_int_config_1.int_pin_cfg.enable_int_pin = BMI090L_ENABLE;
    int_config.gyro_int_config_1.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    int_config.gyro_int_config_1.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;

    #if defined(MCU_APP20)
    int_config.gyro_int_config_2.int_channel = BMI090L_INT_CHANNEL_4;
    #elif defined(MCU_APP30)
    int_config.gyro_int_config_2.int_channel = BMI090L_INT_CHANNEL_3;
    #endif
    int_config.gyro_int_config_2.int_type = BMI090L_GYRO_INT_DATA_RDY;
    int_config.gyro_int_config_2.int_pin_cfg.enable_int_pin = BMI090L_DISABLE;
    int_config.gyro_int_config_2.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    int_config.gyro_int_config_2.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;

    rslt = bmi090la_set_data_sync_int_config(&int_config, bmi090ldev);

    if (rslt != BMI090L_OK)
    {
        printf("BMI090L data synchronization enable interrupt configuration failure!\n");
        exit(COINES_E_FAILURE);
    }
}

/*!
 *  @brief This internal API is used to disable data synchronization interrupt.
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static void disable_bmi090l_data_synchronization_interrupt(struct bmi090l_dev *bmi090ldev)
{
    int8_t rslt;
    struct bmi090l_int_cfg int_config = { };
    struct bmi090l_data_sync_cfg sync_cfg = { };

    sync_cfg.mode = BMI090L_ACCEL_DATA_SYNC_MODE_OFF; /*turn off the sync feature*/

    rslt = bmi090la_configure_data_synchronization(sync_cfg, bmi090ldev);

    /* Wait for 150ms to enable the data synchronization --delay taken care inside the function */
    /* configure synchronization interrupt pins */
    if (rslt == BMI090L_OK)
    {
        /* Set accel interrupt pin configuration */
        /* Configure host data ready interrupt */
    #if defined(MCU_APP20)
        int_config.accel_int_config_1.int_channel = BMI090L_INT_CHANNEL_1;
    #elif defined(MCU_APP30)
        int_config.accel_int_config_1.int_channel = BMI090L_INT_CHANNEL_2;
    #endif
        int_config.accel_int_config_1.int_type = BMI090L_ACCEL_SYNC_INPUT;
        int_config.accel_int_config_1.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
        int_config.accel_int_config_1.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
        int_config.accel_int_config_1.int_pin_cfg.enable_int_pin = BMI090L_DISABLE;

        /* Configure Accel synchronization input interrupt pin */
    #if defined(MCU_APP20)
        int_config.accel_int_config_2.int_channel = BMI090L_INT_CHANNEL_2;
    #elif defined(MCU_APP30)
        int_config.accel_int_config_2.int_channel = BMI090L_INT_CHANNEL_1;
    #endif
        int_config.accel_int_config_2.int_type = BMI090L_ACCEL_SYNC_DATA_RDY_INT;
        int_config.accel_int_config_2.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
        int_config.accel_int_config_2.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
        int_config.accel_int_config_2.int_pin_cfg.enable_int_pin = BMI090L_DISABLE;

        /* Set gyro interrupt pin configuration*/
    #if defined(MCU_APP20)
        int_config.gyro_int_config_1.int_channel = BMI090L_INT_CHANNEL_3;
    #elif defined(MCU_APP30)
        int_config.gyro_int_config_1.int_channel = BMI090L_INT_CHANNEL_4;
    #endif
        int_config.gyro_int_config_1.int_type = BMI090L_GYRO_INT_DATA_RDY;
        int_config.gyro_int_config_1.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
        int_config.gyro_int_config_1.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
        int_config.gyro_int_config_1.int_pin_cfg.enable_int_pin = BMI090L_DISABLE;

    #if defined(MCU_APP20)
        int_config.gyro_int_config_2.int_channel = BMI090L_INT_CHANNEL_4;
    #elif defined(MCU_APP30)
        int_config.gyro_int_config_2.int_channel = BMI090L_INT_CHANNEL_3;
    #endif
        int_config.gyro_int_config_2.int_type = BMI090L_GYRO_INT_DATA_RDY;
        int_config.gyro_int_config_2.int_pin_cfg.enable_int_pin = BMI090L_DISABLE;
        int_config.gyro_int_config_2.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
        int_config.gyro_int_config_2.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;

        rslt = bmi090la_set_data_sync_int_config(&int_config, bmi090ldev);
    }

    if (rslt != BMI090L_OK)
    {
        printf("BMI090L data synchronization disable interrupt configuration failure!\n");
    }
}

/*!
 *  @brief Main Function where the execution getting started to test the code.
 *
 *  @param[in] argc
 *  @param[in] argv
 *
 *  @return status
 *
 */
int main(int argc, char *argv[])
{
    struct bmi090l_dev bmi090l;
    int8_t rslt;

    struct bmi090l_sensor_data bmi090l_accel, bmi090l_gyro;

    /* Interface reference is given as a parameter
     *         For I2C : BMI090L_I2C_INTF
     *         For SPI : BMI090L_SPI_INTF
     */
    rslt = bmi090l_interface_init(&bmi090l, BMI090L_SPI_INTF);

    /* Initialize the sensors */
    init_bmi090l(&bmi090l);

#if defined(MCU_APP20)
    coines_attach_interrupt(COINES_SHUTTLE_PIN_20, bmi090l_data_sync_int, COINES_PIN_INTERRUPT_FALLING_EDGE);
#elif defined(MCU_APP30)
    coines_attach_interrupt(COINES_MINI_SHUTTLE_PIN_1_6, bmi090l_data_sync_int, COINES_PIN_INTERRUPT_FALLING_EDGE);
#endif

    /* Enable data ready interrupts*/
    enable_bmi090l_data_synchronization_interrupt(&bmi090l);

    uint32_t start_time = coines_get_millis();

    /* Run data synchronization for 1s before disabling interrupts */
    while (coines_get_millis() - start_time < 1000)
    {
        if (data_sync_int == true)
        {
            data_sync_int = false;

            bmi090la_get_synchronized_data(&bmi090l_accel, &bmi090l_gyro, &bmi090l);
            count++;

            /*
             * Wait time to collect the accel samples for the datasync feature
             */
            if (count >= 2)
            {
                printf("ax:%d \t ay:%d \t az:%d \t gx:%d \t gy:%d \t gz:%d \t t(ms):%lu\n",
                       bmi090l_accel.x,
                       bmi090l_accel.y,
                       bmi090l_accel.z,
                       bmi090l_gyro.x,
                       bmi090l_gyro.y,
                       bmi090l_gyro.z,
                       coines_get_millis());
            }
        }
    }

    /* Reset count value */
    count = 0;

    /* Disable data ready interrupts */
    disable_bmi090l_data_synchronization_interrupt(&bmi090l);

    bmi090l_coines_deinit();

    return rslt;
}

/* bmi090l data sync interrupt callback */
void bmi090l_data_sync_int(uint32_t param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    
    data_sync_int = true;
}

