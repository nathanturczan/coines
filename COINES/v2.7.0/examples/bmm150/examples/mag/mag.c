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
/*!                            Macros                                         */

/* Macro to hold count of mag samples to be printed */
#define SAMPLE_COUNT  UINT8_C(50)

/******************************************************************************/
/*!         Static Function Declaration                                       */

/*!
 *  @brief This internal API is used to set configurations powermode, odr and interrupt mapping.
 *
 *  @param[in] dev       : Structure instance of bmm150_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_config(struct bmm150_dev *dev);

/*!
 *  @brief This internal API is used to get gyro data.
 *
 *  @param[in] dev       : Structure instance of bmm150_dev.
 *
 *  @return Status of execution.
 */
static int8_t get_data(struct bmm150_dev *dev);

/******************************************************************************/
/*!            Functions                                        */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable */
    int8_t rslt;

    /* Sensor initialization configuration. */
    struct bmm150_dev dev = { 0 };

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

        printf("CHIP ID : 0x%x\n", dev.chip_id);

        if (rslt == BMM150_OK)
        {
            rslt = set_config(&dev);
            bmm150_error_codes_print_result("set_config", rslt);

            if (rslt == BMM150_OK)
            {
                rslt = get_data(&dev);
                bmm150_error_codes_print_result("get_data", rslt);
            }
        }
    }

    bmm150_coines_deinit();

    return rslt;
}

/*!
 *  @brief This internal API is used to set configurations like powermode, odr and interrupt mapping.
 */
static int8_t set_config(struct bmm150_dev *dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    struct bmm150_settings settings;
    struct bmm150_settings get_settings = { 0 };

    /* Set powermode as normal mode */
    settings.pwr_mode = BMM150_POWERMODE_NORMAL;
    rslt = bmm150_set_op_mode(&settings, dev);
    bmm150_error_codes_print_result("bmm150_set_op_mode", rslt);

    if (rslt == BMM150_OK)
    {
        /* Setting the preset mode as Low power mode
         * i.e. data rate = 10Hz, XY-rep = 1, Z-rep = 2
         */
        settings.preset_mode = BMM150_PRESETMODE_LOWPOWER;
        rslt = bmm150_set_presetmode(&settings, dev);
        bmm150_error_codes_print_result("bmm150_set_presetmode", rslt);

        if (rslt == BMM150_OK)
        {
            /* Map the data interrupt pin */
            settings.int_settings.drdy_pin_en = BMM150_INT_ENABLE;
            rslt = bmm150_set_sensor_settings(BMM150_SEL_DRDY_PIN_EN, &settings, dev);
            bmm150_error_codes_print_result("bmm150_set_sensor_settings", rslt);

            rslt = bmm150_get_sensor_settings(&get_settings, dev);
            printf("drdy_en %d\n", get_settings.int_settings.drdy_pin_en);
        }
    }

    return rslt;
}

/*!
 *  @brief This internal API is used to get gyro data.
 */
static int8_t get_data(struct bmm150_dev *dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to store mag x,y,z data */
    struct bmm150_mag_data mag_data = { 0 };

    /* Variable to store interrupt status values */
    uint16_t int_status = 0;

    /* Variable that holds count of samples printed */
    uint8_t count = 0;

    printf("Magnetometer X,Y,Z data read based on data ready interrupt\n\n");

#ifdef BMM150_USE_FIXED_POINT
    printf("Magnetometer data contains fraction part (last 4 bits) and decimal part\n\n");
#endif

    /* Reading the mag data */
    while (1)
    {
        /* Get the interrupt status */
        rslt = bmm150_get_interrupt_status(&int_status, dev);
        bmm150_error_codes_print_result("bmm150_read_mag_data", rslt);

        if (int_status & BMM150_INT_DATA_READY)
        {
            /* Read mag data */
            rslt = bmm150_read_mag_data(&mag_data, dev);
            bmm150_error_codes_print_result("bmm150_read_mag_data", rslt);

#ifdef BMM150_USE_FLOATING_POINT
            printf("MAG DATA[%d]  X : %.4lf uT   Y : %.4lf uT   Z : %.4lf uT\n",
                   count,
                   mag_data.x,
                   mag_data.y,
                   mag_data.z);
#else
            printf("MAG DATA[%d]  X : %ld uT   Y : %ld uT   Z : %ld uT\n", count, (long unsigned int)mag_data.x,
                   (long unsigned int)mag_data.y, (long unsigned int)mag_data.z);
#endif

            count++;
        }

        int_status = 0;

        if (count == SAMPLE_COUNT)
        {
            break;
        }
    }

    return rslt;
}
