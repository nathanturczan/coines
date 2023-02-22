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

    /* Loop variable */
    uint8_t iteration = 10;

    /* Define a variable to get the status */
    uint16_t int_status = 0;
    uint8_t int_line[2] = { BMA4_INTR1_MAP, BMA4_INTR2_MAP };

    /* Variable to define feature interrupts to be mapped */
    uint16_t int_map = (BMA423_SINGLE_TAP_INT | BMA423_DOUBLE_TAP_INT);

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

    /* Enable single & double tap feature */
    rslt = bma423_feature_enable(BMA423_SINGLE_TAP | BMA423_DOUBLE_TAP, 1, &bma);
    bma4_error_codes_print_result("bma423_feature_enable status", rslt);

    /* Mapping line interrupt 1 with that of two sensor feature interrupts -
     * double tap and single tap interrupt */
    rslt = bma423_map_interrupt(int_line[0], int_map, BMA4_ENABLE, &bma);
    bma4_error_codes_print_result("bma423_map_interrupt", rslt);

    if (BMA4_OK == rslt)
    {
        printf("Do Single or Double Tap the board\n");

        while (1)
        {
            /* Read the interrupt register to check whether single tap or double tap interrupt is received */
            rslt = bma423_read_int_status(&int_status, &bma);
            if (int_status & BMA423_SINGLE_TAP_INT)
            {
                printf("Single tap received\n");
                iteration--;
            }
            else if (int_status & BMA423_DOUBLE_TAP_INT)
            {
                printf("Double tap received\n");
                iteration--;
            }

            int_status = 0;

            /* Break out of the loop when iteration has reached zero */
            if (iteration == 0)
            {
                printf("Iterations are done. Exiting !");
                break;
            }
        }
    }

    bma4_coines_deinit();

    return rslt;
}
