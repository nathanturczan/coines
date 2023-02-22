/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    axis_remap.c
 * @brief   Test code to demonstrate on axis remap feature
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

/*!
 * @brief This internal API is used to configure accel and gyro data ready interrupts
 */
static void configure_accel_data_ready_interrupts(struct bmi090l_dev *bmi090ldev);

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
            bmi090ldev->accel_cfg.range = BMI090L_ACCEL_RANGE_3G;
            bmi090la_set_meas_conf(bmi090ldev);
        }
    }
}

static void configure_accel_data_ready_interrupts(struct bmi090l_dev *bmi090ldev)
{
    int8_t rslt;
    struct bmi090l_accel_int_channel_cfg accel_int_config;

    /* Configure the Interrupt configurations for accel */
    accel_int_config.int_channel = BMI090L_INT_CHANNEL_1;
    accel_int_config.int_type = BMI090L_ACCEL_DATA_RDY_INT;
    accel_int_config.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    accel_int_config.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
    accel_int_config.int_pin_cfg.enable_int_pin = BMI090L_ENABLE;

    /* Set the interrupt configuration */
    rslt = bmi090la_set_int_config(&accel_int_config, bmi090ldev);

    if (rslt != BMI090L_OK)
    {
        printf("Failure in interrupt configurations \n");
        exit(COINES_E_FAILURE);
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
    int8_t rslt = BMI090L_OK;
    struct bmi090l_dev bmi090l;
    uint8_t status = 0;
    struct bmi090l_remap remap_data = { 0 };
    struct bmi090l_sensor_data accel;

    char data_array[13][14] =
    { { 0 }, { "BMI090L_X" }, { "BMI090L_Y" }, { 0 }, { "BMI090L_Z" }, { 0 }, { 0 }, { 0 }, { 0 }, { "BMI090L_NEG_X" },
      { "BMI090L_NEG_Y" }, { 0 }, { "BMI090L_NEG_Z" } };

    /* Interface reference is given as a parameter
     *         For I2C : BMI090L_I2C_INTF
     *         For SPI : BMI090L_SPI_INTF
     */
    rslt = bmi090l_interface_init(&bmi090l, BMI090L_I2C_INTF);
    bmi090l_check_rslt("bmi090l_interface_init", rslt);

    /* Initialize the sensors */
    init_bmi090l(&bmi090l);

    configure_accel_data_ready_interrupts(&bmi090l);

    printf("\nAXIS_REMAP_FUNC_TEST 1\n");
    printf("Get sensor data of re-mapped axes\n");

    rslt = bmi090la_get_remap_axes(&remap_data, &bmi090l);
    bmi090l_check_rslt("bmi090la_get_remap_axes", rslt);

    printf("Re-mapped x value = %s\n", data_array[remap_data.x]);
    printf("Re-mapped y value = %s\n", data_array[remap_data.y]);
    printf("Re-mapped z value = %s\n", data_array[remap_data.z]);

    printf("Expected Re-mapped x value = BMI090L_X\n");
    printf("Expected Re-mapped y value = BMI090L_Y\n");
    printf("Expected Re-mapped z value = BMI090L_Z\n");

    if ((remap_data.x == BMI090L_X) && (remap_data.y == BMI090L_Y) && (remap_data.z == BMI090L_Z))
    {
        printf(">> PASS\n");
    }
    else
    {
        printf(">> FAIL\n");
    }

    printf("Print mapped data\n");

    while (1)
    {
        /* Read accel data ready interrupt status */
        rslt = bmi090la_get_data_int_status(&status, &bmi090l);

        if (status & BMI090L_ACCEL_DATA_READY_INT)
        {
            rslt = bmi090la_get_data(&accel, &bmi090l);
            bmi090l_check_rslt("bmi090la_get_data", rslt);

            printf("Accel :: X = %d Y = %d Z = %d\n", accel.x, accel.y, accel.z);

            break;
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 2\n");
    printf("Get sensor data of re-mapped axes\n");

    remap_data.x = BMI090L_NEG_Y;
    remap_data.y = BMI090L_Z;
    remap_data.z = BMI090L_NEG_X;

    rslt = bmi090la_set_remap_axes(&remap_data, &bmi090l);
    bmi090l_check_rslt("bmi090la_set_remap_axes", rslt);

    if (rslt == BMI090L_OK)
    {
        rslt = bmi090la_get_remap_axes(&remap_data, &bmi090l);
        bmi090l_check_rslt("bmi090la_get_remap_axes", rslt);

        if (rslt == BMI090L_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remap_data.x]);
            printf("Re-mapped y value = %s\n", data_array[remap_data.y]);
            printf("Re-mapped z value = %s\n", data_array[remap_data.z]);
        }

        printf("Expected Re-mapped x value = BMI090L_NEG_Y\n");
        printf("Expected Re-mapped y value = BMI090L_Z\n");
        printf("Expected Re-mapped z value = BMI090L_NEG_X\n");

        if ((remap_data.x == BMI090L_NEG_Y) && (remap_data.y == BMI090L_Z) && (remap_data.z == BMI090L_NEG_X))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf(">> FAIL\n");
        }
    }

    printf("Print mapped data\n");

    while (1)
    {
        /* Read accel data ready interrupt status */
        rslt = bmi090la_get_data_int_status(&status, &bmi090l);

        if (status & BMI090L_ACCEL_DATA_READY_INT)
        {
            rslt = bmi090la_get_data(&accel, &bmi090l);
            bmi090l_check_rslt("bmi090la_get_data", rslt);

            printf("Accel :: X = %d Y = %d Z = %d\n", accel.x, accel.y, accel.z);

            break;
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 3\n");
    printf("Get sensor data of re-mapped axes - 2nd combination\n");

    remap_data.x = BMI090L_NEG_Z;
    remap_data.y = BMI090L_NEG_X;
    remap_data.z = BMI090L_Y;

    rslt = bmi090la_set_remap_axes(&remap_data, &bmi090l);
    bmi090l_check_rslt("bmi090la_set_remap_axes", rslt);

    if (rslt == BMI090L_OK)
    {
        rslt = bmi090la_get_remap_axes(&remap_data, &bmi090l);
        bmi090l_check_rslt("bmi090la_get_remap_axes", rslt);

        if (rslt == BMI090L_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remap_data.x]);
            printf("Re-mapped y value = %s\n", data_array[remap_data.y]);
            printf("Re-mapped z value = %s\n", data_array[remap_data.z]);
        }

        printf("Expected Re-mapped x value = BMI090L_NEG_Z\n");
        printf("Expected Re-mapped y value = BMI090L_NEG_X\n");
        printf("Expected Re-mapped z value = BMI090L_Y\n");

        if ((remap_data.x == BMI090L_NEG_Z) && (remap_data.y == BMI090L_NEG_X) && (remap_data.z == BMI090L_Y))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf(">> FAIL\n");
        }
    }

    printf("Print mapped data\n");

    while (1)
    {
        /* Read accel data ready interrupt status */
        rslt = bmi090la_get_data_int_status(&status, &bmi090l);

        if (status & BMI090L_ACCEL_DATA_READY_INT)
        {
            rslt = bmi090la_get_data(&accel, &bmi090l);
            bmi090l_check_rslt("bmi090la_get_data", rslt);

            printf("Accel :: X = %d Y = %d Z = %d\n", accel.x, accel.y, accel.z);

            break;
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 4\n");
    printf("Get sensor data of re-mapped axes - 3rd combination\n");

    remap_data.x = BMI090L_Y;
    remap_data.y = BMI090L_Z;
    remap_data.z = BMI090L_X;

    rslt = bmi090la_set_remap_axes(&remap_data, &bmi090l);
    bmi090l_check_rslt("bmi090la_set_remap_axes", rslt);
    if (rslt == BMI090L_OK)
    {
        rslt = bmi090la_get_remap_axes(&remap_data, &bmi090l);
        bmi090l_check_rslt("bmi090la_get_remap_axes", rslt);

        if (rslt == BMI090L_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remap_data.x]);
            printf("Re-mapped y value = %s\n", data_array[remap_data.y]);
            printf("Re-mapped z value = %s\n", data_array[remap_data.z]);
        }

        printf("Expected Re-mapped x value = BMI090L_Y\n");
        printf("Expected Re-mapped y value = BMI090L_Z\n");
        printf("Expected Re-mapped z value = BMI090L_X\n");

        if ((remap_data.x == BMI090L_Y) && (remap_data.y == BMI090L_Z) && (remap_data.z == BMI090L_X))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf(">> FAIL\n");
        }
    }

    printf("Print mapped data\n");

    while (1)
    {
        /* Read accel data ready interrupt status */
        rslt = bmi090la_get_data_int_status(&status, &bmi090l);

        if (status & BMI090L_ACCEL_DATA_READY_INT)
        {
            rslt = bmi090la_get_data(&accel, &bmi090l);
            bmi090l_check_rslt("bmi090la_get_data", rslt);

            printf("Accel :: X = %d Y = %d Z = %d\n", accel.x, accel.y, accel.z);

            break;
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 5\n");
    printf("Get sensor data of re-mapped axes - 4th combination\n");

    remap_data.x = BMI090L_NEG_X;
    remap_data.y = BMI090L_NEG_Y;
    remap_data.z = BMI090L_NEG_Z;

    rslt = bmi090la_set_remap_axes(&remap_data, &bmi090l);
    bmi090l_check_rslt("bmi090la_set_remap_axes", rslt);

    if (rslt == BMI090L_OK)
    {
        rslt = bmi090la_get_remap_axes(&remap_data, &bmi090l);
        bmi090l_check_rslt("bmi090la_get_remap_axes", rslt);

        if (rslt == BMI090L_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remap_data.x]);
            printf("Re-mapped y value = %s\n", data_array[remap_data.y]);
            printf("Re-mapped z value = %s\n", data_array[remap_data.z]);
        }

        printf("Expected Re-mapped x value = BMI090L_NEG_X\n");
        printf("Expected Re-mapped y value = BMI090L_NEG_Y\n");
        printf("Expected Re-mapped z value = BMI090L_NEG_Z\n");

        if ((remap_data.x == BMI090L_NEG_X) && (remap_data.y == BMI090L_NEG_Y) && (remap_data.z == BMI090L_NEG_Z))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf(">> FAIL\n");
        }
    }

    printf("Print mapped data\n");

    while (1)
    {
        /* Read accel data ready interrupt status */
        rslt = bmi090la_get_data_int_status(&status, &bmi090l);

        if (status & BMI090L_ACCEL_DATA_READY_INT)
        {
            rslt = bmi090la_get_data(&accel, &bmi090l);
            bmi090l_check_rslt("bmi090la_get_data", rslt);

            printf("Accel :: X = %d Y = %d Z = %d\n", accel.x, accel.y, accel.z);

            break;
        }
    }

    bmi090l_coines_deinit();

    return rslt;
}
