/**\
 * Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmm150.h"
#include "common.h"

/******************************************************************************/
/*!                           Macros                                          */

/* High threshold value to be set
 *
 * NOTE :
 * Maximum allowed value is 127 LSB
 * Minimum allowed value is -128 LSB
 * High threshold interrupt occurs when at-least one of X,Y,Z axis reaches (HIGH_THRESHOLD * 6) uT or above
 */
#define HIGH_THRESHOLD  INT8_C(20)

/******************************************************************************/
/*!         Static Function Declaration                                       */

/*!
 *  @brief This internal API is used to set configurations like threshold and interrupt mapping.
 *
 *  @param[in] settings    : Structure instance of bmm150_settings.
 *  @param[in] dev         : Structure instance of bmm150_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_config(struct bmm150_settings *settings, struct bmm150_dev *dev);

/******************************************************************************/
/*!            Functions                                        */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable */
    int8_t rslt;

    /* Sensor initialization configuration. */
    struct bmm150_dev dev = { 0 };

    /* Structure to store interrupt settings */
    struct bmm150_settings settings;

    /* Variable to store interrupt status values */
    uint16_t int_status = 0;

    /* Variable to indicate when high threshold interrupt occurs */
    int16_t high_thres_value = 0;

    /* Structure to store mag x,y,z data */
    struct bmm150_mag_data mag_data = { 0 };

    /* Interrupt status count for X,Y,Z axes */
    uint8_t int_count_x = 0, int_count_y = 0, int_count_z = 0;

    /* Interface selection is to be updated as parameter
     * For I2C :  BMM150_I2C_INTF
     * For SPI :  BMM150_SPI_INTF
     */
    rslt = bmm150_interface_selection(&dev, BMM150_SPI_INTF);
    bmm150_error_codes_print_result("bmm150_interface_selection", rslt);

    if (rslt == BMM150_OK)
    {
        rslt = bmm150_init(&dev);
        bmm150_error_codes_print_result("bmm150_init", rslt);

        if (rslt == BMM150_OK)
        {
            rslt = set_config(&settings, &dev);
            bmm150_error_codes_print_result("set_config", rslt);
        }

        printf("\nHigh threshold example executes to detect interrupt once in each (X,Y,Z) axis\n");

        high_thres_value = (int16_t)(((int8_t)settings.int_settings.high_threshold) * 6);

        printf("High threshold interrupt condition ::\n");

        printf("At-least one of X,Y,Z axis reaches %d uT(decimal part) or above\n", high_thres_value);

#ifdef BMM150_USE_FIXED_POINT
        printf("\nMagnetometer data contains fraction part (last 4 bits) and decimal part\n\n");
#endif

        while (1)
        {
            /* Get the interrupt status */
            rslt = bmm150_get_interrupt_status(&int_status, &dev);

            if ((int_status & BMM150_INT_THRESHOLD_X_HIGH) && (int_count_x == 0))
            {
                printf("\nHigh Threshold X Axis Asserted\n");

                int_count_x = 1;
            }

            if ((int_status & BMM150_INT_THRESHOLD_Y_HIGH) && (int_count_y == 0))
            {
                printf("\nHigh Threshold Y Axis Asserted\n");

                int_count_y = 1;
            }

            if ((int_status & BMM150_INT_THRESHOLD_Z_HIGH) && (int_count_z == 0))
            {
                printf("\nHigh Threshold Z Axis Asserted\n");

                int_count_z = 1;
            }

            if ((int_count_x == 1) && (int_count_y == 1) && (int_count_z == 1))
            {
                printf("\nHigh threshold tested for X,Y and Z axes. Exiting !\n");
                break;
            }
        }
    }

    bmm150_coines_deinit();

    return rslt;
}

/*!
 *  @brief This internal API is used to set configurations like threshold and interrupt mapping.
 */
static int8_t set_config(struct bmm150_settings *settings, struct bmm150_dev *dev)
{
    /* Status of API is returned to this variable. */
    int8_t rslt;

    struct bmm150_settings get_settings = { 0 };

    /* Set any threshold level above which high threshold interrupt occurs */
    settings->int_settings.high_threshold = HIGH_THRESHOLD;
    rslt = bmm150_set_sensor_settings(BMM150_SEL_HIGH_THRESHOLD_SETTING, settings, dev);
    bmm150_error_codes_print_result("bmm150_set_sensor_settings_1", rslt);
    printf("High threshold - Set : %d \n", settings->int_settings.high_threshold);

    rslt = bmm150_get_sensor_settings(&get_settings, dev);
    bmm150_error_codes_print_result("bmm150_get_sensor_settings", rslt);
    printf("High threshold - Get : %d \n", get_settings.int_settings.high_threshold);

    /* Read the default configuration from the sensor */
    settings->pwr_mode = BMM150_POWERMODE_NORMAL;
    rslt = bmm150_set_op_mode(settings, dev);
    bmm150_error_codes_print_result("bmm150_set_op_mode", rslt);

    if (rslt == BMM150_OK)
    {
        settings->int_settings.high_int_en = BMM150_SEL_XYZ_AXES;
        rslt = bmm150_set_sensor_settings(BMM150_SEL_HIGH_THRESHOLD_INT, settings, dev);
        bmm150_error_codes_print_result("bmm150_set_sensor_settings_2", rslt);

        if (rslt == BMM150_OK)
        {
            settings->int_settings.int_pin_en = BMM150_INT_ENABLE;
            rslt = bmm150_set_sensor_settings(BMM150_SEL_INT_PIN_EN, settings, dev);
            bmm150_error_codes_print_result("bmm150_set_sensor_settings_3", rslt);
        }
    }

    return rslt;
}
