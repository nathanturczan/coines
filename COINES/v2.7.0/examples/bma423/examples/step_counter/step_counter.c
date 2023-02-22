/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <stdio.h>
#include "bma423.h"
#include "common.h"

int main(void)
{
    struct bma4_dev bma;
    struct bma4_accel_config accel_conf;
    int8_t rslt;

    /* Variable to get the step counter output */
    uint32_t step_out = 0;

    /* Variable to get the interrupt status */
    uint16_t int_status = 0;

    /* Interface reference is given as a parameter
     *         For I2C : BMA4_I2C_INTF
     *         For SPI : BMA4_SPI_INTF
     * Variant information given as parameter
     *         For B variant        : BMA42X_B_VARIANT
     *         For Non-B variant    : BMA42X_VARIANT
     */
    rslt = bma4_interface_init(&bma, BMA4_I2C_INTF, BMA42X_VARIANT);
    bma4_error_codes_print_result("bma4_interface_init", rslt);

    /* Sensor initialization */
    rslt = bma423_init(&bma);
    bma4_error_codes_print_result("bma423_init", rslt);

    /* Upload the configuration file to enable the features of the sensor. */
    rslt = bma423_write_config_file(&bma);
    bma4_error_codes_print_result("bma423_write_config", rslt);

    /* Accelerometer Configuration Setting */
    /* Output data Rate */
    accel_conf.odr = BMA4_OUTPUT_DATA_RATE_100HZ;

    /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G) */
    accel_conf.range = BMA4_ACCEL_RANGE_2G;

    /* Bandwidth configure number of sensor samples required to average
     * if value = 2, then 4 samples are averaged
     * averaged samples = 2^(val(accel bandwidth))
     * Note1 : More info refer datasheets
     * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but since the
     * performance power mode phase is increased, the power consumption will also rise.
     */
    accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /* Enable the filter performance mode where averaging of samples
     * will be done based on above set bandwidth and ODR.
     * There are two modes
     *  0 -> Averaging samples (Default)
     *  1 -> No averaging
     * For more info on No Averaging mode refer datasheets.
     */
    accel_conf.perf_mode = BMA4_CIC_AVG_MODE;

    /* Set the accel configurations */
    rslt = bma4_set_accel_config(&accel_conf, &bma);
    bma4_error_codes_print_result("bma4_set_accel_config status", rslt);

    /* NOTE : Enable accel after set of configurations */
    rslt = bma4_set_accel_enable(1, &bma);
    bma4_error_codes_print_result("bma4_set_accel_enable status", rslt);

    /* Enable step counter */
    rslt = bma423_feature_enable(BMA423_STEP_CNTR, 1, &bma);
    bma4_error_codes_print_result("bma423_feature_enable status", rslt);

    /* Map the interrupt pin with that of step counter interrupts
     * Interrupt will  be generated when step activity is generated.
     */
    rslt = bma423_map_interrupt(BMA4_INTR1_MAP, BMA423_STEP_CNTR_INT, 1, &bma);
    bma4_error_codes_print_result("bma423_map_interrupt status", rslt);

    /* Set water-mark level 1 to get interrupt after 20 steps.
     * Range of step counter interrupt is 0 to 20460(resolution of 20 steps).
     */
    rslt = bma423_step_counter_set_watermark(1, &bma);
    bma4_error_codes_print_result("bma423_step_counter status", rslt);

    printf("\nStep counter feature is enabled\n");

    printf("Step counter watermark level is 1 (Output resolution is 20 steps)\n");

    printf("Move the board in steps for greater than 3 seconds\n");

    while (1)
    {
        /* Read the interrupt register to check whether step counter interrupt is received. */
        rslt = bma423_read_int_status(&int_status, &bma);

        /* Check if step counter interrupt is triggered */
        if ((BMA4_OK == rslt) && (int_status & BMA423_STEP_CNTR_INT))
        {
            printf("\nStep counter interrupt received when watermark level is reached (20 steps)\n");

            /* On interrupt, Get step counter output */
            rslt = bma423_step_counter_output(&step_out, &bma);
            bma4_error_codes_print_result("bma423_step_counter_output status", rslt);

            break;
        }
    }

    printf("\nThe step counter output is %lu\r\n", (long unsigned int)step_out);

    bma4_coines_deinit();

    return rslt;
}
