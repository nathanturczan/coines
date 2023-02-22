/**\
 * Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    gyro_fifo_watermark.c
 * @brief   Example file how to read bmi090l gyro FIFO watermark data
 */

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include <stdlib.h>

#include "bmi090l.h"
#include "common.h"

/******************************************************************************/
/*!                  Macros                                                   */

/* Buffer size allocated to store raw FIFO data */
#define BMI090L_FIFO_RAW_DATA_BUFFER_SIZE        UINT16_C(800)

/* Length of data to be read from FIFO */
#define BMI090L_FIFO_RAW_DATA_USER_LENGTH        UINT16_C(800)

/* Watermark level */
#define BMI090L_FIFO_WATERMARK_LEVEL             UINT8_C(50)

/* Number of Gyro frames to be extracted from FIFO */
#define BMI090L_FIFO_EXTRACTED_DATA_FRAME_COUNT  UINT8_C(50)

/******************************************************************************/
/*!                   Static Functions                                        */

/*!
 *  @brief This internal API is used to initializes the bmi090l sensor
 *  settings like power mode and OSRS settings.
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

    if (rslt == BMI090L_OK)
    {
        /* Max read/write length (maximum supported length is 32).
         * To be set by the user */
        bmi090ldev->read_write_len = 32;

        bmi090ldev->gyro_cfg.power = BMI090L_GYRO_PM_NORMAL;
        rslt = bmi090lg_set_power_mode(bmi090ldev);
    }

    if (rslt == BMI090L_OK)
    {
        bmi090ldev->gyro_cfg.odr = BMI090L_GYRO_BW_32_ODR_100_HZ;
        bmi090ldev->gyro_cfg.range = BMI090L_GYRO_RANGE_125_DPS;
        bmi090ldev->gyro_cfg.bw = BMI090L_GYRO_BW_32_ODR_100_HZ;

        rslt = bmi090lg_set_meas_conf(bmi090ldev);
        bmi090l_check_rslt("bmi090lg_set_meas_conf", rslt);
    }
}

/*!
 *  @brief This API is used to enable bmi090l interrupt
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static int8_t enable_bmi090l_interrupt(struct bmi090l_dev *bmi090ldev)
{
    int8_t rslt;

    /* BMI090L gyro int config */
    struct bmi090l_gyro_int_channel_cfg gyro_int_config;

    /* Set gyro interrupt pin configuration */
    gyro_int_config.int_channel = BMI090L_INT_CHANNEL_3;
    gyro_int_config.int_type = BMI090L_GYRO_INT_FIFO_WM;
    gyro_int_config.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
    gyro_int_config.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    gyro_int_config.int_pin_cfg.enable_int_pin = BMI090L_ENABLE;

    /* Enable gyro fifo interrupt channel */
    rslt = bmi090lg_set_int_config((const struct bmi090l_gyro_int_channel_cfg*)&gyro_int_config, bmi090ldev);
    bmi090l_check_rslt("bmi090lg_set_int_config", rslt);

    rslt = bmi090lg_enable_watermark(BMI090L_ENABLE, bmi090ldev);
    bmi090l_check_rslt("bmi090lg_enable_watermark", rslt);

    return rslt;
}

/*!
 *  @brief This API is used to disable bmi090l interrupt
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static int8_t disable_bmi090l_interrupt(struct bmi090l_dev *bmi090ldev)
{
    int8_t rslt;

    /* BMI090L gyro int config */
    struct bmi090l_gyro_int_channel_cfg gyro_int_config;

    /* Set gyro interrupt pin configuration */
    gyro_int_config.int_channel = BMI090L_INT_CHANNEL_3;
    gyro_int_config.int_type = BMI090L_GYRO_INT_FIFO_WM;
    gyro_int_config.int_pin_cfg.output_mode = BMI090L_INT_MODE_PUSH_PULL;
    gyro_int_config.int_pin_cfg.lvl = BMI090L_INT_ACTIVE_HIGH;
    gyro_int_config.int_pin_cfg.enable_int_pin = BMI090L_DISABLE;

    /* Disable gyro fifo interrupt channel */
    rslt = bmi090lg_set_int_config((const struct bmi090l_gyro_int_channel_cfg*)&gyro_int_config, bmi090ldev);
    bmi090l_check_rslt("bmi090lg_set_int_config", rslt);

    rslt = bmi090lg_enable_watermark(BMI090L_DISABLE, bmi090ldev);
    bmi090l_check_rslt("bmi090lg_enable_watermark", rslt);

    return rslt;
}

/******************************************************************************/
/*!            Functions                                        */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable */
    int8_t rslt;

    /* Structure instance of bmi090l_dev */
    struct bmi090l_dev bmi090l;

    /* Gyroscope fifo configurations */
    struct bmi090l_gyr_fifo_config gyr_conf = { 0 };

    /* Fifo frame structure */
    struct bmi090l_fifo_frame fifo = { 0 };

    /* Number of gyroscope frames */
    uint16_t gyro_length = BMI090L_FIFO_EXTRACTED_DATA_FRAME_COUNT;

    /* Variable to index bytes */
    uint16_t idx = 0;

    /* Variable that holds loop count of fifo example */
    uint8_t try = 1;

    /* Variable that holds interrupt status */
    uint8_t status = 0;

    /* Number of bytes of FIFO data */
    uint8_t fifo_data[BMI090L_FIFO_RAW_DATA_BUFFER_SIZE] = { 0 };

    /* Variable to hold the bmi090l gyro data */
    struct bmi090l_sensor_data bmi090l_gyro[BMI090L_FIFO_EXTRACTED_DATA_FRAME_COUNT] = { { 0 } };

    /* Interface given as parameter
     * For I2C : BMI090L_I2C_INTF
     * For SPI : BMI090L_SPI_INTF
     */
    rslt = bmi090l_interface_init(&bmi090l, BMI090L_I2C_INTF);
    bmi090l_check_rslt("bmi090l_interface_init", rslt);

    init_bmi090l(&bmi090l);

    if (rslt == BMI090L_OK)
    {
        /* Enable FIFO watermark interrupts */
        rslt = enable_bmi090l_interrupt(&bmi090l);
        bmi090l_check_rslt("enable_bmi090l_interrupt", rslt);

        printf("Gyro FIFO watermark interrupt data\n");
        if (rslt == BMI090L_OK)
        {
            gyr_conf.mode = BMI090L_GYRO_FIFO_MODE;
            gyr_conf.wm_level = BMI090L_FIFO_WATERMARK_LEVEL;
            gyr_conf.tag = BMI090L_GYRO_FIFO_TAG_DISABLED;

            rslt = bmi090lg_set_fifo_config(&gyr_conf, &bmi090l);
            bmi090l_check_rslt("bmi090lg_set_fifo_config", rslt);

            /* Update FIFO structure */
            fifo.data = fifo_data;
            fifo.length = BMI090L_FIFO_RAW_DATA_USER_LENGTH;

            while (try <= 10)
            {
                rslt = bmi090lg_get_data_int_status(&status, &bmi090l);
                bmi090l_check_rslt("bmi090lg_set_fifo_config", rslt);

                if (status & BMI090L_GYRO_FIFO_WM_INT)
                {
                    printf("\nIteration : %d\n", try);
                    printf("Fifo watermark level : %d\n", gyr_conf.wm_level);

                    gyro_length = BMI090L_FIFO_EXTRACTED_DATA_FRAME_COUNT;

                    rslt = bmi090lg_get_fifo_config(&gyr_conf, &bmi090l);
                    bmi090l_check_rslt("bmi090lg_get_fifo_config", rslt);

                    rslt = bmi090lg_get_fifo_length(&gyr_conf, &fifo);
                    bmi090l_check_rslt("bmi090lg_get_fifo_length", rslt);

                    /* Read FIFO data */
                    rslt = bmi090lg_read_fifo_data(&fifo, &bmi090l);
                    bmi090l_check_rslt("bmi090lg_read_fifo_data", rslt);

                    printf("Requested data frames before parsing: %d\n", gyro_length);
                    printf("FIFO length available : %d\n", fifo.length);

                    /* Parse the FIFO data to extract gyroscope data from the FIFO buffer */
                    bmi090lg_extract_gyro(bmi090l_gyro, &gyro_length, &gyr_conf, &fifo);

                    printf("Parsed gyroscope frames: %d\n", gyr_conf.frame_count);

                    /* Print the parsed gyroscope data from the FIFO buffer */
                    for (idx = 0; idx < gyr_conf.frame_count; idx++)
                    {
                        printf("GYRO[%d] X : %d\t Y : %d\t Z : %d\n",
                               idx,
                               bmi090l_gyro[idx].x,
                               bmi090l_gyro[idx].y,
                               bmi090l_gyro[idx].z);
                    }

                    try++;
                }
            }
        }
    }

    /* Disable FIFO watermark interrupts */
    rslt = disable_bmi090l_interrupt(&bmi090l);

    bmi090l_coines_deinit();

    return rslt;
}
