/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi270.h"
#include "bmi2_ois.h"
#include "common.h"

/******************************************************************************/
/*!                Macro definition                                           */

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH  (9.80665f)

/******************************************************************************/
/*!           Static Function Declaration                                     */

/*!
 *  @brief This internal API is used to set configurations for accel.
 *
 *  @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_accel_config(struct bmi2_dev *bmi2_dev);

/*!
 *  @brief This internal API is used to set configurations for gyro.
 *
 *  @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_gyro_config(struct bmi2_dev *dev);

/******************************************************************************/
/*!            Functions                                        */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variable to define limit to print accel data. */
    uint8_t limit = 50;

    /* Sensor list to enable accel and gyro. */
    uint8_t n_sens = 2;
    uint8_t ois_sensor_list[2] = {BMI2_OIS_ACCEL, BMI2_OIS_GYRO};

    /* Sensor initialization configuration. */
    struct bmi2_dev bmi2_dev;
    struct bmi2_ois_dev bmi2_ois_dev;

    /* To store the gyroscope cross sensitivity value */
    int16_t ois_gyr_cross_sens_zx = 0;

    uint8_t indx = 0;

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    enum bmi2_intf intf_select = BMI2_I2C_INTF;

    rslt = bmi2_interface_init(&bmi2_dev, intf_select);
    if(rslt != BMI2_OK)
    {
		bmi2_error_codes_print_result(rslt);
        return rslt;
    }

    if (bmi2_dev.variant_feature & BMI2_GYRO_CROSS_SENS_ENABLE)
    {
        ois_gyr_cross_sens_zx = bmi2_dev.gyr_cross_sens_zx;
    }

    /* Accel configuration settings. */
    rslt = set_accel_config(&bmi2_dev);
    if(rslt != BMI2_OK)
    {   
        bmi2_error_codes_print_result(rslt);
        return rslt;
    }

    /* Gyro configuration settings. */
    rslt = set_gyro_config(&bmi2_dev);
    if(rslt != BMI2_OK)
    {   
        bmi2_error_codes_print_result(rslt);
        return rslt;
    }

    /* Initialize OIS interface. */
    rslt = bmi2_ois_interface_init(&bmi2_ois_dev);
    if(rslt != BMI2_OK)
    {   
        bmi2_error_codes_print_result(rslt);
        return rslt;
    }

    /* OIS interface must be enabled using primary interface */
    rslt = bmi2_set_ois_interface(1, &bmi2_dev);
    if(rslt != BMI2_OK)
    {   
        bmi2_error_codes_print_result(rslt);
        return rslt;
    }

    /* Enable Accel/Gyro sensors through OIS interface. */
    bmi2_ois_dev.acc_en = BMI2_OIS_ENABLE;
    bmi2_ois_dev.gyr_en = BMI2_OIS_ENABLE;
    bmi2_ois_dev.lp_filter_en = BMI2_OIS_ENABLE;
    
    rslt = bmi2_ois_set_config(&bmi2_ois_dev);
    if(rslt != BMI2_OK)
    {   
        bmi2_error_codes_print_result(rslt);
        return rslt;
    }

    printf("\nAccel and Gyro (OIS) data \n");

    for (indx = 0; indx < limit; indx++)
    {
        bmi2_ois_dev.ois_delay_us(156, bmi2_ois_dev.intf_ptr);

        /* Accel ODR is 1600hz and gyro ODR is 6400hz.Delay required for
            * accel 625us and 156us for gyro.
            * taken modules from accel and gyro ODR which results every 4th sample accel and gyro
            * read happens, rest of three samples gyro data alone will be read */
        if (indx % 4 == 0)
        {
            /* Get OIS accelerometer and gyro data through OIS interface
                * @note for sensor which support gyro cross axes sensitivity pass the
                * gyr_cross_sens_zx from the bmi2_dev structure */
            rslt = bmi2_ois_read_data(&ois_sensor_list[0], n_sens, &bmi2_ois_dev, ois_gyr_cross_sens_zx);
            if(rslt != BMI2_OK)
            {   
                bmi2_error_codes_print_result(rslt);
                return rslt;
            }

            printf("\n");

            /* Print accelerometer data */
            printf("OIS Accel x-axis = %d ", bmi2_ois_dev.acc_data.x);
            printf("OIS Accel y-axis = %d ", bmi2_ois_dev.acc_data.y);
            printf("OIS Accel z-axis = %d \n", bmi2_ois_dev.acc_data.z);

            /* Print gyro data */
            printf("OIS Gyro x-axis = %d ", bmi2_ois_dev.gyr_data.x);
            printf("OIS Gyro y-axis = %d ", bmi2_ois_dev.gyr_data.y);
            printf("OIS Gyro z-axis = %d ", bmi2_ois_dev.gyr_data.z);

            printf("\n");
        
        }
        else
        {
            /* Get OIS gyro data through OIS interface
                * @note for sensor which support gyro cross axes sensitivity pass the
                * gyr_cross_sens_zx from the bmi2_dev structure */
            rslt = bmi2_ois_read_data(&ois_sensor_list[1], 1, &bmi2_ois_dev, ois_gyr_cross_sens_zx);
            if(rslt != BMI2_OK)
            {   
                bmi2_error_codes_print_result(rslt);
                return rslt;
            }
            
            /* Print gyro data */
            printf("OIS Gyro x-axis = %d ", bmi2_ois_dev.gyr_data.x);
            printf("OIS Gyro y-axis = %d ", bmi2_ois_dev.gyr_data.y);
            printf("OIS Gyro z-axis = %d ", bmi2_ois_dev.gyr_data.z);

            printf("\n");            
        }
    }

    bmi2_coines_deinit();

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for accel.
 */
static int8_t set_accel_config(struct bmi2_dev *bmi2_dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer configuration. */
    struct bmi2_sens_config config;

    /* Configure the type of feature. */
    config.type = BMI2_ACCEL;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(&config, 1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        
        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config.cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* Set the accel configurations. */
        rslt = bmi270_set_sensor_config(&config, 1, bmi2_dev);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

/*!
 *  @brief This internal API is used to set configurations for gyro.
 */
static int8_t set_gyro_config(struct bmi2_dev *dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config;

    /* Configure the type of feature. */
    config.type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(&config, 1, dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* The user can change the following configuration parameters according to their requirement. */
        /* Configure OIS gyro range to either 250dps or 2000 dps */
        config.cfg.gyr.ois_range = BMI2_GYR_RANGE_2000;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config.cfg.gyr.noise_perf = BMI2_PERF_OPT_MODE;

        /* Set the gyro configurations. */
        rslt = bmi270_set_sensor_config(&config, 1, dev);
    }

    return rslt;
}
