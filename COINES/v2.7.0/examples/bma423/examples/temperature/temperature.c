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
    int8_t rslt;
    int32_t get_temp_C = 0;
    int32_t get_temp_F = 0;
    int32_t get_temp_K = 0;
    float actual_temp = 0;
    uint8_t sample_count = 0;
    struct bma4_accel_config accel_conf = { 0 };

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
    bma4_error_codes_print_result("bma423_init status", rslt);

    /* Upload the configuration file to enable the features of the sensor. */
    rslt = bma423_write_config_file(&bma);
    bma4_error_codes_print_result("bma423_write_config status", rslt);

    /* Accelerometer configuration Setting */
    /* Output data Rate */
    accel_conf.odr = BMA4_OUTPUT_DATA_RATE_50HZ;

    /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G) */
    accel_conf.range = BMA4_ACCEL_RANGE_2G;

    /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
     * if it is set to 2, then 2^(bandwidth parameter) samples
     * are averaged, resulting in 4 averaged samples
     * Note1 : For more information, refer the datasheet.
     * Note2 : A higher number of averaged samples will result in a less noisier signal, but
     * this has an adverse effect on the power consumed.
     */
    accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /* Enable the filter performance mode where averaging of samples
     * will be done based on above set bandwidth and ODR.
     * There are two modes
     *  0 -> Averaging samples (Default)
     *  1 -> No averaging
     * For more info on No Averaging mode refer datasheet.
     */
    accel_conf.perf_mode = BMA4_CIC_AVG_MODE;

    /* Set the accel configurations */
    rslt = bma4_set_accel_config(&accel_conf, &bma);
    bma4_error_codes_print_result("bma4_set_accel_config status", rslt);

    /* NOTE : As per datasheet, temperature is always on WHEN accelerometer is active */

    /* NOTE : Enable accel after set of configurations */
    rslt = bma4_set_accel_enable(1, &bma);
    bma4_error_codes_print_result("bma4_set_accel_enable status", rslt);

    printf("Temperature Data in Degrees\n");
    printf("    Celsius \t   Fahrenheit \t    Kelvin\n");

    while (sample_count < 10)
    {
        /* NOTE : As per datasheet, Temperature value is updated every 1.28s
         * Hence, delay of 3s is given before every temperature read
         */
        bma.delay_us(3000000, bma.intf_ptr);

        /* Get temperature in degree C */
        rslt = bma4_get_temperature(&get_temp_C, BMA4_DEG, &bma);
        bma4_error_codes_print_result("bma4_get_temperature in degree C status", rslt);

        /* Get temperature in degree F */
        rslt = bma4_get_temperature(&get_temp_F, BMA4_FAHREN, &bma);
        bma4_error_codes_print_result("bma4_get_temperature in degree F status", rslt);

        /* Get temperature in degree K */
        rslt = bma4_get_temperature(&get_temp_K, BMA4_KELVIN, &bma);
        bma4_error_codes_print_result("bma4_get_temperature in degree K status", rslt);

        /* Scale the output to get the actual temperature  */
        actual_temp = (float)get_temp_C / (float)BMA4_SCALE_TEMP;
        printf("%10.2f \t", actual_temp);
        actual_temp = (float)get_temp_F / (float)BMA4_SCALE_TEMP;
        printf("%10.2f \t", actual_temp);
        actual_temp = (float)get_temp_K / (float)BMA4_SCALE_TEMP;
        printf("%10.2f \n", actual_temp);

        sample_count++;
    }

    bma4_coines_deinit();

    return rslt;
}
