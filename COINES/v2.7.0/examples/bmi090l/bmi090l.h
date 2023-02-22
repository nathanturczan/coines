/**
* Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file       bmi090l.h
* @date       2021-06-22
* @version    v1.1.7
*
*/

/*! \file bmi090l.h
 * \brief Sensor Driver for BMI090L family of sensors */

/*!
 * @defgroup bmi090l BMI090L
 */

#ifndef BMI090L_H_
#define BMI090L_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************/
/* Header files */
#include "bmi090l_defs.h"

/*********************************************************************/
/* (Extern) Variable declarations */
/*********************************************************************/
/* Function prototype declarations */
/*********************** BMI090L Accelerometer function prototypes ************************/

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiInit Accel Initialization
 * @brief Initialize the accel sensor and device structure
 */

/*!
 * \ingroup bmi090laApiInit
 * \page bmi090la_api_bmi090la_init bmi090la_init
 * \code
 * int8_t bmi090la_init(struct bmi090l_dev *dev);
 * \endcode
 * @details This API is the entry point for accel sensor.
 *  It performs the selection of I2C/SPI read mechanism according to the
 *  selected interface and reads the chip-id of accel sensor.
 *
 *  @param[in,out] dev  : Structure instance of bmi090l_dev.
 *  @note : Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_init(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laWFconfig Accel Feature config
 * @brief Writes feature configuration to the accel sensor
 */

/*!
 * \ingroup bmi090laWFconfig
 * \page bmi090la_api_bmi090la_write_feature_config bmi090la_write_feature_config
 * \code
 * int8_t bmi090la_write_feature_config(uint8_t reg_addr,
 *                                    const uint16_t *reg_data,
 *                                    uint8_t len,
 *                                    struct bmi090l_dev *dev);
 *
 * \endcode
 * @details This API writes the feature configuration to the accel sensor.
 *
 *  @param[in] reg_addr : Address offset of the feature setting inside the feature conf region.
 *  @param[in] reg_data : Feature settings.
 *  @param[in] len : Number of 16 bit words to be written.
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_write_feature_config(uint8_t reg_addr, const uint16_t *reg_data, uint8_t len, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiRegs Accel Registers
 * @brief Read / Write data from the given register address of accel sensor
 */

/*!
 * \ingroup bmi090laApiRegs
 * \page bmi090la_api_bmi090la_get_regs bmi090la_get_regs
 * \code
 * int8_t bmi090la_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the data from the given register address of accel sensor.
 *
 *  @param[in] reg_addr  : Register address from where the data to be read
 *  @param[out] reg_data : Pointer to data buffer to store the read data.
 *  @param[in] len       : No. of bytes of data to be read.
 *  @param[in] dev       : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiRegs
 * \page bmi090la_api_bmi090la_set_regs bmi090la_set_regs
 * \code
 * int8_t bmi090la_set_regs(uint8_t reg_addr, const uint8_t *reg_data, uint16_t len, struct bmi090l_dev *dev);
 * \endcode
 * @details This API writes the given data to the register address
 *  of accel sensor.
 *
 *  @param[in] reg_addr  : Register address to where the data to be written.
 *  @param[in] reg_data  : Pointer to data buffer which is to be written
 *  in the sensor.
 *  @param[in] len       : No. of bytes of data to write.
 *  @param[in] dev       : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_regs(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiEstatus Accel Error status
 * @brief Read accel error status
 */

/*!
 * \ingroup bmi090laApiEstatus
 * \page bmi090la_api_bmi090la_get_error_status bmi090la_get_error_status
 * \code
 * int8_t bmi090la_get_error_status(struct bmi090l_err_reg *err_reg, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the error status from the accel sensor.
 *
 *  Below table mention the types of error which can occur in the sensor
 *
 *@verbatim
 *************************************************************************
 *        Error           |       Description
 *************************|***********************************************
 *                        |       Fatal Error, chip is not in operational
 *        fatal           |       state (Boot-, power-system).
 *                        |       This flag will be reset only by
 *                        |       power-on-reset or soft-reset.
 *************************|***********************************************
 *                        |       Value        Name       Description
 *        error_code      |       000        no_error     no error
 *                        |       001        accel_err      error in
 *                        |                               ACCEL_CONF
 *************************************************************************
 *@endverbatim
 *
 *  @param[out] err_reg : Pointer to structure variable which stores the
 *  error status read from the sensor.
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_error_status(struct bmi090l_err_reg *err_reg, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiStatus Accel Status
 * @brief Read status of accel sensor
 */

/*!
 * \ingroup bmi090laApiStatus
 * \page bmi090la_api_bmi090la_get_status bmi090la_get_status
 * \code
 * int8_t bmi090la_get_status(uint8_t *status, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the status of the accel sensor.
 *
 *  Below table lists the sensor status flags
 *
 *@verbatim
 *************************************************************************
 *        Status                    |       Description
 ***********************************|*************************************
 *        drdy_accel                | Data ready for Accel.
 *************************************************************************
 *@endverbatim
 *
 *  @param[out] status : Variable used to store the sensor status flags
 *  which is read from the sensor.
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 *
 */
int8_t bmi090la_get_status(uint8_t *status, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiSoftreset Accel Soft Reset
 * @brief Perform accel sensor soft reset
 */

/*!
 * \ingroup bmi090laApiSoftreset
 * \page bmi090la_api_bmi090la_soft_reset bmi090la_soft_reset
 * \code
 * int8_t bmi090la_soft_reset(struct bmi090l_dev *dev);
 * \endcode
 * @details This API resets the accel sensor.
 *
 *  @param[in] dev  : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_soft_reset(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiConfig Accel Configurations
 * @brief Set / Get Accel sensor configurations
 */

/*!
 * \ingroup bmi090laApiConfig
 * \page bmi090la_api_bmi090la_get_meas_conf bmi090la_get_meas_conf
 * \code
 * int8_t bmi090la_get_meas_conf(struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the accel config values ie odr, band width and range from the sensor,
 * store it in the bmi090l_dev structure instance
 * passed by the user.
 *  @param[in,out]  dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_meas_conf(struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiConfig
 * \page bmi090la_api_bmi090la_set_meas_conf bmi090la_set_meas_conf
 * \code
 * int8_t bmi090la_set_meas_conf(struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets the Output data rate, range and bandwidth
 *  of accel sensor.
 *  @param[in] dev  : Structure instance of bmi090l_dev.
 *
 *  @note : The user must select one among the following macros to
 *  select range value for BMI09 accel
 *
 *@verbatim
 *      config                         |   value
 *      -------------------------------|---------------------------
 *      BMI090L_ACCEL_RANGE_3G          |   0x00
 *      BMI090L_ACCEL_RANGE_6G          |   0x01
 *      BMI090L_ACCEL_RANGE_12G         |   0x02
 *      BMI090L_ACCEL_RANGE_24G         |   0x03
 *@endverbatim
 *
 *  @note : Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_meas_conf(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiPowermode Accel Power mode
 * @brief Set / Get Accel power mode
 */

/*!
 * \ingroup bmi090laApiPowermode
 * \page bmi090la_api_bmi090la_get_power_mode bmi090la_get_power_mode
 * \code
 * int8_t bmi090la_get_power_mode(struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the accel power mode from the sensor,
 * store it in the bmi090l_dev structure instance
 * passed by the user.
 *  @param[in,out]  dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_power_mode(struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiPowermode
 * \page bmi090la_api_bmi090la_set_power_mode bmi090la_set_power_mode
 * \code
 * int8_t bmi090la_set_power_mode(struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets the power mode of the accel sensor.
 *
 *  @param[in] dev  : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_power_mode(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiData Accel Data
 * @brief Read accel data from the sensor
 */

/*!
 * \ingroup bmi090laApiData
 * \page bmi090la_api_bmi090la_get_data bmi090la_get_data
 * \code
 * int8_t bmi090la_get_data(struct bmi090l_sensor_data *accel, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the accel data from the sensor,
 *  store it in the bmi090l_sensor_data structure instance
 *  passed by the user.
 *
 *  @param[out] accel  : Structure pointer to store accel data
 *  @param[in]  dev    : Structure instance of bmi090l_dev.
 *
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_data(struct bmi090l_sensor_data *accel, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiIntConfig Accel Interrupt configurations
 * @brief Set accel sensor interrupt configurations
 */

/*!
 * \ingroup bmi090laApiIntConfig
 * \page bmi090la_api_bmi090la_set_int_config bmi090la_set_int_config
 * \code
 * int8_t bmi090la_set_int_config(const struct bmi090l_accel_int_channel_cfg *int_config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API configures the necessary accel interrupt
 *  based on the user settings in the bmi090l_accel_int_channel_cfg
 *  structure instance.
 *
 *  @param[in] int_config  : Structure instance of bmi090l_accel_int_channel_cfg.
 *  @param[in] dev         : Structure instance of bmi090l_dev.
 *  @note : Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_int_config(const struct bmi090l_accel_int_channel_cfg *int_config, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiATemp Accel Temperature
 * @brief Get accel sensor temperature
 */

/*!
 * \ingroup bmi090laApiATemp
 * \page bmi090la_api_bmi090la_get_sensor_temperature bmi090la_get_sensor_temperature
 * \code
 * int8_t bmi090la_get_sensor_temperature(struct bmi090l_dev *dev, int32_t *sensor_temp);
 * \endcode
 * @details This API reads the temperature of the sensor in degree Celcius.
 *
 *  @param[in]  dev             : Structure instance of bmi090l_dev.
 *  @param[out] sensor_temp     : Pointer to store sensor temperature in degree Celcius
 *
 *  @note Temperature data output must be divided by a factor of 1000
 *
 *  Consider sensor_temp = 19520 , Then the actual temperature is 19.520 degree Celsius
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_sensor_temperature(struct bmi090l_dev *dev, int32_t *sensor_temp);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiSensortime Accel sensor time
 * @brief Get sensor time of accel sensor
 */

/*!
 * \ingroup bmi090laApiSensortime
 * \page bmi090la_api_bmi090la_get_sensor_time bmi090la_get_sensor_time
 * \code
 * int8_t bmi090la_get_sensor_time(struct bmi090l_dev *dev, uint32_t *sensor_time);
 * \endcode
 * @details This API reads the sensor time of the accel sensor.
 *
 *  @param[in]  dev             : Structure instance of bmi090l_dev.
 *  @param[out] sensor_time     : Pointer to store sensor time
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_sensor_time(struct bmi090l_dev *dev, uint32_t *sensor_time);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiSelftest Accel Self test
 * @brief Perform self test of accel sensor
 */

/*!
 * \ingroup bmi090laApiSelftest
 * \page bmi090la_api_bmi090la_perform_selftest bmi090la_perform_selftest
 * \code
 * int8_t bmi090la_perform_selftest(struct bmi090l_dev *dev);
 * \endcode
 * @details This API checks whether the self test functionality of the sensor
 *  is working or not
 *
 *  @param[in] dev    : Structure instance of bmi090l_dev
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_perform_selftest(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiAConfig Upload config file
 * @brief Uploads bmi09 config file onto the device
 */

/*!
 * \ingroup bmi090laApiAConfig
 * \page bmi090la_api_bmi090la_apply_config_file bmi090la_apply_config_file
 * \code
 * int8_t bmi090la_apply_config_file(struct bmi090l_dev *dev);
 * \endcode
 * @details This API uploads the bmi09 config file onto the device.
 *
 *  @param[in,out] dev  : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_apply_config_file(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiSyncData Accel Data Synchronization
 * @brief Enable / Disable the data synchronization
 */

/*!
 * \ingroup bmi090laApiSyncData
 * \page bmi090la_api_bmi090la_configure_data_synchronization bmi090la_configure_data_synchronization
 * \code
 * int8_t bmi090la_configure_data_synchronization(struct bmi090l_data_sync_cfg sync_cfg, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is used to enable/disable the data synchronization
 *  feature.
 *
 *  @param[in] sync_cfg : configure sync feature
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_configure_data_synchronization(struct bmi090l_data_sync_cfg sync_cfg, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiAnymotion Accel Anymotion
 * @brief Configure anymotion of sensor
 */

/*!
 * \ingroup bmi090laApiAnymotion
 * \page bmi090la_api_bmi090la_configure_anymotion bmi090la_configure_anymotion
 * \code
 * int8_t bmi090la_configure_anymotion(struct bmi090l_anymotion_cfg anymotion_cfg, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is used to enable/disable and configure the anymotion
 *  feature.
 *
 *  @param[in] anymotion_cfg : configure anymotion feature
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_configure_anymotion(struct bmi090l_anymotion_cfg anymotion_cfg, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiSData Synchronized data read
 * @brief Read synchronized accel and gyro data from the sensor
 */

/*!
 * \ingroup bmi090laApiSData
 * \page bmi090la_api_bmi090la_get_synchronized_data bmi090la_get_synchronized_data
 * \code
 * int8_t bmi090la_get_synchronized_data(struct bmi090l_sensor_data *accel,
 *                                     struct bmi090l_sensor_data *gyro,
 *                                     struct bmi090l_dev *dev);
 *
 * \endcode
 * @details This API reads the synchronized accel & gyro data from the sensor,
 *  store it in the bmi090l_sensor_data structure instance
 *  passed by the user.
 *
 *  @param[out] accel  : Structure pointer to store accel data
 *  @param[out] gyro   : Structure pointer to store gyro  data
 *  @param[in]  dev    : Structure instance of bmi090l_dev.
 *
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_synchronized_data(struct bmi090l_sensor_data *accel,
                                      struct bmi090l_sensor_data *gyro,
                                      struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiInt Synchronized interrupt config
 * @brief Configure synchronized interrupt of the sensor
 */

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_set_data_sync_int_config bmi090la_set_data_sync_int_config
 * \code
 * int8_t bmi090la_set_data_sync_int_config(const struct bmi090l_int_cfg *int_config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API configures the synchronization interrupt
 *  based on the user settings in the bmi090l_int_cfg
 *  structure instance.
 *
 *  @param[in] int_config : Structure instance of accel bmi090l_int_cfg.
 *  @param[in] dev         : Structure instance of bmi090l_dev.
 *  @note : Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_data_sync_int_config(const struct bmi090l_int_cfg *int_config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_set_high_g_config bmi090la_set_high_g_config
 * \code
 * int8_t bmi090la_set_high_g_config(const struct bmi090l_high_g_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets high-g configurations like threshold,
 * hysteresis and duration.
 *
 * @param[in] config : Structure to hold low-g settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_high_g_config(const struct bmi090l_high_g_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_high_g_config bmi090la_get_high_g_config
 * \code
 * int8_t bmi090la_get_high_g_config(struct bmi090l_high_g_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets high-g configurations like threshold,
 * hysteresis and duration.
 *
 * @param[in] config : Structure to hold high-g settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_high_g_config(struct bmi090l_high_g_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_set_low_g_config bmi090la_set_low_g_config
 * \code
 * int8_t bmi090la_set_low_g_config(const struct bmi090l_low_g_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets low-g configurations like threshold,
 * hysteresis and duration.
 *
 * @param[in] config : Structure to hold low-g settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_low_g_config(const struct bmi090l_low_g_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_low_g_config bmi090la_get_low_g_config
 * \code
 * int8_t bmi090la_get_low_g_config(struct bmi090l_low_g_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets low-g configurations like threshold,
 * hysteresis and duration.
 *
 * @param[in] config : Structure to hold low-g settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_low_g_config(struct bmi090l_low_g_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_set_no_motion_config bmi090la_set_no_motion_config
 * \code
 * int8_t bmi090la_set_no_motion_config(const struct bmi090l_no_motion_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets and enables no-motion parameters like threshold,
 * duration,etc .,
 *
 * @param[in] config : Structure to hold no-motion settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_no_motion_config(const struct bmi090l_no_motion_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_no_motion_config bmi090la_get_no_motion_config
 * \code
 * int8_t bmi090la_get_no_motion_config(struct bmi090l_no_motion_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets no-motion parameters like threshold,
 * duration,etc .,
 *
 * @param[in] config : Structure to hold no-motion settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_no_motion_config(struct bmi090l_no_motion_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_set_orient_config bmi090la_set_orient_config
 * \code
 * int8_t bmi090la_set_orient_config(const struct bmi090l_orient_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets orientation parameters like mode, hysteresis, theta, etc.,
 *
 * @param[in] config : Structure to hold orientation feature settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_orient_config(const struct bmi090l_orient_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_orient_config bmi090la_get_orient_config
 * \code
 * int8_t bmi090la_get_orient_config(struct bmi090l_orient_cfg *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets orientation parameters like mode, hysteresis, theta, etc.,
 *
 * @param[in] config : Structure to hold orientation feature settings
 * @param[in] dev    : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_orient_config(struct bmi090l_orient_cfg *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_orient_output bmi090la_get_orient_output
 * \code
 * int8_t bmi090la_get_orient_output(struct bmi090l_orient_out *orient_out, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the output values of orientation: portrait-
 * landscape and face up-down.
 *
 * @param[out] orient_out      : Structure instance of bmi090l_orient_out
 * @param[in]  dev             : Structure instance of bmi090l_dev.
 *
 *
 * portrait   |
 * landscape  |  Output
 * -----------|------------
 * 0x00       |  PORTRAIT_UPRIGHT
 * 0x01       |  LANDSCAPE_LEFT
 * 0x02       |  PORTRAIT_UPSIDE_DOWN
 * 0x03       |  LANDSCAPE_RIGHT
 *
 * Face       |
 * up-down    |  Output
 * -----------|------------
 * 0x00       |  FACE_UP
 * 0x01       |  FACE_DOWN
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_orient_output(struct bmi090l_orient_out *orient_out, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_high_g_output bmi090la_get_high_g_output
 * \code
 * int8_t bmi090la_get_high_g_output(struct bmi090l_high_g_out *high_g_out, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the output values of high_g: Axis and Direction
 *
 * @param[out] high_g_out      : Structure instance of bmi090l_high_g_out
 * @param[in]  dev             : Structure instance of bmi090l_dev.
 *
 * Direction  |  Output
 * -----------|-----------------
 * 0x00       |  Positive axis
 * 0x01       |  Negative axis
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_high_g_output(struct bmi090l_high_g_out *high_g_out, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_data_int_status bmi090la_get_data_int_status
 * \code
 * int8_t bmi090la_get_data_int_status(uint8_t *int_status, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is to get accel data ready interrupt status
 *
 * @param[out] int_status      : Variable to store interrupt status
 * @param[in]  dev             : Structure instance of bmi090l_dev
 *
 *@verbatim
 *-----------------------------------------
 *   int_status    |     Interrupt
 *-----------------------------------------
 *      0x01       |    Fifo full
 *      0x02       |    Fifo watermark
 *      0x08       |    Accel data ready
 *------------------------------------------
 *@endverbatim
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_data_int_status(uint8_t *int_status, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090la_get_feat_int_status bmi090la_get_feat_int_status
 * \code
 * int8_t bmi090la_get_feat_int_status(uint8_t *int_status, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is to get accel feature interrupt status
 *
 * @param[out] int_status      : Variable to store interrupt status
 * @param[in]  dev             : Structure instance of bmi090l_dev
 *
 *@verbatim
 *-----------------------------------------
 *   int_status    |     Interrupt
 *-----------------------------------------
 *      0x01       |    Data sync
 *      0x02       |    Any-Motion
 *      0x04       |    High-g
 *      0x08       |     Low-g
 *      0x16       |     Orient
 *      0x32       |     No-motion
 *------------------------------------------
 *@endverbatim
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_feat_int_status(uint8_t *int_status, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiRemap Axis Remap
 * @brief Functions of axis remapping of bmi09 sensor
 */

/*!
 * \ingroup bmi090laApiRemap
 * @brief Set / Get x, y and z axis re-mapping in the sensor
 * \page bmi090la_api_bmi090la_set_remap_axes bmi090la_set_remap_axes
 * \code
 * int8_t bmi090la_set_remap_axes(const struct bmi090l_remap *remapped_axis, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets the re-mapped x, y and z axes to the sensor and
 * updates them in the device structure.
 *
 * @param[in] remapped_axis    : Pointer to store axes re-mapping data.
 * @param[in] dev              : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status.
 *
 * @return 0 -> Success
 * @return < 0  -> Fail
 *
 */
int8_t bmi090la_set_remap_axes(const struct bmi090l_remap *remapped_axis, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiRemap
 * \page bmi090l_api_bmi090la_get_remap_axes bmi090la_get_remap_axes
 * \code
 * int8_t bmi090la_get_remap_axes(struct bmi090l_remap *remapped_axis, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the re-mapped x, y and z axes from the sensor and
 * updates the values in the device structure.
 *
 * @param[out] remapped_axis   : Structure instance of bmi090l_remap
 * @param[in] dev              : Structure instance of bmi090l_dev
 *
 * @return Result of API execution status.
 *
 * @return 0 -> Success
 * @return < 0 -> Fail
 *
 */
int8_t bmi090la_get_remap_axes(struct bmi090l_remap *remapped_axis, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiFIFO FIFO Operations
 * @brief Various FIFO operations on bmi09 sensor
 */

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_set_fifo_config bmi090la_set_fifo_config
 * \code
 * int8_t bmi090la_set_fifo_config(const struct bmi090l_accel_fifo_config *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets the FIFO configuration in the sensor.
 *
 * @param[in] config        : Structure instance of FIFO configurations.
 * @param[in] dev           : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_fifo_config(const struct bmi090l_accel_fifo_config *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_get_fifo_config bmi090la_get_fifo_config
 * \code
 * int8_t bmi090la_get_fifo_config(struct bmi090l_accel_fifo_config *config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the FIFO configuration from the sensor.
 *
 * @param[out] config   : Structure instance to get FIFO configuration value.
 * @param[in]  dev      : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_fifo_config(struct bmi090l_accel_fifo_config *config, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_read_fifo_data bmi090la_read_fifo_data
 * \code
 * int8_t bmi090la_read_fifo_data(struct bmi090l_fifo_frame *fifo, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads FIFO data.
 *
 * @param[in, out] fifo     : Structure instance of bmi090l_fifo_frame.
 * @param[in]      dev      : Structure instance of bmi090l_dev.
 *
 * @note APS has to be disabled before calling this function.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_read_fifo_data(struct bmi090l_fifo_frame *fifo, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_get_fifo_length bmi090la_get_fifo_length
 * \code
 * int8_t bmi090la_get_fifo_length(uint16_t *fifo_length, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the length of FIFO data available in the sensor in
 * bytes.
 *
 * @param[out] fifo_length  : Pointer variable to store the value of FIFO byte
 *                            counter.
 * @param[in]  dev          : Structure instance of bmi090l_dev.
 *
 * @note The byte counter is updated each time a complete frame is read or
 * written.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_fifo_length(uint16_t *fifo_length, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_get_fifo_wm bmi090la_get_fifo_wm
 * \code
 * int8_t bmi090la_get_fifo_wm(uint16_t *wm, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the FIFO water mark level which is set in the sensor.
 *
 * @param[out] wm        : Pointer variable to store FIFO water-mark level.
 * @param[in]  dev            : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_fifo_wm(uint16_t *wm, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_set_fifo_wm bmi090la_set_fifo_wm
 * \code
 * int8_t bmi090la_set_fifo_wm(uint16_t wm, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets the FIFO water mark level which is set in the sensor.
 *
 * @param[out] wm        : Pointer variable to store FIFO water-mark level.
 * @param[in]  dev            : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_fifo_wm(uint16_t wm, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_extract_accel bmi090la_extract_accel
 * \code
 * int8_t bmi090la_extract_accel(struct bmi090l_sensor_data *accel_data,
 *                             uint16_t *accel_length,
 *                             struct bmi090l_fifo_frame *fifo,
 *                             const struct bmi090l_dev *dev);
 *
 * \endcode
 * @details This API parses and extracts the accelerometer frames from FIFO data read by
 * the "bmi090l_read_fifo_data" API and stores it in the "accel_data" structure
 * instance.
 *
 * @param[out]    accel_data   : Structure instance of bmi090l_sensor_data
 *                               where the parsed data bytes are stored.
 * @param[in,out] accel_length : Number of accelerometer frames.
 * @param[in,out] fifo         : Structure instance of bmi090l_fifo_frame.
 * @param[in]     dev          : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_extract_accel(struct bmi090l_sensor_data *accel_data,
                              uint16_t *accel_length,
                              struct bmi090l_fifo_frame *fifo,
                              const struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_get_fifo_down_sample bmi090la_get_fifo_down_sample
 * \code
 * int8_t bmi090la_get_fifo_down_sample(uint8_t *fifo_downs, struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the down sampling rate, configured for FIFO
 * accelerometer.
 *
 * @param[out] fifo_downs : Pointer variable to store the down sampling rate
 * @param[in]  dev            : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_get_fifo_down_sample(uint8_t *fifo_downs, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090laApiFIFO
 * \page bmi090la_api_bmi090la_set_fifo_down_sample bmi090la_set_fifo_down_sample
 * \code
 * int8_t bmi090la_set_fifo_down_sample(uint8_t fifo_downs, struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets the down sampling rate for FIFO accelerometer FIFO data.
 *
 * @param[in] fifo_downs : Variable to set the down sampling rate.
 * @param[in] dev            : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090la_set_fifo_down_sample(uint8_t fifo_downs, struct bmi090l_dev *dev);

/*********************** BMI09 Gyroscope function prototypes ****************************/

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiInit Gyro Initialization
 * @brief Initialize the accel sensor and device structure
 */

/*!
 * \ingroup bmi090lgApiInit
 * \page bmi090lg_api_bmi090lg_init bmi090lg_init
 * \code
 * int8_t bmi090lg_init(struct bmi090l_dev *dev);
 * \endcode
 * @details This API is the entry point for gyro sensor.
 *  It performs the selection of I2C/SPI read mechanism according to the
 *  selected interface and reads the chip-id of gyro sensor.
 *
 *  @param[in,out] dev : Structure instance of bmi090l_dev.
 *  @note : Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_init(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiRegs Gyro Registers
 * @brief Read / Write data from the given register address of gyro sensor
 */

/*!
 * \ingroup bmi090lgApiRegs
 * \page bmi090lg_api_bmi090lg_get_regs bmi090lg_get_regs
 * \code
 * int8_t bmi090lg_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the data from the given register address of gyro sensor.
 *
 *  @param[in] reg_addr  : Register address from where the data to be read
 *  @param[out] reg_data : Pointer to data buffer to store the read data.
 *  @param[in] len       : No. of bytes of data to be read.
 *  @param[in] dev       : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090lgApiRegs
 * \page bmi090lg_api_bmi090lg_set_regs bmi090lg_set_regs
 * \code
 * int8_t bmi090lg_set_regs(uint8_t reg_addr,const uint8_t *reg_data, uint32_t len, struct bmi090l_dev *dev);
 * \endcode
 * @details This API writes the given data to the register address
 *  of gyro sensor.
 *
 *  @param[in] reg_addr  : Register address to where the data to be written.
 *  @param[in] reg_data  : Pointer to data buffer which is to be written
 *  in the sensor.
 *  @param[in] len       : No. of bytes of data to write.
 *  @param[in] dev       : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_set_regs(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiSoftreset Gyro soft reset
 * @brief Perform soft reset of gyro sensor
 */

/*!
 * \ingroup bmi090lgApiSoftreset
 * \page bmi090lg_api_bmi090lg_soft_reset bmi090lg_soft_reset
 * \code
 * int8_t bmi090lg_soft_reset(struct bmi090l_dev *dev);
 * \endcode
 * @details This API resets the gyro sensor.
 *
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_soft_reset(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiConfig Gyro Configurations
 * @brief Set / Get gyro sensor configurations
 */

/*!
 * \ingroup bmi090lgApiConfig
 * \page bmi090lg_api_bmi090lg_get_meas_conf bmi090lg_get_meas_conf
 * \code
 * int8_t bmi090lg_get_meas_conf(struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the gyro odr and range from the sensor,
 *  store it in the bmi090l_dev structure instance
 *  passed by the user.
 *
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @note : band width also updated, which is same as odr
 *          Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_get_meas_conf(struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090lgApiConfig
 * \page bmi090lg_api_bmi090lg_set_meas_conf bmi090lg_set_meas_conf
 * \code
 * int8_t bmi090lg_set_meas_conf(struct bmi090l_dev *dev);
 * \endcode
 * @details This API sets the output data rate, range and bandwidth
 *  of gyro sensor.
 *
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *  @note : Refer user guide for detailed info.
 *
 *  @note : No need to give the band width parameter,
 *          odr will update the band width.
 *          Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_set_meas_conf(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiPowermode Gyro Power mode
 * @brief Set / Get Gyro power mode
 */

/*!
 * \ingroup bmi090lgApiPowermode
 * \page bmi090lg_api_bmi090la_get_power_mode bmi090lg_get_power_mode
 * \code
 * int8_t bmi090lg_get_power_mode(struct bmi090l_dev *dev);
 * \endcode
 * @details This API gets the power mode of the gyro sensor and store it
 *  inside the instance of bmi090l_dev passed by the user.
 *
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_get_power_mode(struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090lgApiPowermode
 * \page bmi090lg_api_bmi090la_set_power_mode bmi090lg_set_power_mode
 * \code
 * int8_t bmi090lg_set_power_mode(struct bmi090l_dev *dev);
 * \endcode
 * @details  This API sets the power mode of the gyro sensor.
 *
 *  @param[in] dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_set_power_mode(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiData Gyro Data
 * @brief Read gyro data from the sensor
 */

/*!
 * \ingroup bmi090lgApiData
 * \page bmi090lg_api_bmi090lg_get_data bmi090lg_get_data
 * \code
 * int8_t bmi090lg_get_data(struct bmi090l_sensor_data *gyro, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads the gyro data from the sensor,
 *  store it in the bmi090l_sensor_data structure instance
 *  passed by the user.
 *
 *  @param[out] gyro   : Structure pointer to store gyro data
 *  @param[in] dev     : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_get_data(struct bmi090l_sensor_data *gyro, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiIntConfig Gyro Interrupt configurations
 * @brief Set gyro sensor interrupt configurations
 */

/*!
 * \ingroup bmi090lgApiIntConfig
 * \page bmi090lg_api_bmi090lg_set_int_config bmi090lg_set_int_config
 * \code
 * int8_t bmi090lg_set_int_config(const struct bmi090l_gyro_int_channel_cfg *int_config, struct bmi090l_dev *dev);
 * \endcode
 * @details This API configures the necessary gyro interrupt
 *  based on the user settings in the bmi090l_gyro_int_channel_cfg
 *  structure instance.
 *
 *  @param[in] int_config  : Structure instance of bmi090l_gyro_int_channel_cfg.
 *  @param[in] dev         : Structure instance of bmi090l_dev.
 *  @note : Refer user guide for detailed info.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_set_int_config(const struct bmi090l_gyro_int_channel_cfg *int_config, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiSelftest Gyro Self test
 * @brief Perform self test of gyro sensor
 */

/*!
 * \ingroup bmi090lgApiSelftest
 * \page bmi090lg_api_bmi090lg_perform_selftest bmi090lg_perform_selftest
 * \code
 * int8_t bmi090lg_perform_selftest(struct bmi090l_dev *dev);
 * \endcode
 * @details This API checks whether the self test functionality of the
 *  gyro sensor is working or not
 *
 *  @param[in]  dev : Structure instance of bmi090l_dev.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_perform_selftest(struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgInt Gyro Interrupt
 * @brief Get gyro data ready interrupt status
 */

/*!
 * \ingroup bmi090laApiInt
 * \page bmi090la_api_bmi090lg_get_data_int_status bmi090lg_get_data_int_status
 * \code
 * int8_t bmi090lg_get_data_int_status(uint8_t *int_status, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is to get gyro data ready interrupt status
 *
 * @param[out] int_status      : Variable to store interrupt status
 * @param[in]  dev             : Structure instance of bmi090l_dev
 *
 *@verbatim
 *-----------------------------------------
 *   int_status    |     Interrupt
 *-----------------------------------------
 *      0x08       |    Gyro data ready
 *------------------------------------------
 *@endverbatim
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success
 *  @retval < 0 -> Fail
 */
int8_t bmi090lg_get_data_int_status(uint8_t *int_status, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090lgApiFIFO FIFO
 * @brief Access and extract FIFO gyro data
 */

/*!
 * \ingroup bmi090lgApiFIFO
 * \page bmi090lg_api_bmi090lg_get_fifo_config bmi090lg_get_fifo_config
 * \code
 * int8_t bmi090lg_get_fifo_config(struct bmi090l_gyr_fifo_config *fifo_conf, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is used to get the fifo configurations like fifo mode, fifo data select, etc
 *
 * @param[in] fifo_conf  : Structure pointer to fifo configurations
 * @param[in] dev        : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
int8_t bmi090lg_get_fifo_config(struct bmi090l_gyr_fifo_config *fifo_conf, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090lgApiFIFO
 * \page bmi090lg_api_bmi090lg_set_fifo_config bmi090lg_set_fifo_config
 * \code
 * int8_t bmi090lg_set_fifo_config(const struct bmi090l_gyr_fifo_config *fifo_conf, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is used to set the fifo configurations like fifo mode, fifo data select, etc
 *
 * @param[in] fifo_conf  : Structure pointer to fifo configurations
 * @param[in] dev        : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
int8_t bmi090lg_set_fifo_config(const struct bmi090l_gyr_fifo_config *fifo_conf, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090lgApiFIFO
 * \page bmi090lg_api_bmi090lg_get_fifo_length bmi090lg_get_fifo_length
 * \code
 * int8_t bmi090lg_get_fifo_length(const struct bmi090l_gyr_fifo_config *fifo_config, struct bmi090l_fifo_frame *fifo);
 * \endcode
 * @details This API is used to get fifo length
 *
 * @param[in] fifo_config  : Structure instance of bmi090l_gyr_fifo_config
 * @param[in] fifo         : Structure instance of bmi090l_fifo_frame.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
int8_t bmi090lg_get_fifo_length(const struct bmi090l_gyr_fifo_config *fifo_config, struct bmi090l_fifo_frame *fifo);

/*!
 * \ingroup bmi090lgApiFIFO
 * \page bmi090lg_api_bmi090lg_read_fifo_data bmi090lg_read_fifo_data
 * \code
 * int8_t bmi090lg_read_fifo_data(const struct bmi090l_fifo_frame *fifo, struct bmi090l_dev *dev);
 * \endcode
 * @details This API reads FIFO data.
 *
 * @param[in, out] fifo       : Structure instance of bmi090l_fifo_frame
 * @param[in]      dev        : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
int8_t bmi090lg_read_fifo_data(const struct bmi090l_fifo_frame *fifo, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090lgApiFIFO
 * \page bmi090lg_api_bmi090lg_extract_gyro bmi090lg_extract_gyro
 * \code
 * void bmi090lg_extract_gyro(struct bmi090l_sensor_data *gyro_data,
 *                            const uint16_t *gyro_length,
 *                            const struct bmi090l_gyr_fifo_config *fifo_conf,
 *                            const struct bmi090l_fifo_frame *fifo);
 * \endcode
 * @details This API parses and extracts the gyroscope frames from FIFO data read by the
 * "bmi090lg_read_fifo_data" API and stores it in the "gyro_data"
 * structure instance.
 *
 * @param[out]    gyro_data    : Structure instance of bmi090l_sensor_data
 *                               where the parsed data bytes are stored.
 * @param[in,out] gyro_length  : Number of gyroscope frames.
 * @param[in,out] fifo_conf    : Structure instance of bmi090l_gyr_fifo_config
 * @param[in,out] fifo         : Structure instance of bmi090l_fifo_frame
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
void bmi090lg_extract_gyro(struct bmi090l_sensor_data *gyro_data,
                           const uint16_t *gyro_length,
                           const struct bmi090l_gyr_fifo_config *fifo_conf,
                           const struct bmi090l_fifo_frame *fifo);

/*!
 * \ingroup bmi090lgApiFIFO
 * \page bmi090lg_api_bmi090lg_get_fifo_overrun bmi090lg_get_fifo_overrun
 * \code
 * int8_t bmi090lg_get_fifo_overrun(uint8_t *fifo_overrun, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is used to get the fifo over run
 *  in the register 0x0E bit 7
 *
 * @param[in,out] fifo_overrun  : The value of fifo over run
 * @param[in]     dev           : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
int8_t bmi090lg_get_fifo_overrun(uint8_t *fifo_overrun, struct bmi090l_dev *dev);

/*!
 * \ingroup bmi090lgApiFIFO
 * \page bmi090lg_api_bmi090lg_enable_watermark bmi090lg_enable_watermark
 * \code
 * int8_t bmi090lg_enable_watermark(uint8_t enable, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is used to set fifo watermark enable/disable
 *  in the register 0x1E bit 7
 *
 * @param[in,out] enable        : The value of fifo watermark enable/disable
 * @param[in]     dev           : Structure instance of bmi090l_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
int8_t bmi090lg_enable_watermark(uint8_t enable, struct bmi090l_dev *dev);

/**
 * \ingroup bmi090l
 * \defgroup bmi090laApiVersion Major and Minor Revision
 * @brief Reads major and minor revision of sensor
 */

/*!
 * \ingroup bmi090laApiVersion
 * \page bmi090l_api_bmi090la_get_version_config bmi090la_get_version_config
 * \code
 *int8_t bmi090la_get_version_config(uint16_t *config_major, uint16_t *config_minor, struct bmi090l_dev *dev);
 * \endcode
 * @details This API is used to get the config file major and minor information.
 *
 * @param[in] dev              : Structure instance of bmi090l_dev.
 * @param[out] config_major    : Pointer to data buffer to store the config major.
 * @param[out] config_minor    : Pointer to data buffer to store the config minor.
 *
 *  @return Result of API execution status
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
int8_t bmi090la_get_version_config(uint16_t *config_major, uint16_t *config_minor, struct bmi090l_dev *dev);

#ifdef __cplusplus
}
#endif

#endif /* BMI090L_H_ */

/** @}*/
