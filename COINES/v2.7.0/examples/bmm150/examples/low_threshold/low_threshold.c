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

/* Low threshold value to be set
 *
 * NOTE :
 * Maximum allowed value is 127
 * Minimum allowed value is -128
 * Low threshold interrupt occurs when at-least one of X,Y,Z axis reaches (LOW_THRESHOLD * 6) uT or below
 */
#define LOW_THRESHOLD  INT8_C(10)

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

    /* Variable to store interrupt status values */
    uint16_t int_status = 0;

    /* Variable to indicate when low threshold interrupt occurs */
    int16_t low_thres_value = 0;

    /* Structure to store interrupt settings */
    struct bmm150_settings settings;

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

        printf("\nLow threshold example executes to detect interrupt once in each (X,Y,Z) axis\n");

        low_thres_value = (int16_t)(((int8_t)settings.int_settings.low_threshold) * 6);

        printf("Low threshold interrupt occurs when at-least one of X,Y,Z axis reaches %d uT(decimal part) or below\n",
               low_thres_value);

#ifdef BMM150_USE_FIXED_POINT
        printf("Magnetometer data contains fraction part (last 4 bits) and decimal part\n\n");
#endif

        while (1)
        {
            /* Get the interrupt status */
            rslt = bmm150_get_interrupt_status(&int_status, &dev);

            if ((int_status & BMM150_INT_THRESHOLD_X_LOW) && (int_count_x == 0))
            {
                printf("\nLow Threshold X Axis Asserted\n");

                /* Read Mag data */
                rslt = bmm150_read_mag_data(&mag_data, &dev);

#ifdef BMM150_USE_FLOATING_POINT
                printf("MAG DATA  X : %.4lf uT   Y : %.4lf uT   Z : %.4lf uT\n", mag_data.x, mag_data.y, mag_data.z);
#else
                printf("MAG DATA  X : %ld uT   Y : %ld uT   Z : %ld uT\n", (long unsigned int)mag_data.x,
                       (long unsigned int)mag_data.y, (long unsigned int)mag_data.z);
#endif

                int_count_x = 1;

            }

            if ((int_status & BMM150_INT_THRESHOLD_Y_LOW) && (int_count_y == 0))
            {
                printf("\nLow Threshold Y Axis Asserted\n");

                /* Read Mag data */
                rslt = bmm150_read_mag_data(&mag_data, &dev);

#ifdef BMM150_USE_FLOATING_POINT
                printf("MAG DATA  X : %.4lf uT   Y : %.4lf uT   Z : %.4lf uT\n", mag_data.x, mag_data.y, mag_data.z);
#else
                printf("MAG DATA X : %ld uT   Y : %ld uT   Z : %ld uT\n", (long unsigned int)mag_data.x,
                       (long unsigned int)mag_data.y, (long unsigned int)mag_data.z);
#endif

                int_count_y = 1;
            }

            if ((int_status & BMM150_INT_THRESHOLD_Z_LOW) && (int_count_z == 0))
            {
                printf("\nLow Threshold Z Axis Asserted\n");

                /* Read Mag data */
                rslt = bmm150_read_mag_data(&mag_data, &dev);

#ifdef BMM150_USE_FLOATING_POINT
                printf("MAG DATA  X : %.4lf uT   Y : %.4lf uT   Z : %.4lf uT\n", mag_data.x, mag_data.y, mag_data.z);
#else
                printf("MAG DATA X : %ld uT   Y : %ld uT   Z : %ld uT\n", (long unsigned int)mag_data.x,
                       (long unsigned int)mag_data.y, (long unsigned int)mag_data.z);
#endif

                int_count_z = 1;
            }

            if ((int_count_x == 1) && (int_count_y == 1) && (int_count_z == 1))
            {
                printf("\nLow threshold tested for X,Y and Z axes. Exiting !\n");
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
    /* Status of api are returned to this variable. */
    int8_t rslt;

    struct bmm150_settings get_settings = { 0 };

    /* Set any threshold level below which low threshold interrupt occurs */
    settings->int_settings.low_threshold = LOW_THRESHOLD;
    rslt = bmm150_set_sensor_settings(BMM150_SEL_LOW_THRESHOLD_SETTING, settings, dev);
    bmm150_error_codes_print_result("bmm150_set_sensor_settings_1", rslt);
    printf("Low threshold - Set : %d \n", settings->int_settings.low_threshold);

    rslt = bmm150_get_sensor_settings(&get_settings, dev);
    bmm150_error_codes_print_result("bmm150_get_sensor_settings", rslt);
    printf("Low threshold - Get : %d \n", get_settings.int_settings.low_threshold);

    /* Read the default configuration from the sensor */
    settings->pwr_mode = BMM150_POWERMODE_NORMAL;
    rslt = bmm150_set_op_mode(settings, dev);
    bmm150_error_codes_print_result("bmm150_set_op_mode", rslt);

    if (rslt == BMM150_OK)
    {
        if (rslt == BMM150_OK)
        {
            /* Low interrupt pins are active low as per datasheet */
            settings->int_settings.low_int_en = BMM150_SEL_XYZ_AXES;
            rslt = bmm150_set_sensor_settings(BMM150_SEL_LOW_THRESHOLD_INT, settings, dev);
            bmm150_error_codes_print_result("bmm150_set_sensor_settings_2", rslt);

            if (rslt == BMM150_OK)
            {
                settings->int_settings.int_pin_en = BMM150_INT_ENABLE;
                rslt = bmm150_set_sensor_settings(BMM150_SEL_INT_PIN_EN, settings, dev);
                bmm150_error_codes_print_result("bmm150_set_sensor_settings_3", rslt);
            }
        }
    }

    return rslt;
}
