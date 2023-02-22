/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    orientation.c
 * @brief   Test code to demonstrate on how to configure and use orientation interrupt
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
            bmi090ldev->accel_cfg.bw = BMI090L_ACCEL_BW_NORMAL;
            bmi090ldev->accel_cfg.odr = BMI090L_ACCEL_ODR_200_HZ;
            bmi090ldev->accel_cfg.range = BMI090L_ACCEL_RANGE_6G;
            bmi090la_set_meas_conf(bmi090ldev);
        }
    }
}

static void configure_bmi090l_orientation_interrupt(struct bmi090l_dev *bmi090ldev)
{
    struct bmi090l_orient_cfg orient_cfg = { };
    struct bmi090l_accel_int_channel_cfg orient_int_cfg = { };

    bmi090la_get_orient_config(&orient_cfg, bmi090ldev);

    orient_cfg.enable = 1;
    orient_cfg.ud_en = 1;
    orient_cfg.blocking = 3;
    orient_cfg.mode = 0;
    orient_cfg.theta = 0x28;
    orient_cfg.hysteresis = 0x80;
    bmi090la_set_orient_config(&orient_cfg, bmi090ldev);

    /* Map orientation interrupt to INT1 */
    orient_int_cfg.int_channel = BMI090L_INT_CHANNEL_1;
    orient_int_cfg.int_type = BMI090L_ORIENT_INT;
    orient_int_cfg.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
    orient_int_cfg.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    orient_int_cfg.int_pin_cfg.enable_int_pin = BMI090L_ENABLE;
    bmi090la_set_int_config(&orient_int_cfg, bmi090ldev);
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
    struct bmi090l_orient_out orient_out = { 0 };
    struct bmi090l_dev bmi090l;
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

    configure_bmi090l_orientation_interrupt(&bmi090l);

    printf("\nMove board to detect orientation\n");

    while (1)
    {
        rslt = bmi090la_get_feat_int_status(&status, &bmi090l);
        if (status & BMI090L_ACCEL_ORIENT_INT)
        {
            rslt = bmi090la_get_orient_output(&orient_out, &bmi090l);

            switch (orient_out.portrait_landscape)
            {
                case BMI090L_ORIENT_PORTRAIT_UPRIGHT:
                    printf("%d - Portrait upright\n", interrupt_count);
                    break;
                case BMI090L_ORIENT_LANDSCAPE_LEFT:
                    printf("%d - Landscape left\n", interrupt_count);
                    break;

                case BMI090L_ORIENT_PORTRAIT_UPSIDE_DOWN:
                    printf("%d - Portrait upside down\n", interrupt_count);
                    break;
                case BMI090L_ORIENT_LANDSCAPE_RIGHT:
                    printf("%d - Landscape right\n", interrupt_count);
                    break;

                default:
                    break;
            }

            switch (orient_out.faceup_down)
            {
                case BMI090L_ORIENT_FACE_UP:
                    printf("%d - Face up\n", interrupt_count);
                    break;
                case BMI090L_ORIENT_FACE_DOWN:
                    printf("%d - Face down\n", interrupt_count);
                    break;

                default:
                    break;
            }

            interrupt_count++;
            if (interrupt_count == 10)
            {
                printf("Orientation interrupt testing done. Exiting! \n");
                break;
            }
        }
    }

    bmi090l_coines_deinit();

    return rslt;
}
