/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    high_g.c
 * @brief   Test code to demonstrate on how to configure and use high-g
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "bmi090l.h"
#include "common.h"

/*********************************************************************/
/*                       Function Declarations                       */
/*********************************************************************/

/*!
 * @brief    This internal API is used to initialize the bmi090l sensor
 */
static void init_bmi090l(struct bmi090l_dev *bmi090ldev);

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

    /* Initialize bmi090l sensors (accel & gyro) */
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

    printf("Uploading config file !\n");
    rslt = bmi090la_apply_config_file(bmi090ldev);

    /* API uploads the bmi090l config file onto the device */
    if (rslt == BMI090L_OK)
    {
        printf("Upload done !\n");

        if (rslt == BMI090L_OK)
        {
            bmi090ldev->accel_cfg.range = BMI090L_ACCEL_RANGE_24G;
            bmi090ldev->accel_cfg.odr = BMI090L_ACCEL_ODR_50_HZ;
            bmi090ldev->accel_cfg.bw = BMI090L_ACCEL_BW_NORMAL;
            bmi090la_set_meas_conf(bmi090ldev);
        }
    }
}

static void configure_bmi090l_high_g_interrupt(struct bmi090l_dev *bmi090ldev)
{
    struct bmi090l_high_g_cfg high_g_cfg = {};
    struct bmi090l_accel_int_channel_cfg high_g_int_cfg = { };

    bmi090la_get_high_g_config(&high_g_cfg, bmi090ldev);

    /* Configure high-g settings */
    high_g_cfg.threshold = 4000; /* 4000*24g/32768 = 2.93g */
    high_g_cfg.hysteresis = 2000; /* 2000*24g/32768 = 1.46g */
    high_g_cfg.duration = 30; /* 150 ms --> 150/5       */
    high_g_cfg.enable = 1;
    high_g_cfg.select_x = 1;
    high_g_cfg.select_y = 1;
    high_g_cfg.select_z = 1;
    bmi090la_set_high_g_config(&high_g_cfg, bmi090ldev);

    /* Map high-g interrupt to INT1 */
    high_g_int_cfg.int_channel = BMI090L_INT_CHANNEL_1;
    high_g_int_cfg.int_type = BMI090L_HIGH_G_INT;
    high_g_int_cfg.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
    high_g_int_cfg.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    high_g_int_cfg.int_pin_cfg.enable_int_pin = BMI090L_ENABLE;
    bmi090la_set_int_config(&high_g_int_cfg, bmi090ldev);
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
    struct bmi090l_high_g_out high_g_out = { 0 };
    int8_t rslt;
    uint8_t status = 0;
    uint8_t interrupt_count = 0;

    /* Interface reference is given as a parameter
     *         For I2C : BMI090L_I2C_INTF
     *         For SPI : BMI090L_SPI_INTF
     */
    rslt = bmi090l_interface_init(&bmi090l, BMI090L_I2C_INTF);
    bmi090l_check_rslt("bmi090l_interface_init", rslt);

    /* Initialize the sensors */
    init_bmi090l(&bmi090l);

    configure_bmi090l_high_g_interrupt(&bmi090l);

    printf("Perform motion to detect High-g\n");
    printf("Direction Value : 1 for negative axis, 0 for positive axis\n\n");

    while (1)
    {
        rslt = bmi090la_get_feat_int_status(&status, &bmi090l);
        bmi090l_check_rslt("bmi090la_get_feat_int_status", rslt);

        if (status & BMI090L_ACCEL_HIGH_G_INT)
        {
            rslt = bmi090la_get_high_g_output(&high_g_out, &bmi090l);
            bmi090l_check_rslt("bmi090la_get_high_g_output", rslt);

            printf("High-g detection %d :: \t", interrupt_count);

            if (high_g_out.x == 1)
            {
                printf("Axis X in Direction %d\n", high_g_out.direction);
            }
            else if (high_g_out.y == 1)
            {
                printf("Axis Y in Direction %d\n", high_g_out.direction);
            }
            else if (high_g_out.z == 1)
            {
                printf("Axis Z in Direction %d\n", high_g_out.direction);
            }
            else
            {
                printf("Invalid Output\n");
            }

            interrupt_count++;
            if (interrupt_count == 10)
            {
                printf("High-g testing done. Exiting! \n");
                break;
            }
        }
    }

    bmi090l_coines_deinit();

    return rslt;
}
