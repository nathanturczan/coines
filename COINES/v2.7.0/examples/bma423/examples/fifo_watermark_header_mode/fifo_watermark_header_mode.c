/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <stdio.h>
#include "bma423.h"
#include "common.h"

/******************************************************************************/
/*!                Macro definition                                           */

/*! Buffer size allocated to store raw FIFO data */
#define BMA423_FIFO_RAW_DATA_BUFFER_SIZE  UINT16_C(1024)

/*! Length of data to be read from FIFO */
#define BMA423_FIFO_RAW_DATA_USER_LENGTH  UINT16_C(1024)

/*! Setting a watermark level in FIFO */
#define BMA423_FIFO_WATERMARK_LEVEL       UINT16_C(650)

/*! Number of accel frames to be extracted from FIFO
 * Calculation:
 * fifo_watermark_level = 650, accel_frame_len = 6, header_byte = 1.
 * fifo_accel_frame_count = (650 / (6 + 1)) = 93 frames
 * NOTE: Extra frames are read in order to get sensor time
 */
#define BMA423_FIFO_ACCEL_FRAME_COUNT     UINT8_C(100)

/******************************************************************************/
/*!            Function                                                       */

/* This function starts the execution of program */
int main(void)
{
    /* Status of API are returned to this variable */
    int8_t rslt;

    /* Accelerometer configuration structure */
    struct bma4_accel_config acc_conf = { 0 };

    /* Sensor initialization configuration */
    struct bma4_dev dev = { 0 };

    /* Number of accelerometer frames */
    uint16_t accel_length;

    /* Variable to idx bytes */
    uint16_t idx = 0;

    /* Variable to loop test case */
    uint8_t loop = 1;

    /* Number of bytes of FIFO data
     * NOTE : Dummy byte (for SPI Interface) required for FIFO data read must be given as part of array size
     */
    uint8_t fifo_data[BMA423_FIFO_RAW_DATA_BUFFER_SIZE + BMA4_SENSORTIME_OVERHEAD_BYTE] = { 0 };

    /* Array of accelerometer frames -> Total bytes =
     * 100 * (6 axes bytes(+/- x,y,z) + 1 header byte) = 700 bytes */
    struct bma4_accel fifo_accel_data[BMA423_FIFO_ACCEL_FRAME_COUNT] = { { 0 } };

    /* Initialize FIFO frame structure */
    struct bma4_fifo_frame fifoframe = { 0 };

    /* Variable that contains interrupt status value */
    uint16_t int_status = 0;

    /* Variable to hold the length of FIFO data */
    uint16_t fifo_length = 0;

    uint16_t watermark = 0;

    /* To set the watermark level in FIFO */
    uint16_t wm_lvl;

    /* Interface reference is given as a parameter
     *         For I2C : BMA4_I2C_INTF
     *         For SPI : BMA4_SPI_INTF
     * Variant information given as parameter
     *         For B variant        : BMA42X_B_VARIANT
     *         For Non-B variant    : BMA42X_VARIANT
     */
    rslt = bma4_interface_init(&dev, BMA4_I2C_INTF, BMA42X_VARIANT);
    bma4_error_codes_print_result("bma4_interface_init", rslt);

    /* Initialize BMA423 */
    rslt = bma423_init(&dev);
    bma4_error_codes_print_result("bma423_init status", rslt);

    /* Accelerometer configuration settings */
    acc_conf.odr = BMA4_OUTPUT_DATA_RATE_25HZ;
    acc_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    acc_conf.range = BMA4_ACCEL_RANGE_2G;
    acc_conf.perf_mode = BMA4_CIC_AVG_MODE;

    /* Set the accel configurations */
    rslt = bma4_set_accel_config(&acc_conf, &dev);
    bma4_error_codes_print_result("bma4_set_accel_config status", rslt);

    /* NOTE : Enable accel after set of configurations */
    rslt = bma4_set_accel_enable(BMA4_ENABLE, &dev);
    bma4_error_codes_print_result("bma4_set_accel_enable status", rslt);

    /* Disabling advance power save mode as FIFO data is not accessible in advance low power mode */
    rslt = bma4_set_advance_power_save(BMA4_DISABLE, &dev);
    bma4_error_codes_print_result("bma4_set_advance_power_save status", rslt);

    /* Clear FIFO configuration register */
    rslt = bma4_set_fifo_config(BMA4_FIFO_ALL, BMA4_DISABLE, &dev);
    bma4_error_codes_print_result("bma4_set_fifo_config disable status", rslt);

    /* Set FIFO configuration by enabling accel.
     * NOTE 1: The header mode is enabled by default.
     * NOTE 2: By default the FIFO operating mode is FIFO mode. */
    rslt = bma4_set_fifo_config(BMA4_FIFO_ACCEL | BMA4_FIFO_HEADER | BMA4_FIFO_TIME, BMA4_ENABLE, &dev);
    bma4_error_codes_print_result("bma4_set_fifo_config enable status", rslt);

    /* Update FIFO structure */
    fifoframe.data = fifo_data;
    fifoframe.length = BMA423_FIFO_RAW_DATA_USER_LENGTH + BMA4_SENSORTIME_OVERHEAD_BYTE;

    printf("FIFO is configured in header mode\n");

    rslt = bma423_map_interrupt(BMA4_INTR1_MAP, BMA4_FIFO_WM_INT, BMA4_ENABLE, &dev);
    bma4_error_codes_print_result("bma4_map_interrupt status", rslt);

    wm_lvl = BMA423_FIFO_WATERMARK_LEVEL;
    rslt = bma4_set_fifo_wm(wm_lvl, &dev);
    bma4_error_codes_print_result("bma4_set_fifo_wm status", rslt);

    while (loop <= 10)
    {
        rslt = bma423_read_int_status(&int_status, &dev);
        bma4_error_codes_print_result("bma4_read_int_status", rslt);

        if ((rslt == BMA4_OK) && (int_status & BMA4_FIFO_WM_INT))
        {
            printf("\nIteration :%d\n", loop);

            rslt = bma4_get_fifo_wm(&watermark, &dev);
            bma4_error_codes_print_result("bma4_get_fifo_wm status", rslt);
            printf("FIFO watermark level : %d\n", watermark);

            rslt = bma4_get_fifo_length(&fifo_length, &dev);
            bma4_error_codes_print_result("bma4_get_fifo_length status", rslt);

            printf("FIFO data bytes available : %d\n", fifo_length);
            printf("FIFO data bytes requested : %d\n", fifoframe.length);

            /* Read FIFO data */
            rslt = bma4_read_fifo_data(&fifoframe, &dev);
            bma4_error_codes_print_result("bma4_read_fifo_data status", rslt);

            accel_length = BMA423_FIFO_ACCEL_FRAME_COUNT;

            if (rslt == BMA4_OK)
            {
                printf("Requested data frames before parsing: %d\n", accel_length);

                /* Parse the FIFO data to extract accelerometer data from the FIFO buffer */
                rslt = bma4_extract_accel(fifo_accel_data, &accel_length, &fifoframe, &dev);
                printf("Parsed accelerometer data frames: %d\n", accel_length);

                /* Print the parsed accelerometer data from the FIFO buffer */
                for (idx = 0; idx < accel_length; idx++)
                {
                    printf("ACCEL[%d] X : %d Y : %d Z : %d\n",
                           idx,
                           fifo_accel_data[idx].x,
                           fifo_accel_data[idx].y,
                           fifo_accel_data[idx].z);
                }

                /* Print control frames like sensor time and skipped frame count */
                printf("Skipped frame count = %d\n", fifoframe.skipped_frame_count);
                printf("Sensor time(in seconds) = %.4lf  s\r\n", (fifoframe.sensor_time * BMA4_SENSORTIME_RESOLUTION));

                loop++;
            }
        }
    }

    bma4_coines_deinit();

    return rslt;
}
