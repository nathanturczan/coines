/**
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
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
 * @file    bhy2cli_callbacks.c
 * @brief   Source file for the command line utility callbacks
 *
 */

#define BHY2CLI_VER_MAJOR       "0"
#define BHY2CLI_VER_MINOR       "4"
#define BHY2CLI_VER_BUGFIX      "6"

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__  1
#else
#define _POSIX_C_SOURCE         200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#if defined(PC)
#include <dirent.h>
#endif

#include "coines.h"
#include "bhy2cli_callbacks.h"
#include "common_callbacks.h"
#include "common.h"
#include "parse.h"
#include "verbose.h"

#define BHY2_ASSERT(x)             assert_rslt = x; if (assert_rslt) check_bhy2_api(__LINE__, __FUNCTION__, assert_rslt)

#if defined(PC)
#define BHY2_RD_WR_LEN             44
#else
#define BHY2_RD_WR_LEN             256
#endif
#define BHY2CLI_MAX_STRING_LENGTH  UINT16_C(32)

#define DATA(format, ...)          verbose("[D]"format,##__VA_ARGS__)
#define PRINT_D(format, ...)       verbose(format,##__VA_ARGS__)

/* Contains all parameters of the of custom virtual sensors
 * required for parsing */
typedef struct BHY2_PACKED custom_driver_information
{
    char sensor_name[BHY2CLI_MAX_STRING_LENGTH];
    uint16_t sensor_payload : 7;
    uint16_t sensor_id : 8;
    uint16_t is_registered : 1;
    char output_formats[BHY2CLI_MAX_STRING_LENGTH];
} custom_driver_information_t;

/* Contains information about klio capabilities and current state, as well as runtime configuration */
static struct
{
    uint8_t max_patterns;
    uint8_t max_pattern_blob_size;
    uint8_t auto_load_pattern_write_index;
    uint8_t auto_load_pattern;
}
klio_vars;
static int8_t assert_rslt;
static uint8_t fifo_buffer[2048];
static bool sensors_active[256] = { false };
static bool first_run = true;

/* Added to report the final swim data to mobile APP when swim is disabled */
static struct bhy2_swim_algo_output swim_data;

/* Static global table that contains the payloads of present custom virtual sensors, derived by a parameter read */
static custom_driver_information_t custom_driver_information[(BHY2_SENSOR_ID_CUSTOM_END - BHY2_SENSOR_ID_CUSTOM_START) +
                                                             1];
static bool klio_enabled = false;

static cli_callback_table_t bhy2_cli_callbacks[] = {
    { 0, "", 0, NULL, NULL }, /* Empty characters creates a new line */
    { 'h', "help", 0, help_callback, help_help }, /* Print all available commands */
    { 0, "version", 0, version_callback, version_help }, /* Prints the HW, SW versions and build date*/
    { 'v', "verb", 1, verbose_callback, verb_help }, /* Change verbose of received outputs */
    { 'b', "ramb", 1, ramb_callback, ramb_help }, /* Reset, Load firmware to RAM and boot */
    { 'd', "flb", 1, flb_callback, flb_help }, /* Reset, Load firmware to Flash and boot */
    { 'n', "reset", 0, reset_callback, reset_help }, /* Trigger a soft reset to the sensor */
    { 'a', "addse", 1, addse_callback, addse_help }, /* Add a custom sensor */
    { 'g', "boot", 1, boot_callback, boot_help }, /* Boot from RAM or Flash */
    { 'c', "actse", 1, actse_callback, actse_help }, /* Activate/De-activate a sensor */
    { 'e', "erase", 0, erase_callback, erase_help }, /* Erase the external Flash */
    { 0, "efd", 0, efd_callback, efd_help }, /* Erase the Flash descriptor */
    { 'r', "rd", 1, rd_callback, rd_help }, /* Read registers */
    { 'u', "ram", 1, ram_callback, ram_help }, /* Upload firmware to RAM */
    { 'f', "fl", 1, fl_callback, fl_help }, /* Upload firmware to Flash */
    { 'w', "wr", 1, wr_callback, wr_help }, /* Write registers */
    { 'i', "info", 0, info_callback, info_help }, /* Get information of the state of the device and loaded sensors */
    { 's', "rdp", 1, rdp_callback, rdp_help }, /* Read a parameter */
    { 't', "wrp", 1, wrp_callback, wrp_help }, /* Write a parameter */
    { 0, "logse", 1, logse_callback, logse_help }, /* Log sensor data in binary */
    { 0, "attlog", 1, attlog_callback, attlog_help }, /* Attach a log file for logging */
    { 0, "detlog", 1, detlog_callback, detlog_help }, /* Detach a log file for logging */
    { 0, "kstatus", 0, kstatus_callback, kstatus_help }, /* Get Klio status */
    { 0, "ksetstate", 4, ksetstate_callback, ksetstate_help }, /* Set Klio state */
    { 0, "kgetstate", 0, kgetstate_callback, kgetstate_help }, /* Get Klio state */
    { 0, "kldpatt", 2, kldpatt_callback, kldpatt_help }, /* Load Klio pattern */
    { 0, "kenpatt", 1, kenpatt_callback, kenpatt_help }, /* Enable Klio pattern */
    { 0, "kdispatt", 1, kdispatt_callback, kdispatt_help }, /* Disable Klio pattern */
    { 0, "kdisapatt", 1, kdisapatt_callback, kdisapatt_help }, /* Disable Klio adaptive pattern */
    { 0, "kswpatt", 1, kswpatt_callback, kswpatt_help }, /* Switch Klio pattern between left/right hand */
    { 0, "kautldpatt", 2, kautldpatt_callback, kautldpatt_help }, /* Auto-load Klio patterns */
    { 0, "kgetparam", 1, kgetparam_callback, kgetparam_help }, /* Get Klio parameters */
    { 0, "ksetparam", 2, ksetparam_callback, ksetparam_help }, /* Set Klio parameters */
    { 0, "ksimscore", 2, ksimscore_callback, ksimscore_help }, /* Get Klio Similarity score */
    { 0, "kmsimscore", 2, kmsimscore_callback, kmsimscore_help }, /* Get Multiple Klio Similarity score */
    { 0, "pfullreset", 0, pfullreset_callback, pfullreset_help }, /* Trigger a PDR reset */
    { 0, "ptrackreset", 0, ptrackreset_callback, ptrackreset_help }, /* Trigger a track reset for the PDR */
    { 0, "prefheaddel", 1, prefheaddel_callback, prefheaddel_help }, /* Set the PDR's reference heading delta */
    { 0, "pstepinfo", 2, pstepinfo_callback, pstepinfo_help }, /* Set the PDR's Step length */
    { 0, "psethand", 1, psethand_callback, psethand_help }, /* Set the hand for the PDR */
    { 0, "pdrver", 0, pdrver_callback, pdrver_help }, /* Get the PDR driver version*/
    { 0, "palver", 0, palver_callback, palver_help }, /* Get the PDR algo version */
    { 0, "pvariant", 0, pvariant_callback, pvariant_help }, /* Get the PDR variant */
    { 0, "pdevpos", 0, pdevpos_callback, pdevpos_help }, /* Get the PDR device position */
    { 0, "swim", 3, swim_callback, swim_help }, /* Configure the Swim recognition */
    { 0, "swimver", 0, swimver_callback, swimver_help }, /* Get the Swim Version */
    { 0, "swimgetfreq", 0, swimgetfreq_callback, swimgetfreq_help }, /* Get the Swim frequency */
    { 0, "swimsetfreq", 2, swimsetfreq_callback, swimsetfreq_help }, /* Set the Swim frequency */
    { 0, "swimgetaxes", 0, swimgetaxes_callback, swimgetaxes_help }, /* Get the Swim orientation sensor */
    { 0, "swimsetaxes", 1, swimsetaxes_callback, swimsetaxes_help }, /* Set the Swim orientation sensor */
#ifndef PC
    { 0, "echo", 1, echo_callback, echo_help }, /* Toggle the echo setting */
    { 0, "heart", 1, heartbeat_callback, heartbeat_help }, /* Toggle the heartbeat message setting */
    { 0, "mklog", 1, mklog_callback, mklog_help }, /* Make a log file */
    { 0, "rm", 1, rm_callback, rm_help }, /* Remove a file */
    { 0, "ls", 0, ls_callback, ls_help }, /* List files */
    { 0, "wrlog", 2, wrlog_callback, wrlog_help }, /* Write content to a log file */
    { 0, "slabel", 1, slabel_callback, slabel_help }, /* Write a binary label into the log file */
    { 0, "cls", 0, cls_callback, cls_help }, /* Clear screen */
    { 0, "strbuf", 1, streambuff_callback, streambuff_help }, /* Enable streaming buffer */
#endif
};

static void check_bhy2_api(unsigned int line, const char *func, int8_t val);
static void reset_hub(struct bhy2_dev *bhy2);
static bool upload_to_ram(const char *filepath, struct bhy2_dev *bhy2);
static void print_boot_status(uint8_t boot_status);
static void boot_ram(struct bhy2_dev *bhy2);
static void show_info(struct bhy2_dev *bhy2);
static void boot_flash(struct bhy2_dev *bhy2);
static bool upload_to_flash(const char *filepath, struct bhy2_dev *bhy2);
static void wr_regs(const char *payload, struct bhy2_dev *bhy2);
static void rd_regs(const char *payload, struct bhy2_dev *bhy2);
static void rd_param(const char *payload, struct bhy2_dev *bhy2);
static void wr_param(const char *payload, struct bhy2_dev *bhy2);
static void erase_flash(uint32_t end_addr, struct bhy2_dev *bhy2);
static void activate_sensor(const char *sensor_parameters, uint8_t parse_flag, struct bhy2_cli_ref *ref);
static void parse_custom_sensor_default(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
static void parse_custom_sensor(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
static void add_sensor(const char *payload, struct bhy2_cli_ref *cli_ref);
static void klio_enable(struct bhy2_dev *bhy2);
static void klio_status(struct bhy2_dev *bhy2);
static void klio_set_state(const char *arg1, const char *arg2, const char *arg3, const char *arg4,
                           struct bhy2_dev *bhy2);
static void klio_get_state(struct bhy2_dev *bhy2);
static void klio_load_pattern(const char *arg1, const char *arg2, struct bhy2_dev *bhy2);
static void klio_get_parameter(const uint8_t *arg, struct bhy2_dev *bhy2);
static void klio_set_parameter(const char *arg1, char *arg2, struct bhy2_dev *bhy2);
static void klio_similarity_score(const uint8_t *arg1, const uint8_t *arg2, struct bhy2_dev *bhy2);
static void klio_similarity_score_multiple(const char *arg1, const char *arg2, struct bhy2_dev *bhy2);
static void klio_pattern_state_operation(const uint8_t enable, const char *arg1, struct bhy2_dev *bhy2);
void parse_klio(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
void parse_klio_log(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
void parse_pdr(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
void parse_swim(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
void parse_acc_gyro(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
void parse_air_quality(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
void parse_hmc(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
void parse_oc(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
static void log_data(uint8_t sid, uint64_t tns, uint8_t event_size, uint8_t *event_payload, struct logbin_dev *logdev);
static void write_meta_info(struct logbin_dev *log, struct bhy2_dev *bhy2);

cli_callback_table_t *bhy2_get_cli_callbacks(void)
{
    return bhy2_cli_callbacks;
}

uint8_t bhy2_get_n_cli_callbacks(void)
{
    return sizeof(bhy2_cli_callbacks) / sizeof(cli_callback_table_t);
}

void bhy2_callbacks_init(struct bhy2_cli_ref *cli_ref)
{
    struct bhy2_dev *bhy2 = &cli_ref->bhy2;
    uint8_t expected_data;

    for (uint8_t i = BHY2_SENSOR_ID_CUSTOM_START;
         i <= BHY2_SENSOR_ID_CUSTOM_END; i++)
    {
        custom_driver_information[i - BHY2_SENSOR_ID_CUSTOM_START].is_registered = 0;
        strcpy(custom_driver_information[i - BHY2_SENSOR_ID_CUSTOM_START].sensor_name, "Undefined custom sensor");
    }

    /* Print Copyright build date */
    PRINT("Copyright (c) 2021 Bosch Sensortec GmbH\r\n");
    PRINT("Version %s.%s.%s Build date: " __DATE__ "\r\n", BHY2CLI_VER_MAJOR, BHY2CLI_VER_MINOR, BHY2CLI_VER_BUGFIX);
#ifdef BHY2_USE_I2C
    BHY2_ASSERT(bhy2_init(BHY2_I2C_INTERFACE, bhy2_i2c_read, bhy2_i2c_write, bhy2_delay_us, BHY2_RD_WR_LEN, NULL,
                          bhy2));
#else
    BHY2_ASSERT(bhy2_init(BHY2_SPI_INTERFACE, bhy2_spi_read, bhy2_spi_write, bhy2_delay_us, BHY2_RD_WR_LEN, NULL,
                          bhy2));
#endif

    /* Install virtual sensor callbacks */
    bhy2_install_callbacks(&cli_ref->bhy2, &cli_ref->parse_table);

    coines_delay_msec(100); /* Wait for flash firmware to load if applicable */

    uint8_t product_id;

    BHY2_ASSERT(bhy2_get_product_id(&product_id, bhy2));
    if (product_id == BHY2_PRODUCT_ID)
    {
        uint8_t feat_status;
        BHY2_ASSERT(bhy2_get_feature_status(&feat_status, bhy2));
        INFO("Device found\r\n");
        if (feat_status & BHY2_FEAT_STATUS_OPEN_RTOS_MSK)
        {
            INFO("RTOS based firmware running\r\n");

            BHY2_ASSERT(bhy2_update_virtual_sensor_list(bhy2));
        }
    }
    else
    {
        ERROR("Device not found, Check connections and power. Product ID read 0x%x\r\n", product_id);
#ifdef PC
        exit(1);
#endif
    }

    /* Config status channel */
    BHY2_ASSERT(bhy2_set_host_intf_ctrl(BHY2_HIF_CTRL_ASYNC_STATUS_CHANNEL, bhy2));
    BHY2_ASSERT(bhy2_get_host_intf_ctrl(&expected_data, bhy2));
    if (!(expected_data & BHY2_HIF_CTRL_ASYNC_STATUS_CHANNEL))
    {
        WARNING("Expected Host Interface Control (0x06) to have bit 0x%x to be set\r\n",
                BHY2_HIF_CTRL_ASYNC_STATUS_CHANNEL);
    }
}

bool bhy2_are_sensors_active(void)
{
    for (uint16_t i = 0; i < 256; i++)
    {
        if (sensors_active[i])
        {
            return true;
        }
    }

    return false;
}

void bhy2_exit(struct bhy2_cli_ref *cli_ref)
{
    for (uint16_t i = 0; i < 256; i++)
    {
        if (sensors_active[i])
        {
            bhy2_set_virt_sensor_cfg(i, 0.0f, 0, &cli_ref->bhy2);
            sensors_active[i] = false;
        }
    }

    if (cli_ref->parse_table.logdev.logfile)
    {
        fclose(cli_ref->parse_table.logdev.logfile);
    }
}

void bhy2_data_parse_callback(struct bhy2_cli_ref *cli_ref)
{
    if (get_interrupt_status())
    {
        BHY2_ASSERT(bhy2_get_and_process_fifo(fifo_buffer, sizeof(fifo_buffer), &cli_ref->bhy2));
    }
}

bhy2_fifo_parse_callback_t bhy2_get_callback(uint8_t sensor_id)
{
    bhy2_fifo_parse_callback_t callback = NULL;

    switch (sensor_id)
    {
        case BHY2_SENSOR_ID_ACC:
        case BHY2_SENSOR_ID_ACC_WU:
        case BHY2_SENSOR_ID_ACC_PASS:
        case BHY2_SENSOR_ID_ACC_RAW:
        case BHY2_SENSOR_ID_ACC_RAW_WU:
        case BHY2_SENSOR_ID_ACC_BIAS:
        case BHY2_SENSOR_ID_ACC_BIAS_WU:
        case BHY2_SENSOR_ID_GRA:
        case BHY2_SENSOR_ID_GRA_WU:
        case BHY2_SENSOR_ID_LACC:
        case BHY2_SENSOR_ID_LACC_WU:
        case BHY2_SENSOR_ID_MAG:
        case BHY2_SENSOR_ID_MAG_WU:
        case BHY2_SENSOR_ID_MAG_PASS:
        case BHY2_SENSOR_ID_MAG_RAW:
        case BHY2_SENSOR_ID_MAG_RAW_WU:
        case BHY2_SENSOR_ID_MAG_BIAS:
        case BHY2_SENSOR_ID_MAG_BIAS_WU:
        case BHY2_SENSOR_ID_GYRO:
        case BHY2_SENSOR_ID_GYRO_WU:
        case BHY2_SENSOR_ID_GYRO_PASS:
        case BHY2_SENSOR_ID_GYRO_RAW:
        case BHY2_SENSOR_ID_GYRO_RAW_WU:
        case BHY2_SENSOR_ID_GYRO_BIAS:
        case BHY2_SENSOR_ID_GYRO_BIAS_WU:
            callback = parse_3axis_s16;
            break;
        case BHY2_SENSOR_ID_ORI:
        case BHY2_SENSOR_ID_ORI_WU:
            callback = parse_euler;
            break;
        case BHY2_SENSOR_ID_RV:
        case BHY2_SENSOR_ID_RV_WU:
        case BHY2_SENSOR_ID_GAMERV:
        case BHY2_SENSOR_ID_GAMERV_WU:
        case BHY2_SENSOR_ID_GEORV:
        case BHY2_SENSOR_ID_GEORV_WU:
            callback = parse_quaternion;
            break;
        case BHY2_SENSOR_ID_SIG:
        case BHY2_SENSOR_ID_SIG_HW:
        case BHY2_SENSOR_ID_SIG_HW_WU:
        case BHY2_SENSOR_ID_STD:
        case BHY2_SENSOR_ID_STD_WU:
        case BHY2_SENSOR_ID_STD_HW:
        case BHY2_SENSOR_ID_STD_HW_WU:
        case BHY2_SENSOR_ID_TILT_DETECTOR:
        case BHY2_SENSOR_ID_WAKE_GESTURE:
        case BHY2_SENSOR_ID_GLANCE_GESTURE:
        case BHY2_SENSOR_ID_PICKUP_GESTURE:
        case BHY2_SENSOR_ID_WRIST_TILT_GESTURE:
        case BHY2_SENSOR_ID_STATIONARY_DET:
        case BHY2_SENSOR_ID_MOTION_DET:
        case BHY2_SENSOR_ID_ANY_MOTION:
        case BHY2_SENSOR_ID_ANY_MOTION_WU:
            callback = parse_scalar_event;
            break;
        case BHY2_SENSOR_ID_EXCAMERA:
        case BHY2_SENSOR_ID_PROX:
        case BHY2_SENSOR_ID_PROX_WU:
        case BHY2_SENSOR_ID_HUM:
        case BHY2_SENSOR_ID_HUM_WU:
            callback = parse_scalar_u8;
            break;
        case BHY2_SENSOR_ID_STC:
        case BHY2_SENSOR_ID_STC_WU:
        case BHY2_SENSOR_ID_STC_HW:
        case BHY2_SENSOR_ID_STC_HW_WU:
        case BHY2_SENSOR_ID_GAS:
        case BHY2_SENSOR_ID_GAS_WU:
            callback = parse_scalar_u32;
            break;
        case BHY2_SENSOR_ID_AR:
            callback = parse_activity;
            break;
        case BHY2_SENSOR_ID_GPS:
            callback = parse_gps;
            break;
        case BHY2_SENSOR_ID_DEVICE_ORI:
        case BHY2_SENSOR_ID_DEVICE_ORI_WU:
            callback = parse_device_ori;
            break;
        case BHY2_SENSOR_ID_TEMP:
        case BHY2_SENSOR_ID_TEMP_WU:
        case BHY2_SENSOR_ID_LIGHT:
        case BHY2_SENSOR_ID_LIGHT_WU:
            callback = parse_s16_as_float;
            break;
        case BHY2_SENSOR_ID_BARO:
        case BHY2_SENSOR_ID_BARO_WU:
            callback = parse_u24_as_float;
            break;
        case BHY2_SENSOR_ID_KLIO:
            callback = parse_klio;
            break;
        case BHY2_SENSOR_ID_KLIO_LOG:
            callback = parse_klio_log;
            break;
        case BHY2_SENSOR_ID_PDR:
            callback = parse_pdr;
            break;
        case BHY2_SENSOR_ID_SWIM:
            callback = parse_swim;
            break;
        case BHY2_SENSOR_ID_SI_ACCEL:
        case BHY2_SENSOR_ID_SI_GYROS:
            callback = parse_acc_gyro;
            break;
        case BHY2_SENSOR_ID_AIR_QUALITY:
            callback = parse_air_quality;
            break;
        case BHY2_SENSOR_ID_HEAD_MIS_CALIB:
            callback = parse_hmc;
            break;
        case BHY2_SENSOR_ID_HEAD_ORIENT:
            callback = parse_oc;
            break;
        default:
            callback = parse_generic;
            break;
    }

    return callback;
}

int8_t kstatus_help(void *ref)
{
    PRINT("  kstatus\r\n");
    PRINT("        = Get and reset current klio driver status\r\n");

    return CLI_OK;
}

int8_t kstatus_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0]);
    klio_enable(&cli_ref->bhy2);
    klio_status(&cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t ksetstate_help(void *ref)
{
    PRINT("  ksetstate <le> <lr> <re> <rr>\r\n");
    PRINT("        = Set klio state\r\n");
    PRINT("         <le> learning enable (0/1)\r\n");
    PRINT("         <lr> learning reset (0/1)\r\n");
    PRINT("         <re> recognition enable (0/1)\r\n");
    PRINT("         <rr> recognition reset (0/1)\r\n");

    return CLI_OK;
}

int8_t ksetstate_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s %s %s %s\r\n",
         (char *)argv[0],
         (char *)argv[1],
         (char *)argv[2],
         (char *)argv[3],
         (char *)argv[4]);
    klio_enable(&cli_ref->bhy2);
    klio_set_state((char *)argv[1], (char *)argv[2], (char *)argv[3], (char *)argv[4], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kgetstate_help(void *ref)
{
    PRINT("  kgetstate\r\n");
    PRINT("        = Get current Klio state\r\n");

    return CLI_OK;
}

int8_t kldpatt_help(void *ref)
{
    PRINT("  kldpatt <index> <pattern>\r\n");
    PRINT("        = Load a pattern/adaptive pattern for recognition. If loading an\r\n");
    PRINT("          adaptive pattern, a regular pattern must have been previously\r\n");
    PRINT("          loaded on the given index\r\n");
    PRINT("         <index> pattern index to write to\r\n");
    PRINT("         <pattern> pattern/adaptive pattern as bare hex bytestring\r\n");

    return CLI_OK;
}

int8_t kenpatt_help(void *ref)
{
    PRINT("  kenpatt <patterns>\r\n");
    PRINT("        = Enable pattern ids for recognition\r\n");
    PRINT("         <patterns> pattern indices to enable, specified as 0,1,4 etc\r\n");

    return CLI_OK;
}

int8_t kdispatt_help(void *ref)
{
    PRINT("  kdispatt <patterns>\r\n");
    PRINT("        = Disable pattern ids for recognition\r\n");
    PRINT("         <patterns> pattern indices to disable, specified as 0,1,4 etc\r\n");

    return CLI_OK;
}

int8_t kdisapatt_help(void *ref)
{
    PRINT("  kdisapatt <patterns>\r\n");
    PRINT("        = Disable pattern adaption for given pattern ids\r\n");
    PRINT("         <patterns> pattern indices to disable, specified as 0,1,4 etc\r\n");

    return CLI_OK;
}

int8_t kswpatt_help(void *ref)
{
    PRINT("  kswpatt <patterns>\r\n");
    PRINT("        = Switch pattern between left/right hand\r\n");
    PRINT("         <patterns> pattern indices to switch, specified as 0,1,4 etc\r\n");

    return CLI_OK;
}

int8_t kautldpatt_help(void *ref)
{
    PRINT("  kautldpatt <enable> <index>\r\n");
    PRINT("        = Automatically use learnt patterns for recognition\r\n");
    PRINT("         <enable> enable or disable (0/1)\r\n");
    PRINT("         <index> pattern index to start loading into (normally 0)\r\n");

    return CLI_OK;
}

int8_t kgetparam_help(void *ref)
{
    PRINT("  kgetparam <param>\r\n");
    PRINT("        = Print klio parameter\r\n");
    PRINT("         <param> parameter id, see documentation\r\n");

    return CLI_OK;
}

int8_t kgetstate_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0]);
    klio_enable(&cli_ref->bhy2);
    klio_get_state(&cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kldpatt_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    klio_enable(&cli_ref->bhy2);
    klio_load_pattern((char *)argv[1], (char *)argv[2], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kenpatt_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    klio_enable(&cli_ref->bhy2);
    klio_pattern_state_operation(KLIO_PATTERN_STATE_ENABLE, (char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kdispatt_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    klio_enable(&cli_ref->bhy2);
    klio_pattern_state_operation(KLIO_PATTERN_STATE_DISABLE, (char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kdisapatt_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    klio_enable(&cli_ref->bhy2);
    klio_pattern_state_operation(KLIO_PATTERN_STATE_AP_DISABLE, (char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kswpatt_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    klio_enable(&cli_ref->bhy2);
    klio_pattern_state_operation(KLIO_PATTERN_STATE_SWITCH_HAND, (char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kautldpatt_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s %s\r\n", argv[0], argv[1], argv[2]);
    klio_enable(&cli_ref->bhy2);
    klio_vars.auto_load_pattern = atoi((char *)argv[1]);
    klio_vars.auto_load_pattern_write_index = atoi((char *)argv[2]);
    INFO("Klio auto load pattern set to %s, index is %s\r\n", argv[1], argv[2]);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kgetparam_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    klio_enable(&cli_ref->bhy2);
    klio_get_parameter(argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t ksetparam_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s %s\r\n", argv[0], argv[1], argv[2]);
    klio_enable(&cli_ref->bhy2);
    klio_set_parameter((char *)argv[1], (char *)argv[2], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t ksimscore_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0], argv[1], argv[2]);
    klio_enable(&cli_ref->bhy2);
    klio_similarity_score(argv[1], argv[2], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t kmsimscore_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0], argv[1], argv[2]);
    klio_enable(&cli_ref->bhy2);
    klio_similarity_score_multiple((const char *)argv[1], (const char *)argv[2], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t ksetparam_help(void *ref)
{
    PRINT("  ksetparam <param> <value>\r\n");
    PRINT("        = Set klio parameter\r\n");
    PRINT("         <param> parameter id, see documentation\r\n");
    PRINT("         <value> depends on parameter id, see documentation\r\n");

    return CLI_OK;
}

int8_t ksimscore_help(void *ref)
{
    PRINT("  ksimscore <pattern1> <pattern2>\r\n");
    PRINT("        = Print similarity score for two patterns\r\n");
    PRINT("         <pattern1> first pattern as bare hex bytestring\r\n");
    PRINT("         <pattern2> second pattern as bare hex bytestring\r\n");

    return CLI_OK;
}

int8_t kmsimscore_help(void *ref)
{
    PRINT("  kmsimscore <base index> <comparison indices>\r\n");
    PRINT("        = Print similarity score for one or more stored patterns\r\n");
    PRINT("         <base index> compare the patterns in <comparison indices> with this pattern index\r\n");
    PRINT(
        "         <comparison indices> pattern indices to compare with pattern in base index, specified as 0,1,4 etc\r\n");

    return CLI_OK;
}

int8_t version_help(void *ref)
{
    PRINT("  version\r\n");
    PRINT("        = Prints the version\r\n");

    return CLI_OK;
}

int8_t version_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct coines_board_info board_info;

    INFO("Executing %s\r\n", argv[0]);

    coines_get_board_info(&board_info);

    PRINT("HW info:: Board: %u, HW ID: %X, Shuttle ID: %X, SW ID: %X\r\n",
          board_info.board,
          board_info.hardware_id,
          board_info.shuttle_id,
          board_info.software_id);
    PRINT("SW Version: %s.%s.%s\r\nBuild date: " __DATE__ "\r\n\r\n\r\n",
          BHY2CLI_VER_MAJOR,
          BHY2CLI_VER_MINOR,
          BHY2CLI_VER_BUGFIX);

    return CLI_OK;
}

int8_t help_help(void *ref)
{
    PRINT("Usage:\r\n");
    PRINT("bhy2cli [<options>]\r\n");
    PRINT("Options:\r\n");
    PRINT("  -h OR help\t= Print this usage message\r\n");

    return CLI_OK;
}

int8_t help_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0]);

    cli_help(ref, &(cli_ref->cli_dev));

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t info_help(void *ref)
{
    PRINT("  -i OR info\t= Show device information: Device ID,\r\n");
    PRINT("    \t  ROM version, RAM version, Power state,\r\n");
    PRINT("    \t  list of available sensors,\r\n");
    PRINT("    \t  content of Boot Status register,\r\n");
    PRINT("    \t  content of Error value register\r\n");

    return CLI_OK;
}

int8_t info_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0]);
    show_info(&cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t ramb_help(void *ref)
{
    PRINT("  -b OR ramb <firmware path>\r\n");
    PRINT("    \t= Reset, upload specified firmware to RAM and boot from RAM\r\n");
    PRINT("    \t  [equivalent to using \"reset ram <firmware> boot r\" successively]\r\n");

    return CLI_OK;
}

int8_t ramb_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    reset_hub(&cli_ref->bhy2);
    if (upload_to_ram((char *)argv[1], &cli_ref->bhy2))
    {
        boot_ram(&cli_ref->bhy2);
        bhy2_get_virt_sensor_list(&cli_ref->bhy2);
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t flb_help(void *ref)
{
    PRINT("   -d OR flb <firmware path>\r\n");
    PRINT("    \t= Reset, upload specified firmware to Flash and boot from Flash\r\n");
    PRINT("    \t  [equivalent to using \"reset fl <firmware path> boot f\" successively]\r\n");

    return CLI_OK;
}

int8_t flb_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    reset_hub(&cli_ref->bhy2);
    if (upload_to_flash((char *)argv[1], &cli_ref->bhy2))
    {
        boot_flash(&cli_ref->bhy2);
        bhy2_get_virt_sensor_list(&cli_ref->bhy2);
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t reset_help(void *ref)
{
    PRINT("  -n OR reset\t= Reset sensor hub\r\n");

    return CLI_OK;
}

int8_t reset_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0]);
    reset_hub(&cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t addse_help(void *ref)
{
    PRINT("  -a OR addse <sensor id>:<sensor name>:<total output payload in bytes>:\r\n");
    PRINT("     <output_format_0>:<output_format_1>\r\n");
    PRINT("    \t= Register the expected payload of a new custom virtual sensor\r\n");
    PRINT("    \t -Valid output_formats: u8: Unsigned 8 Bit, u16: Unsigned 16 Bit, u32:\r\n");
    PRINT("    \t  Unsigned 32 Bit, s8: Signed 8 Bit, s16: Signed 16 Bit, s32: Signed 32 Bit,\r\n");
    PRINT("    \t  f: Float, c: Char \r\n");
    PRINT("    \t -e.g.: addse 160:\"Lean Orientation\":2:c:c \r\n");
    PRINT("    \t -Note that the corresponding virtual sensor has to be enabled in the same function\r\n");
    PRINT("    \t  call (trailing actse option), since the registration of the sensor is temporary. \r\n");

    return CLI_OK;
}

int8_t addse_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    add_sensor((char *)argv[1], ref);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t rd_help(void *ref)
{
    PRINT("  -r OR rd <adr>[:<len>]\r\n");
    PRINT("    \t= Read from register address <adr> for length <len> bytes\r\n");
    PRINT("    \t -If input <len> is not provided, the default read length is 1 byte\r\n");
    PRINT("    \t -When reading registers with auto-increment, the provided register as well as\r\n");
    PRINT("    \t  the following registers will be read\r\n");
    PRINT("    \t -e.g rd 0x08:3 will read the data of registers 0x08, 0x09 and 0x0a\r\n");
    PRINT("    \t  max. 53 bytes can be read at once\r\n");

    return CLI_OK;
}

int8_t rd_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    rd_regs((char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t wr_help(void *ref)
{
    PRINT("  -w OR wr <adr>=<val1>[,<val2>]...\r\n");
    PRINT("    \t= Write to register address <adr> with comma separated values <val>\r\n");
    PRINT("    \t -If more values provided <val>, the additional\r\n");
    PRINT("    \t  values will be written to the following addresses\r\n");
    PRINT("    \t -When writing to registers with auto-increment, the provided register as well as\r\n");
    PRINT("    \t  the following registers will be written\r\n");
    PRINT("    \t -e.g wr 0x08=0x02,0x03,0x04 will write the provided data to registers 0x08, 0x09\r\n");
    PRINT("    \t  and 0x0a. Max. 46 bytes can be written at once\r\n");

    return CLI_OK;
}

int8_t wr_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    wr_regs((char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t rdp_help(void *ref)
{
    PRINT("  -s OR rdp <param id>\r\n");
    PRINT("    \t= Display read_param response of parameter <param id>\r\n");

    return CLI_OK;
}

int8_t rdp_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    rd_param((char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t wrp_help(void *ref)
{
    PRINT("  -t OR wrp <param id>=<val1>[,<val2>]...\r\n");
    PRINT("    \t= Write data to parameter <param id> with the bytes to be written, <val1>[,<val2>]... \r\n");
    PRINT("    \t -e.g. 0x103=5,6 will write 0x05 to the first byte and 0x06 to the second byte\r\n");
    PRINT("    \t  of the parameter \"Fifo Control\"\r\n");

    return CLI_OK;
}

int8_t wrp_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    wr_param((char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t ram_help(void *ref)
{
    PRINT("  ram <firmware path>\r\n");
    PRINT("    \t= Upload firmware to RAM\r\n");

    return CLI_OK;
}

int8_t ram_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    upload_to_ram((char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t fl_help(void *ref)
{
    PRINT("  fl <firmware path>\r\n");
    PRINT("    \t= Upload firmware to external-flash\r\n");

    return CLI_OK;
}

int8_t fl_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    upload_to_flash((char *)argv[1], &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t boot_help(void *ref)
{
    PRINT("  -g OR boot <medium>\r\n");
    PRINT("    \t= Boot from the specified <medium>: \"f\" for FLASH, \"r\" for RAM\r\n");

    return CLI_OK;
}

int8_t boot_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    if ((argv[1][0]) == 'r')
    {
        boot_ram(&cli_ref->bhy2);
        bhy2_get_virt_sensor_list(&cli_ref->bhy2);
    }
    else if ((argv[1][0]) == 'f')
    {
        boot_flash(&cli_ref->bhy2);
        bhy2_get_virt_sensor_list(&cli_ref->bhy2);
    }
    else
    {
        ERROR("Invalid boot medium: %s\r\n", argv[1]);

        return CLI_E_INVALID_PARAM;
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t erase_help(void *ref)
{
    PRINT("  -e OR erase\t= Erase external-flash\r\n");

    return CLI_OK;
}

int8_t erase_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s \r\n", argv[0]);
    erase_flash(BHY2_FLASH_SIZE_4MB, &cli_ref->bhy2);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t efd_help(void *ref)
{
    PRINT("  efd\t= Erase the flash descriptor\r\n");

    return CLI_OK;
}

int8_t efd_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    struct bhy2_dev *bhy2 = &cli_ref->bhy2;
    int8_t rslt;
    uint8_t boot_status;

    BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        if ((boot_status & BHY2_BST_HOST_FW_VERIFY_DONE) || (boot_status & BHY2_BST_FLASH_VERIFY_DONE))
        {
            ERROR("Seems like a firmware is running. Reset the BHI260/BHA260 before erasing the flash descriptor\r\n");

            return CLI_OK;
        }
    }

    PRINT("Erasing flash descriptor. This might hang if a reset command before this was not issued\r\n");
    rslt = bhy2_erase_flash(0, 0xFFF, bhy2); /* 0xFFF is hopefully within the first sector */
    if (rslt != BHY2_OK)
    {
        ERROR("Erasing flash descriptor failed, status: %02d\r\n", rslt);

        return CLI_OK;
    }

    PRINT("Erasing flash descriptor successful\r\n");

    return CLI_OK;
}

int8_t actse_help(void *ref)
{
    PRINT("  -c OR actse <sensor id>:<frequency>[:<latency>]\r\n");
    PRINT("    \t= Activate sensor <sensor id> at specified sample rate <frequency>,\r\n");
    PRINT("    \t -latency <latency>, duration time <time>, sample counts <count>\r\n");
    PRINT("    \t -At least <frequency> is a must input parameter\r\n");
    PRINT("    \t -<latency> is optional\r\n");
    PRINT("    \t -One or more sensors can be active by passing multiple actse options\r\n");
    PRINT("    \t -id: sensor id\r\n");
    PRINT("    \t -frequency(Hz): sensor ODR\r\n");
    PRINT("    \t -latency(ms): sensor data outputs with a latency\r\n");

    return CLI_OK;
}

int8_t actse_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    activate_sensor((char *)argv[1], PARSE_FLAG_STREAM, (struct bhy2_cli_ref *)ref);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t logse_help(void *ref)
{
    PRINT("  logse <sensor id>:<frequency>[:<latency>]\r\n");
    PRINT("    \t= Log sensor <sensor id> at specified sample rate <frequency>,\r\n");
    PRINT("    \t -latency <latency>, duration time <time>, sample counts <count>\r\n");
    PRINT("    \t -At least <frequency> is a must input parameter\r\n");
    PRINT("    \t -<latency> is optional\r\n");
    PRINT("    \t -One or more sensors can be active by passing multiple logse options\r\n");
    PRINT("    \t -id: sensor id\r\n");
    PRINT("    \t -frequency(Hz): sensor ODR\r\n");
    PRINT("    \t -latency(ms): sensor data outputs with a latency\r\n");

    return CLI_OK;
}

int8_t logse_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    activate_sensor((char *)argv[1], PARSE_FLAG_LOG, (struct bhy2_cli_ref *)ref);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t attlog_help(void *ref)
{
    PRINT("  attlog <filename.ext>\r\n");
    PRINT("    \t= Attach (and create if required) a log file (write-only),");
    PRINT(" where data can be logged to\r\n");

    return CLI_OK;
}

int8_t attlog_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    cli_ref->parse_table.logdev.logfile = fopen((char *)argv[1], "wb");
    if (cli_ref->parse_table.logdev.logfile)
    {
        PRINT("File %s was created\r\n", argv[1]);
        write_meta_info(&cli_ref->parse_table.logdev, &cli_ref->bhy2);
    }
    else
    {
        ERROR("File %s could not be found/created\r\n", argv[1]);
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t detlog_help(void *ref)
{
    PRINT("  detlog <filename.ext>\r\n");
    PRINT("    \t= Detach the log file \r\n");

    return CLI_OK;
}

int8_t detlog_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    fclose(cli_ref->parse_table.logdev.logfile);
    cli_ref->parse_table.logdev.logfile = NULL;

    PRINT("File %s was detached for logging\r\n", argv[1]);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t slabel_help(void *ref)
{
    PRINT("  slabel <label string>\r\n");
    PRINT("    \t= Set a string label in the log file %u characters long\r\n", LOGBIN_LABEL_SIZE);

    return CLI_OK;
}

int8_t slabel_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    uint8_t label[LOGBIN_LABEL_SIZE] = { 0 };
    uint64_t timestamp_ns;

    if (strlen((char *)argv[1]) <= LOGBIN_LABEL_SIZE)
    {
        strcpy((char *)label, (char *)argv[1]);
    }
    else
    {
        memcpy(label, argv[1], LOGBIN_LABEL_SIZE);
    }

    INFO("Executing %s %s %s\r\n", argv[0], argv[1], label);
    bhy2_get_hw_timestamp_ns(&timestamp_ns, &cli_ref->bhy2);

    /* System IDs start at 224 */
    log_data(LOGBIN_META_ID_LABEL, timestamp_ns, LOGBIN_LABEL_SIZE, label, &cli_ref->parse_table.logdev);

    return CLI_OK;
}

int8_t pfullreset_help(void *ref)
{
    PRINT("  pfullreset\r\n");
    PRINT("    \t= Triggers a full reset of the PDR algorithm\r\n");

    return CLI_OK;
}

int8_t pfullreset_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0]);

    BHY2_ASSERT(bhy2_pdr_reset_full(&cli_ref->bhy2));

    PRINT("Full reset complete\r\n\r\n\r\n");

    return CLI_OK;
}

int8_t ptrackreset_help(void *ref)
{
    PRINT("  ptrackreset\r\n");
    PRINT("    \t= Triggers a reset of the PDR track\r\n");

    return CLI_OK;
}

int8_t ptrackreset_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;

    INFO("Executing %s\r\n", argv[0]);

    BHY2_ASSERT(bhy2_pdr_reset_position(&cli_ref->bhy2));

    PRINT("Track reset complete\r\n\r\n\r\n");

    return CLI_OK;
}

int8_t prefheaddel_help(void *ref)
{
    PRINT("  prefheaddel <heading>\r\n");
    PRINT("    \t= Set the reference heading delta.\r\n");
    PRINT("    \t <heading> values from 0.0 to 359.9 degrees\r\n");

    return CLI_OK;
}

int8_t prefheaddel_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    float heading;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    heading = (float)atof((char *)argv[1]);

    INFO("HD = %f\r\n", heading);

    BHY2_ASSERT(bhy2_pdr_set_ref_heading_del(heading, &cli_ref->bhy2));

    PRINT("Setting reference heading delta\r\n\r\n");

    return CLI_OK;
}

int8_t pstepinfo_help(void *ref)
{
    PRINT("  pstepinfo <step length> <step length accuracy>\r\n");
    PRINT("    \t= Set the step length.\r\n");
    PRINT("    \t <step length> values from 0.00 to 5.00 meters\r\n");
    PRINT("    \t <step accuracy> values from 0.00 to 5.00 meters\r\n");

    return CLI_OK;
}

int8_t pstepinfo_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    float step_length, step_length_acc;

    INFO("Executing %s %s %s\r\n", argv[0], argv[1], argv[2]);

    step_length = (float)atof((char *)argv[1]);
    step_length_acc = (float)atof((char *)argv[2]);

    INFO("SL, SLA = %f, %f\r\n", step_length, step_length_acc);

    BHY2_ASSERT(bhy2_pdr_set_step_info(step_length, step_length_acc, &cli_ref->bhy2));

    PRINT("Setting step length\r\n\r\n\r\n");

    return CLI_OK;
}

int8_t psethand_help(void *ref)
{
    PRINT("  psethand <hand>\r\n");
    PRINT("    \t= Set the hand position.\r\n");
    PRINT("    \t <hand> 0 left, 1 right\r\n");

    return CLI_OK;
}

int8_t psethand_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    uint8_t hand;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    hand = (uint8_t)atoi((char *)argv[1]);

    BHY2_ASSERT(bhy2_pdr_set_hand(hand, &cli_ref->bhy2));

    PRINT("Setting hand to %s\r\n\r\n", hand ? "right" : "left");

    return CLI_OK;
}

int8_t pdrver_help(void *ref)
{
    PRINT("  pdrver \r\n");
    PRINT("    \t= Get the driver version.\r\n");

    return CLI_OK;
}

int8_t pdrver_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    struct bhy2_pdr_ver version;

    INFO("Executing %\r\n", argv[0]);

    BHY2_ASSERT(bhy2_pdr_get_driver_version(&version, &cli_ref->bhy2));

    PRINT("Driver version: %u.%u.%u. Firmware build number %u\r\n\r\n\r\n",
          version.major,
          version.minor,
          version.patch,
          version.fw_build);

    return CLI_OK;
}

int8_t palver_help(void *ref)
{
    PRINT("  palver\r\n");
    PRINT("    \t= Get the algorithm version\r\n");

    return CLI_OK;
}

int8_t palver_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    struct bhy2_pdr_ver ver;

    INFO("Executing %s\r\n", argv[0]);

    BHY2_ASSERT(bhy2_pdr_get_algo_version(&ver, &cli_ref->bhy2));

    PRINT("Algorithm version: %u.%u.%u. Firmware build number %u\r\n\r\n\r\n",
          ver.major,
          ver.minor,
          ver.patch,
          ver.fw_build);

    return CLI_OK;
}

int8_t pvariant_help(void *ref)
{
    PRINT("  pvariant\r\n");
    PRINT("    \t= Get the PDR variant.\r\n");

    return CLI_OK;
}

int8_t pvariant_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    uint8_t variant;

    INFO("Executing %s\r\n", argv[0]);

    BHY2_ASSERT(bhy2_pdr_get_pdr_variant(&variant, &cli_ref->bhy2));
    PRINT("PDR variant: ");
    switch (variant)
    {
        case BHY2_PDR_VAR_6DOF:
            PRINT("6DoF");
            break;
        case BHY2_PDR_VAR_9DOF:
            PRINT("9DoF");
            break;
        default:
            PRINT("Undefined [%u]", variant);
    }

    PRINT("\r\n\r\n\r\n");

    return CLI_OK;
}

int8_t pdevpos_help(void *ref)
{
    PRINT("  pdevpos\r\n");
    PRINT("    \t= Get the Device position,\r\n");

    return CLI_OK;
}

int8_t pdevpos_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    uint8_t dev_pos;

    INFO("Executing %s\r\n", argv[0]);

    BHY2_ASSERT(bhy2_pdr_get_device_position(&dev_pos, &cli_ref->bhy2));
    PRINT("Device position: ");
    switch (dev_pos)
    {
        case BHY2_PDR_DEV_POS_WRIST:
            PRINT("Wrist");
            break;
        case BHY2_PDR_DEV_POS_HEAD:
            PRINT("Head");
            break;
        case BHY2_PDR_DEV_POS_SHOE:
            PRINT("Shoe");
            break;
        case BHY2_PDR_DEV_POS_BACKPACK:
            PRINT("Backpack");
            break;
        default:
            PRINT("Undefined [%u]", dev_pos);
    }

    PRINT("\r\n\r\n\r\n");

    return CLI_OK;
}

int8_t swim_help(void *ref)
{
    PRINT("  swim <e/d> <r/l> <pool length>\r\n");
    PRINT("    \t<e/d>= Enable / Disable\r\n");
    PRINT("    \t<r/l>= Right / Left\r\n");
    PRINT("    \t<length>= Length of the pool as an integer\r\n");

    return CLI_OK;
}

int8_t swimver_help(void *ref)
{
    PRINT("  swimver\r\n");
    PRINT("    \t= Get the algorithm version\r\n");

    return CLI_OK;
}

int8_t swimgetfreq_help(void *ref)
{
    PRINT("  swimgetfreq\r\n");
    PRINT("    \t To Get the Swim sampling frequency\r\n");

    return CLI_OK;
}

int8_t swimsetfreq_help(void *ref)
{
    PRINT("  swimsetfreq <Freq> <latency>\r\n");
    PRINT("    \t To SET the Swim sampling frequency\r\n");
    PRINT("    \t <Freq> = Frequency (in Hz) to set \r\n");
    PRINT("    \t <Latency> = latency (ms) to set \r\n");

    return CLI_OK;
}

int8_t swimgetaxes_help(void *ref)
{
    PRINT("  swimgetaxes\r\n");
    PRINT("    \t= Get the orientation of Physical sensor set for swim algorithm\r\n");

    return CLI_OK;
}

int8_t swimsetaxes_help(void *ref)
{
    PRINT("  swimsetaxes <orientation_matrix>\r\n");
    PRINT("    \t= Set the orientation of Physical sensor set for swim algorithm\r\n");

    return CLI_OK;
}

int8_t swimgetfreq_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    struct bhy2_virt_sensor_conf sensor_conf;

    INFO("Executing %s\r\n", argv[0]);

    if ((assert_rslt = bhy2_get_virt_sensor_cfg(BHY2_SENSOR_ID_SWIM, &sensor_conf, &cli_ref->bhy2)))
    {
        PRINT("SWIMFREQ GET Failed %d\r\n\r\n\r\n", assert_rslt);
    }
    else
    {
        PRINT("SWIMFREQ %.2f\r\n\r\n\r\n", sensor_conf.sample_rate);
    }

    return CLI_OK;
}

int8_t swimsetfreq_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    bhy2_float sample_rate = 0.0F;
    uint32_t latency = 0;

    INFO("Executing %s\r\n", argv[0]);

    sample_rate = atof((const char *)argv[1]);
    latency = atoi((const char *)argv[2]);

    if ((assert_rslt = bhy2_set_virt_sensor_cfg(BHY2_SENSOR_ID_SWIM, sample_rate, latency, &cli_ref->bhy2)))
    {
        PRINT("SWIMFREQ SET Failed %d\r\n\r\n\r\n", assert_rslt);
    }
    else
    {
        PRINT("SWIMFREQ SET Success\r\n\r\n\r\n");
    }

    return CLI_OK;
}

int8_t swimver_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    bhy2_swim_version_t swim_algo_ver;
    bhy2_swim_config_param_t config;

    INFO("Executing %s\r\n", argv[0]);
    if (bhy2_swim_get_version(&swim_algo_ver, &cli_ref->bhy2))
    {
        PRINT("SWIMVER 0.0.0\r\n\r\n\r\n");
    }
    else
    {
        if (bhy2_swim_get_config(&config, &cli_ref->bhy2))
        {
            PRINT("SWIMVER %u.%u.%u\r\n\r\n\r\n",
                  swim_algo_ver.improvement,
                  swim_algo_ver.bugfix,
                  swim_algo_ver.platform);
        }
        else
        {
            PRINT("SWIMVER %u.%u.%u\r\nSWIMCONF %s %u\r\n\r\n\r\n",
                  swim_algo_ver.improvement,
                  swim_algo_ver.bugfix,
                  swim_algo_ver.platform,
                  config.dev_on_left_hand ? "LEFT" : "RIGHT",
                  config.pool_length_integral);
        }
    }

    return CLI_OK;
}

int8_t swimgetaxes_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    struct bhy2_orient_matrix orient_matrix;
    uint8_t index;

    INFO("Executing %s\r\n", argv[0]);
    BHY2_ASSERT(bhy2_get_orientation_matrix(BHY2_PHYS_SENSOR_ID_ACCELEROMETER, &orient_matrix, &cli_ref->bhy2));
    PRINT("Acc ");

    for (index = 0; index < 8; index++)
    {
        PRINT("%d,", orient_matrix.c[index]);
    }

    PRINT("%d\r\n\r\n", orient_matrix.c[index]);

    memset(&orient_matrix.c[0], 0x0, sizeof(orient_matrix.c) / sizeof(orient_matrix.c[0]));
    BHY2_ASSERT(bhy2_get_orientation_matrix(BHY2_PHYS_SENSOR_ID_GYROSCOPE, &orient_matrix, &cli_ref->bhy2));
    PRINT("Gyro ");

    for (index = 0; index < 8; index++)
    {
        PRINT("%d,", orient_matrix.c[index]);
    }

    PRINT("%d\r\n\r\n", orient_matrix.c[index]);

    return CLI_OK;
}

int8_t swimsetaxes_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    struct bhy2_orient_matrix orient_matrix;
    uint8_t index = 0;
    char delimiter[] = ",";
    char *axes = (char *)argv[1];
    char *token = strtok(axes, delimiter);

    while ((token != NULL))
    {
        orient_matrix.c[index] = atoi(token);
        token = strtok(NULL, delimiter);
        index++;
    }

    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    BHY2_ASSERT(bhy2_set_orientation_matrix(BHY2_PHYS_SENSOR_ID_ACCELEROMETER, orient_matrix, &cli_ref->bhy2));
    BHY2_ASSERT(bhy2_set_orientation_matrix(BHY2_PHYS_SENSOR_ID_GYROSCOPE, orient_matrix, &cli_ref->bhy2));

    return CLI_OK;
}

int8_t swim_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    struct bhy2_cli_ref *cli_ref = (struct bhy2_cli_ref *)ref;
    bhy2_swim_config_param_t config;

    INFO("Executing %s %s %s %s\r\n", argv[0], argv[1], argv[2], argv[3]);

    config.update_swim_config = (argv[1][0] == 'e') ? BHY2_SWIM_ENABLE_CONFIG : BHY2_SWIM_DISABLE_CONFIG;
    config.dev_on_left_hand = (argv[2][0] == 'r') ? BHY2_SWIM_DEVICE_ON_RIGHT_HAND : BHY2_SWIM_DEVICE_ON_LEFT_HAND;
    config.pool_length_integral = atoi((char *)argv[3]);
    config.pool_length_floating = 0;

    BHY2_ASSERT(bhy2_swim_set_config(&config, &cli_ref->bhy2));

    PRINT("Setting swim config: %s, %s, %u\r\n\r\n",
          (argv[1][0] == 'e') ? "Enable" : "Disable",
          (argv[2][0] == 'r') ? "Right" : "Left",
          atoi((char *)argv[3]));

    /*! Writing the final swim output after disabling swim sensor*/
    if (config.update_swim_config == BHY2_SWIM_DISABLE_CONFIG)
    {
        PRINT("Summary D: %u; C: %u; FRS: %u; BRS: %u; BTS: %u; BKS: %u; STC: %u\r\n",
              swim_data.total_distance,
              swim_data.length_count,
              swim_data.lengths_freestyle,
              swim_data.lengths_breaststroke,
              swim_data.lengths_butterfly,
              swim_data.lengths_backstroke,
              swim_data.stroke_count);
    }

    return CLI_OK;
}

static void time_to_s_ns(uint64_t time_ticks, uint32_t *s, uint32_t *ns, uint64_t *tns)
{
    *tns = time_ticks * 15625; /* timestamp is now in nanoseconds */
    *s = (uint32_t)(*tns / UINT64_C(1000000000));
    *ns = (uint32_t)(*tns - ((*s) * UINT64_C(1000000000)));
}

void parse_klio(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint32_t s, ns;
    uint64_t tns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    bhy2_klio_sensor_frame_t data;
    uint8_t parse_flag;
    uint32_t klio_driver_status;
    uint8_t tmp_buf[252];
    uint16_t bufsize = sizeof(tmp_buf);
    bhy2_klio_sensor_state_t klio_sensor_state;
    struct bhy2_dev *bhy2;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    bhy2 = parse_table->bhy2;

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;

    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    memcpy(&data, callback_info->data_ptr, sizeof(data));

    if (data.learn.index != -1)
    {
        /* Read out learnt pattern */
        BHY2_ASSERT(bhy2_klio_read_pattern(0, tmp_buf, &bufsize, bhy2));
        BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));

        DATA("SID: %u; T: %lu.%09lu; Pattern learnt: ", callback_info->sensor_id, s, ns);
        for (uint16_t i = 0; i < bufsize; i++)
        {
            PRINT_D("%02x", tmp_buf[i]);
        }

        PRINT_D("\r\n");

        /* write back learnt pattern for recognition */
        if (klio_vars.auto_load_pattern && klio_vars.auto_load_pattern_write_index < klio_vars.max_patterns)
        {
            BHY2_ASSERT(bhy2_klio_write_pattern(klio_vars.auto_load_pattern_write_index, tmp_buf, bufsize, bhy2));
            BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));

            BHY2_ASSERT(bhy2_klio_set_pattern_states(KLIO_PATTERN_STATE_ENABLE,
                                                     &klio_vars.auto_load_pattern_write_index, 1, bhy2));
            BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));

            klio_vars.auto_load_pattern_write_index++;

            /* write klio state (enable recognition, and also make sure learning is not disabled) */
            klio_sensor_state.learning_enabled = 1;
            klio_sensor_state.learning_reset = 0;
            klio_sensor_state.recognition_enabled = 1;
            klio_sensor_state.recognition_reset = 0;
            BHY2_ASSERT(bhy2_klio_set_state(&klio_sensor_state, bhy2));
            BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));
        }
    }

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %lu.%09lu; Learning [Id:%d Progress:%u Change:%u]; Recognition[Id:%d Count:%f]\r\n",
             callback_info->sensor_id,
             s,
             ns,
             data.learn.index,
             data.learn.progress,
             data.learn.change_reason,
             data.recognize.index,
             data.recognize.count);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void parse_klio_log(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint32_t s, ns;
    uint64_t tns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    bhy2_klio_log_frame_t data;
    uint8_t parse_flag;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;

    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    memcpy(&data, callback_info->data_ptr, sizeof(data));

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %.9g; ax: %.9g, ay: %.9g, az: %.9g, gx: %.9g, gy: %.9g, gz: %.9g\r\n",
             callback_info->sensor_id,
             data.timestamp,
             data.accel[0],
             data.accel[1],
             data.accel[2],
             data.gyro[0],
             data.gyro[1],
             data.gyro[2]);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void parse_pdr(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint32_t s, ns;
    uint64_t tns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    struct bhy2_pdr_frame pdr_data;
    uint8_t parse_flag;
    float x, y, d, hor_acc, heading, heading_acc;
    char full_reset[] = "Full reset";
    char track_reset[] = "Track reset";
    char no_status[] = "";
    char *full_reset_status;
    char *track_reset_status;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;
    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    bhy2_pdr_parse_frame(callback_info->data_ptr, &pdr_data);
    x = pdr_data.pos_x / 10.0f;
    y = pdr_data.pos_y / 10.0f;
    d = sqrt((x * x) + (y * y)); /* Displacement from origin */
    hor_acc = pdr_data.hor_acc / 10.0f;
    heading = pdr_data.heading / 10.0f;
    heading_acc = pdr_data.heading_acc / 10.0f;
    full_reset_status = (pdr_data.status & 0x01) ? full_reset : no_status;
    track_reset_status = (pdr_data.status & 0x02) ? track_reset : no_status;

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %lu.%09lu; X: %f, Y: %f, D: %f, HorAcc %f; H: %f, HAcc: %f; SC: %u; S: %s %s\r\n",
             callback_info->sensor_id,
             s,
             ns,
             x,
             y,
             d,
             hor_acc,
             heading,
             heading_acc,
             pdr_data.step_count,
             full_reset_status,
             track_reset_status);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void parse_acc_gyro(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint8_t parse_flag = 0;
    uint32_t s = 0, ns = 0;
    uint64_t tns = 0;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    float *sensor_data = (float *)callback_info->data_ptr;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;
    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %lu.%09lu; X: %f; Y: %f; Z: %f;\r\n",
             callback_info->sensor_id,
             s,
             ns,
             sensor_data[0],
             sensor_data[1],
             sensor_data[2]);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void parse_air_quality(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint8_t parse_flag = 0;
    uint32_t s = 0, ns = 0;
    uint64_t tns = 0;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    struct parse_sensor_details *sensor_details;
    struct bhy2_bsec_air_quality aq = { 0 };

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;
    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    bhy2_bsec_parse_air_quality(callback_info->data_ptr, &aq);

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %lu.%09lu; T: %.2f, H: %.2f, G: %.2f, I: %.2f, S: %.2f, C: %.2f, V: %.2f, A: %u\r\n",
             callback_info->sensor_id,
             s,
             ns,
             aq.comp_temp,
             aq.comp_hum,
             aq.comp_gas,
             aq.iaq,
             aq.static_iaq,
             aq.e_co2,
             aq.voc,
             aq.iaq_accuracy);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void parse_hmc(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    struct bhy2_head_tracker_data data;
    uint32_t s, ns;
    uint64_t tns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    uint8_t parse_flag;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        INFO("Parse slot not defined for %u\r\n", callback_info->sensor_id);

        return;
    }

    parse_flag = sensor_details->parse_flag;

    bhy2_head_tracker_parsing(callback_info->data_ptr, &data);

    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %lu.%09lu; x: %f, y: %f, z: %f, w: %f; acc: %u\r\n",
             callback_info->sensor_id,
             s,
             ns,
             data.x / 16384.0f,
             data.y / 16384.0f,
             data.z / 16384.0f,
             data.w / 16384.0f,
             data.accuracy);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void parse_oc(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    struct bhy2_head_tracker_data data;
    uint32_t s, ns;
    uint64_t tns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    uint8_t parse_flag;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        INFO("Parse slot not defined for %u\r\n", callback_info->sensor_id);

        return;
    }

    parse_flag = sensor_details->parse_flag;

    bhy2_head_tracker_parsing(callback_info->data_ptr, &data);

    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %lu.%09lu; x: %f, y: %f, z: %f, w: %f; acc: %u\r\n",
             callback_info->sensor_id,
             s,
             ns,
             data.x / 16384.0f,
             data.y / 16384.0f,
             data.z / 16384.0f,
             data.w / 16384.0f,
             data.accuracy);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void parse_swim(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint32_t s, ns;
    uint64_t tns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    uint8_t parse_flag;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;
    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    bhy2_swim_parse_data(callback_info->data_ptr, &swim_data);

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        DATA("SID: %u; T: %lu.%09lu; D: %u; C: %u; FRS: %u; BRS: %u; BTF: %u; BKS: %u; STC: %u\r\n",
             callback_info->sensor_id,
             s,
             ns,
             swim_data.total_distance,
             swim_data.length_count,
             swim_data.lengths_freestyle,
             swim_data.lengths_breaststroke,
             swim_data.lengths_butterfly,
             swim_data.lengths_backstroke,
             swim_data.stroke_count);
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

void bhy2_install_callbacks(struct bhy2_dev *bhy2, struct parse_ref *parse_table)
{
    for (uint8_t i = 0; i < BHY2_MAX_SIMUL_SENSORS; i++)
    {
        parse_table->sensor[i].parse_flag = PARSE_FLAG_NONE;
        parse_table->sensor[i].id = 0;
    }

    BHY2_ASSERT(bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT, parse_meta_event, parse_table, bhy2));
    BHY2_ASSERT(bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT_WU, parse_meta_event, parse_table, bhy2));
    BHY2_ASSERT(bhy2_register_fifo_parse_callback(BHY2_SYS_ID_DEBUG_MSG, parse_debug_message, parse_table, bhy2));
}

static void check_bhy2_api(unsigned int line, const char *func, int8_t val)
{
    ERROR("BHI260 API failed at line %u. The function %s returned error code %d. %s\r\n",
          line,
          func,
          val,
          get_api_error(val));
}

static void reset_hub(struct bhy2_dev *bhy2)
{
    uint8_t data = 0, data_exp;

    BHY2_ASSERT(bhy2_soft_reset(bhy2));

    BHY2_ASSERT(bhy2_get_host_interrupt_ctrl(&data, bhy2));
    data &= ~BHY2_ICTL_DISABLE_STATUS_FIFO; /* Enable status interrupts */
    data &= ~BHY2_ICTL_DISABLE_DEBUG; /* Enable debug interrupts */
    data &= ~BHY2_ICTL_EDGE; /* Level */
    data &= ~BHY2_ICTL_ACTIVE_LOW; /* Active high */
    data &= ~BHY2_ICTL_OPEN_DRAIN; /* Push-pull */
    data_exp = data;
    BHY2_ASSERT(bhy2_set_host_interrupt_ctrl(data, bhy2));
    BHY2_ASSERT(bhy2_get_host_interrupt_ctrl(&data, bhy2));
    if (data != data_exp)
    {
        WARNING("Expected Host Interrupt Control (0x07) to have value 0x%x but instead read 0x%x\r\n", data_exp, data);
    }

    /* Config status channel */
    BHY2_ASSERT(bhy2_set_host_intf_ctrl(BHY2_HIF_CTRL_ASYNC_STATUS_CHANNEL, bhy2));
    BHY2_ASSERT(bhy2_get_host_intf_ctrl(&data, bhy2));
    if (!(data & BHY2_HIF_CTRL_ASYNC_STATUS_CHANNEL))
    {
        WARNING("Expected Host Interface Control (0x06) to have bit 0x%x to be set\r\n",
                BHY2_HIF_CTRL_ASYNC_STATUS_CHANNEL);
    }

    PRINT("Reset successful\r\n");
}

static bool upload_to_ram(const char *filepath, struct bhy2_dev *bhy2)
{
    FILE *fw_file;
    struct stat st;
    uint8_t firmware_chunk[BHY2_RD_WR_LEN];
    uint32_t len;
    int8_t rslt = BHY2_OK;
    uint8_t boot_status;
    uint32_t start_time_ms;

#ifdef PC
    uint8_t progress = 0, new_progress = 0;
#endif
    BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        if ((boot_status & BHY2_BST_HOST_FW_VERIFY_DONE) || (boot_status & BHY2_BST_FLASH_VERIFY_DONE))
        {
            ERROR("Seems like a firmware is running. Please reset the BHI260/BHA260 before uploading firmware\r\n");

            return false;
        }

#ifdef PC
        fw_file = fopen(filepath, "rb"); /* Without the b, the file is read incorrectly */
#else
        fw_file = fopen(filepath, "r");
#endif

        if (!fw_file)
        {
            ERROR("Cannot open file: %s\r\n", filepath);

            return false;
        }

        stat(filepath, &st);
        len = st.st_size;

        /* 256 KB */
        if (len > 262144)
        {
            ERROR("Invalid RAM Size of %lu bytes\r\n", len);

            return false;
        }

        PRINT("Uploading %lu bytes of firmware to RAM\r\n", len);
        start_time_ms = coines_get_millis();
        uint32_t incr = BHY2_RD_WR_LEN;
        if ((incr % 4) != 0)
        {
            incr = BHY2_ROUND_WORD_LOWER(incr);
        }

        for (uint32_t i = 0; (i < len) && (rslt == BHY2_OK); i += incr)
        {
            if (incr > (len - i)) /* Last payload */
            {
                incr = len - i;
                if ((incr % 4) != 0) /* Round off to higher 4 bytes */
                {
                    incr = BHY2_ROUND_WORD_HIGHER(incr);
                }
            }

            fread(firmware_chunk, 1, incr, fw_file);
            rslt = bhy2_upload_firmware_to_ram_partly(firmware_chunk, len, i, incr, bhy2);
#ifdef PC
            progress = (float)(i + incr) / (float)len * 100.0f;
            if (progress != new_progress)
            {
                INFO("Completed %u %%\r", progress);
                new_progress = progress;
            }

#endif
        }

        INFO("Firmware upload took %.2f seconds\r\n", (float)(coines_get_millis() - start_time_ms) / 1000.0f);
        fclose(fw_file);

        if (rslt != BHY2_OK)
        {
            ERROR("Firmware upload failed. Returned with error code: %d. %s\r\n", rslt, get_api_error(rslt));

            return false;
        }
    }
    else
    {
        ERROR("Host interface is not ready\r\n");

        return false;
    }

    PRINT("Uploading firmware to RAM successful\r\n");

    return true;
}

static void print_boot_status(uint8_t boot_status)
{
    PRINT("Boot Status : 0x%02x: ", boot_status);
    if (boot_status & BHY2_BST_FLASH_DETECTED)
    {
        PRINT("Flash detected. ");
    }

    if (boot_status & BHY2_BST_FLASH_VERIFY_DONE)
    {
        PRINT("Flash verify done. ");
    }

    if (boot_status & BHY2_BST_FLASH_VERIFY_ERROR)
    {
        PRINT("Flash verification failed. ");
    }

    if (boot_status & BHY2_BST_NO_FLASH)
    {
        PRINT("No flash installed. ");
    }

    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        PRINT("Host interface ready. ");
    }

    if (boot_status & BHY2_BST_HOST_FW_VERIFY_DONE)
    {
        PRINT("Firmware verification done. ");
    }

    if (boot_status & BHY2_BST_HOST_FW_VERIFY_ERROR)
    {
        PRINT("Firmware verification error. ");
    }

    if (boot_status & BHY2_BST_HOST_FW_IDLE)
    {
        PRINT("Firmware halted. ");
    }

    PRINT("\r\n");
}

static void boot_ram(struct bhy2_dev *bhy2)
{
    int8_t rslt;
    uint8_t feat_status;
    uint8_t boot_status, error_val;
    uint16_t tries = 100;

    PRINT("Waiting for firmware verification to complete\r\n");
    do
    {
        bhy2->hif.delay_us(10000, NULL);
        BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
        if (boot_status & BHY2_BST_HOST_FW_VERIFY_DONE)
        {
            break;
        }
    } while (tries--);

    print_boot_status(boot_status);
    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        if (boot_status & BHY2_BST_HOST_FW_VERIFY_DONE)
        {
            INFO("Booting from RAM\r\n");
            rslt = bhy2_boot_from_ram(bhy2);
            if (rslt != BHY2_OK)
            {
                ERROR("Booting from RAM failed. API error code %d.\r\n%s\r\n", rslt, get_api_error(rslt));
                BHY2_ASSERT(bhy2_get_error_value(&error_val, bhy2));
                if (error_val)
                {
                    ERROR("Sensor reports error 0x%X.\r\n%s", error_val, get_sensor_error_text(error_val));
                }

                return;
            }

            BHY2_ASSERT(bhy2_get_feature_status(&feat_status, bhy2));
            if (feat_status & BHY2_FEAT_STATUS_OPEN_RTOS_MSK)
            {
                BHY2_ASSERT(bhy2_update_virtual_sensor_list(bhy2));

                for (uint16_t i = 0; i < 10; i++)
                {
                    /* Process meta events over a period of 100ms*/
                    BHY2_ASSERT(bhy2_get_and_process_fifo(fifo_buffer, sizeof(fifo_buffer), bhy2));
                    bhy2->hif.delay_us(10000, NULL);
                }
            }
            else
            {
                ERROR("Reading Feature status failed, booting from RAM failed\r\n");

                return;
            }
        }
        else
        {
            ERROR("Upload firmware to RAM before boot\r\n");

            return;
        }
    }
    else
    {
        ERROR("Host interface is not ready\r\n");

        return;
    }

    PRINT("Booting from RAM successful\r\n");
}

void show_info(struct bhy2_dev *bhy2)
{
    uint16_t kernel_version = 0, user_version = 0;
    uint16_t rom_version = 0;
    uint8_t product_id = 0;
    uint8_t host_status = 0, feat_status = 0;
    uint8_t val = 0;
    uint8_t sensor_error;
    struct bhy2_sensor_info info;

    /* Get product_id */
    BHY2_ASSERT(bhy2_get_product_id(&product_id, bhy2));

    /* Get Kernel version */
    BHY2_ASSERT(bhy2_get_kernel_version(&kernel_version, bhy2));

    /* Get User version */
    BHY2_ASSERT(bhy2_get_user_version(&user_version, bhy2));

    /* Get ROM version */
    BHY2_ASSERT(bhy2_get_rom_version(&rom_version, bhy2));

    BHY2_ASSERT(bhy2_get_host_status(&host_status, bhy2));

    BHY2_ASSERT(bhy2_get_feature_status(&feat_status, bhy2));

    PRINT("Product ID     : %02x\r\n", product_id);
    PRINT("Kernel version : %04u\r\n", kernel_version);
    PRINT("User version   : %04u\r\n", user_version);
    PRINT("ROM version    : %04u\r\n", rom_version);
    PRINT("Power state    : %s\r\n", (host_status & BHY2_HST_POWER_STATE) ? "sleeping" : "active");
    PRINT("Host interface : %s\r\n", (host_status & BHY2_HST_HOST_PROTOCOL) ? "SPI" : "I2C");
    PRINT("Feature status : 0x%02x\r\n", feat_status);

    /* Read boot status */
    BHY2_ASSERT(bhy2_get_boot_status(&val, bhy2));
    print_boot_status(val);

    /* Read error value */
    BHY2_ASSERT(bhy2_get_error_value(&sensor_error, bhy2));
    if (sensor_error)
    {
        ERROR("%s\r\n", get_sensor_error_text(sensor_error));
    }

    if (feat_status & BHY2_FEAT_STATUS_OPEN_RTOS_MSK)
    {
        bhy2_update_virtual_sensor_list(bhy2);

        /* Get present virtual sensor */
        bhy2_get_virt_sensor_list(bhy2);

        PRINT("Virtual sensor list.\r\n");
        PRINT("Sensor ID |                          Sensor Name |  ID | Ver |  Min rate |  Max rate |\r\n");
        PRINT("----------+--------------------------------------+-----+-----+-----------+-----------|\r\n");
        for (uint8_t i = 0; i < BHY2_SENSOR_ID_MAX; i++)
        {
            if (bhy2_is_sensor_available(i, bhy2))
            {
                if (i < BHY2_SENSOR_ID_CUSTOM_START)
                {
                    PRINT(" %8u | %36s ", i, get_sensor_name(i));
                }
                else
                {
                    PRINT(" %8u | %36s ", i, custom_driver_information[i - BHY2_SENSOR_ID_CUSTOM_START].sensor_name);
                }

                BHY2_ASSERT(bhy2_get_sensor_info(i, &info, bhy2));
                PRINT("| %3u | %3u | %9.4f | %9.4f |\r\n",
                      info.driver_id,
                      info.driver_version,
                      info.min_rate.f_val,
                      info.max_rate.f_val);
            }
        }
    }
}

static void boot_flash(struct bhy2_dev *bhy2)
{
    int8_t rslt;
    uint8_t boot_status, feat_status;
    uint8_t error_val = 0;
    uint16_t tries = 300; /* Wait for up to little over 3s */

    PRINT("Waiting for firmware verification to complete\r\n");
    do
    {
        bhy2->hif.delay_us(10000, NULL);
        BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
        if (boot_status & BHY2_BST_FLASH_VERIFY_DONE)
        {
            break;
        }
    } while (tries--);

    BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
    print_boot_status(boot_status);
    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        if (boot_status & BHY2_BST_FLASH_DETECTED)
        {

            /* If no firmware is running, boot from Flash */
            PRINT("Booting from flash\r\n");
            rslt = bhy2_boot_from_flash(bhy2);
            if (rslt != BHY2_OK)
            {
                ERROR("%s. Booting from flash failed.\r\n", get_api_error(rslt));
                BHY2_ASSERT(bhy2_get_regs(BHY2_REG_ERROR_VALUE, &error_val, 1, bhy2));
                if (error_val)
                {
                    ERROR("%s\r\n", get_sensor_error_text(error_val));
                }

                return;
            }

            BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
            print_boot_status(boot_status);

            if (!(boot_status & BHY2_BST_HOST_INTERFACE_READY))
            {
                /* hub is not ready, need reset hub */
                PRINT("Host interface is not ready, triggering a reset\r\n");

                BHY2_ASSERT(bhy2_soft_reset(bhy2));
            }

            BHY2_ASSERT(bhy2_get_feature_status(&feat_status, bhy2));
            if (feat_status & BHY2_FEAT_STATUS_OPEN_RTOS_MSK)
            {
                BHY2_ASSERT(bhy2_update_virtual_sensor_list(bhy2));
                for (uint16_t i = 0; i < 10; i++) /* Process meta events */
                {
                    BHY2_ASSERT(bhy2_get_and_process_fifo(fifo_buffer, sizeof(fifo_buffer), bhy2));
                    bhy2->hif.delay_us(10000, NULL);
                }
            }
            else
            {
                ERROR("Reading Feature status failed, booting from flash failed\r\n");

                return;
            }
        }
        else
        {
            ERROR("Can't detect external flash\r\n");

            return;
        }
    }
    else
    {
        ERROR("Host interface is not ready\r\n");

        return;
    }

    PRINT("Booting from flash successful\r\n");
}

static bool upload_to_flash(const char *filepath, struct bhy2_dev *bhy2)
{
    FILE *fw_file;
    struct stat st;
    uint8_t firmware_chunk[BHY2_RD_WR_LEN];
    uint32_t len;
    int8_t rslt = BHY2_OK;
    uint8_t boot_status;
    uint32_t start_time_ms;

#ifdef PC
    uint8_t progress = 0, new_progress = 0;
#endif
    BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        if ((boot_status & BHY2_BST_HOST_FW_VERIFY_DONE) || (boot_status & BHY2_BST_FLASH_VERIFY_DONE))
        {
            ERROR("Reset the BHI260/BHA260 before uploading firmware to external flash\r\n");

            return false;
        }

#ifdef PC
        fw_file = fopen(filepath, "rb"); /* Without the b, the file is read incorrectly */
#else
        fw_file = fopen(filepath, "r");
#endif
        if (!fw_file)
        {
            ERROR("Cannot open file: %s\r\n", filepath);

            return false;
        }

        stat(filepath, &st);
        len = st.st_size;

        /* 8 MB */
        if (len > 8388608)
        {
            ERROR("Invalid firmware size of %lu bytes\r\n", len);

            return false;
        }

        PRINT("Erasing first %lu bytes of flash\r\n", len);
        rslt = bhy2_erase_flash(BHY2_FLASH_SECTOR_START_ADDR, BHY2_FLASH_SECTOR_START_ADDR + len, bhy2);
        if (rslt != BHY2_OK)
        {
            ERROR("Erasing flash failed with error code %d. %s\r\n", rslt, get_api_error(rslt));
        }

        PRINT("Uploading %lu bytes of firmware to flash\r\n", len);
        start_time_ms = coines_get_millis();
        uint32_t incr = BHY2_RD_WR_LEN;
        if ((incr % 4) != 0) /* Round of to lower 4 bytes */
        {
            incr = BHY2_ROUND_WORD_LOWER(incr);
        }

        for (uint32_t i = 0; (i < len) && (rslt == BHY2_OK); i += incr)
        {
            if (incr > (len - i)) /* Last payload */
            {
                incr = len - i;
                if ((incr % 4) != 0) /* Round of to higher 4bytes */
                {
                    incr = BHY2_ROUND_WORD_HIGHER(incr);
                }
            }

            fread(firmware_chunk, 1, incr, fw_file);
            rslt = bhy2_upload_firmware_to_flash_partly(firmware_chunk, i, incr, bhy2);
#ifdef PC
            progress = (float)(i + incr) / (float)len * 100.0f;
            if (progress != new_progress)
            {
                INFO("Completed %u %%\r", progress);
                new_progress = progress;
            }

#endif
        }

        INFO("Firmware upload took %.2f seconds\r\n", (float)(coines_get_millis() - start_time_ms) / 1000.0f);
        fclose(fw_file);
        if (rslt != BHY2_OK)
        {
            ERROR("%s. Firmware upload failed\r\n", get_api_error(rslt));

            return false;
        }
    }
    else
    {
        ERROR("Host interface is not ready\r\n");
    }

    PRINT("Uploading firmware to flash successful\r\n");

    return true;
}

static void wr_regs(const char *payload, struct bhy2_dev *bhy2)
{
    char *start;
    char *end;
    char str_reg[8] = { 0 };
    uint8_t reg;
    uint8_t val[1024] = { 0 };
    char *strtok_ptr;
    char *byte_delimiter = ",";
    uint16_t len = 0;

    /* Parse register address */
    start = (char *)payload;
    end = strchr(start, '=');
    if (end == NULL)
    {
        ERROR("Write address format error\r\n");

        return;
    }

    strncpy(str_reg, start, (size_t)(end - start));
    str_reg[end - start] = '\0';
    reg = (uint8_t)strtol(str_reg, NULL, 0);

    /* Parse values to be written */
    start = end + 1;
    strtok_ptr = strtok(start, byte_delimiter);

    while (strtok_ptr != NULL)
    {
        val[len] = (uint8_t)strtol(strtok_ptr, NULL, 0);
        strtok_ptr = strtok(NULL, byte_delimiter);
        ++len;
    }

    /* Execution of bus write function */
    BHY2_ASSERT(bhy2_set_regs(reg, val, len, bhy2));

    PRINT("Writing address successful\r\n");
}

static void rd_regs(const char *payload, struct bhy2_dev *bhy2)
{
    char *start;
    char *end;
    char str_reg[8] = { 0 };
    uint8_t reg;
    uint8_t val[1024];
    uint16_t len;
    uint16_t i = 0;
    uint16_t j = 0;

    start = (char *)payload;
    end = strchr(start, ':');
    if (end == NULL)
    {
        end = start + strlen(start);
    }

    /* Parse register address */
    strncpy(str_reg, start, (size_t)(end - start));
    str_reg[end - start] = '\0';
    reg = (uint8_t)strtol(str_reg, NULL, 0);

    /* Parse read length, rest of the string */
    start = end + 1;
    len = (uint16_t)strtol(start, NULL, 0);

    /* Default read length is 1 */
    if (len < 1)
    {
        len = 1;
    }

    /* Execution of bus read function */
    BHY2_ASSERT(bhy2_get_regs(reg, val, len, bhy2));

    /* Print register data to console */

    /* Registers after the status channel are auto increment,
     * reading more than 1 byte will lead to reading
     * the subsequent register addresses */
    if (reg <= BHY2_REG_CHAN_STATUS)
    {
        PRINT("Reading from register address 0x%02x:\r\n", reg);
        PRINT("Byte hex       dec | Data\r\n");
        PRINT("-------------------------------------------\r\n");
        for (i = 0; i < len; i++)
        {
            if (j == 0)
            {
                PRINT(" ");
                PRINT("0x%06x %8d |", i, i);
            }

            PRINT(" %02x", val[i]);
            ++j;
            if (j >= 8)
            {
                PRINT("\r\n");
                j = 0;
            }
        }

        if ((len % 8) == 0)
        {
            PRINT("\r\n");
        }
    }
    else
    {
        PRINT("Register address: Data\r\n");
        PRINT("----------------------\r\n");
        for (i = 0; i < len; i++)
        {
            PRINT("0x%02x            : %02x \r\n", reg + i, val[i]);
        }
    }

    PRINT("Read complete\r\n");
}

void rd_param(const char *payload, struct bhy2_dev *bhy2)
{
    char str_param_id[8] = { 0 };
    uint8_t tmp_buf[1024] = { 0 };
    uint16_t param_id;
    uint32_t ret_len = 0;
    uint16_t i;
    uint16_t j = 0;

    strncpy(str_param_id, payload, strlen(payload));
    str_param_id[strlen(payload)] = '\0';
    param_id = (uint16_t)strtol(str_param_id, NULL, 0);

    BHY2_ASSERT(bhy2_get_parameter(param_id, tmp_buf, sizeof(tmp_buf), &ret_len, bhy2));
    if (assert_rslt != BHY2_OK)
    {
        return;
    }

    PRINT("Byte hex      dec | Data\r\n");
    PRINT("-------------------------------------------\r\n");
    for (i = 0; i < ret_len; i++)
    {
        if (j == 0)
        {
            PRINT("0x%06x %8d |", i, i);
        }

        PRINT("%02x ", tmp_buf[i]);
        j++;
        if (j >= 8)
        {
            PRINT("\r\n");
            j = 0;
        }
    }

    if ((ret_len % 8) != 0)
    {
        PRINT("\r\n");
    }

    PRINT("Reading parameter 0x%04X successful\r\n", param_id);
}

static void wr_param(const char *payload, struct bhy2_dev *bhy2)
{
    char *start, *end;
    char str_param_id[8] = { 0 };
    char str_data[8] = { 0 };
    uint8_t data_buf[1024] = { 0 };
    uint16_t param_id;
    uint8_t val;
    uint16_t buf_size = 0;
    uint8_t break_out = 0;

    start = (char *)payload;
    end = strchr(start, '=');
    if (end == NULL)
    {
        ERROR("Write parameter I/O format error\r\n");

        return;
    }

    strncpy(str_param_id, start, (size_t)(end - start));
    str_param_id[end - start] = '\0';
    param_id = (uint16_t)strtol(str_param_id, NULL, 0);

    /* Parse write data */
    do
    {
        start = end + 1;
        end = strchr(start, ',');
        if (end == NULL)
        {
            end = start + strlen(start);
            break_out++;
        }

        strncpy(str_data, start, (size_t)(end - start));
        str_data[end - start] = '\0';
        val = (uint8_t)strtol(str_data, NULL, 0);
        data_buf[buf_size] = val;
        buf_size++;
    } while (!break_out);

    /* Make sure write buffer size is always multiples of 4 */
    if (buf_size % 4)
    {
        buf_size = (uint16_t)((buf_size / 4 + 1) * 4);
    }

    BHY2_ASSERT(bhy2_set_parameter(param_id, data_buf, buf_size, bhy2));
    PRINT("Writing parameter successful\r\n");
}

static void erase_flash(uint32_t end_addr, struct bhy2_dev *bhy2)
{
    int8_t rslt;
    uint8_t boot_status;

    BHY2_ASSERT(bhy2_get_boot_status(&boot_status, bhy2));
    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        if ((boot_status & BHY2_BST_HOST_FW_VERIFY_DONE) || (boot_status & BHY2_BST_FLASH_VERIFY_DONE))
        {
            ERROR("Seems like a firmware is running. Reset the BHI260/BHA260 before erasing external flash\r\n");

            return;
        }
    }

    PRINT("Erasing flash. May take a while\r\n");
    rslt = bhy2_erase_flash(BHY2_FLASH_SECTOR_START_ADDR, BHY2_FLASH_SECTOR_START_ADDR + end_addr, bhy2);
    if (rslt != BHY2_OK)
    {
        ERROR("Erasing flash failed, status: %02d\r\n", rslt);

        return;
    }

    PRINT("Erasing flash successful\r\n");
}

static void activate_sensor(const char *sensor_parameters, uint8_t parse_flag, struct bhy2_cli_ref *ref)
{
    struct bhy2_dev *bhy2 = &(ref->bhy2);
    struct parse_ref *parse_table = &(ref->parse_table);
    bhy2_fifo_parse_callback_t callback;
    struct parse_sensor_details *sensor_details;

    char sen_id_str[8], sample_rate_str[8], sen_latency_str[8];
    uint8_t sen_id;
    uint32_t sen_latency = 0;
    float sample_rate;
    char *start, *end;

    /* Parse Sensor ID */
    start = (char *)sensor_parameters;
    end = strchr(start, ':');
    if (end == NULL)
    {
        ERROR("Sensor ID / Sample rate format error\r\n");

        return;
    }

    strncpy(sen_id_str, start, (size_t)(end - start));
    sen_id_str[end - start] = '\0';
    sen_id = (uint8_t)atoi(sen_id_str);

    /* Parse sample rate */
    start = end + 1;
    end = strchr(start, ':');

    if (end == NULL)
    {
        end = start + strlen(start);
    }

    strncpy(sample_rate_str, start, (size_t)(end - start));
    sample_rate_str[end - start] = '\0';
    sample_rate = (float)atof(sample_rate_str);

    if (sample_rate < 0)
    {
        sample_rate = 0.0f;
    }

    /*  Parse Latency */
    if (strlen(end))
    {
        start = end + 1;
        end = strchr(start, ':');

        if (end == NULL)
        {
            end = start + strlen(start);
        }

        strncpy(sen_latency_str, start, (size_t)(end - start));
        sen_latency_str[end - start] = '\0';
        sen_latency = (uint32_t)atoi(sen_latency_str);
    }

    if (first_run)
    {
        first_run = false;
        bhy2_update_virtual_sensor_list(bhy2);

        /* Get present virtual sensor */
        bhy2_get_virt_sensor_list(bhy2);
    }

    /* If the payload of this sensor is not yet registered and within the custom virtual sensor id range, register the
     * default parsing function */
    if (bhy2_is_sensor_available(sen_id, bhy2))
    {
        if ((sen_id >= BHY2_SENSOR_ID_CUSTOM_START) && (sen_id <= BHY2_SENSOR_ID_CUSTOM_END) &&
            (custom_driver_information[sen_id - BHY2_SENSOR_ID_CUSTOM_START].is_registered != 1))
        {
            custom_driver_information[sen_id - BHY2_SENSOR_ID_CUSTOM_START].sensor_payload = bhy2->event_size[sen_id];

            BHY2_ASSERT(bhy2_register_fifo_parse_callback(sen_id, parse_custom_sensor_default, parse_table, bhy2));
            PRINT("No output interpretation has been provided for this sensor. ");
            PRINT("FIFO data will be printed as hex values. ");
            PRINT("For registering the payload interpretation, use the addse option\r\n");
        }
    }
    else
    {
        ERROR("The requested sensor is not present in the loaded firmware!\r\n");

        return;
    }

    INFO("Sensor ID: %u, sample rate: %f Hz, latency: %lu ms\r\n", sen_id, sample_rate, sen_latency);

    sensor_details = parse_add_sensor_details(sen_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Insufficient parsing slots\r\n");

        return;
    }

    /* If through the logging command, check for valid sample rate before enabling logging */
    if (sample_rate > 0.0f)
    {
        /* Register if not already requested earlier */
        if (sensor_details->parse_flag == PARSE_FLAG_NONE)
        {
            callback = bhy2_get_callback(sen_id);
            BHY2_ASSERT(bhy2_register_fifo_parse_callback(sen_id, callback, parse_table, bhy2));
        }

        sensor_details->parse_flag = PARSE_SET_FLAG(sensor_details->parse_flag, parse_flag);
        sensor_details->scaling_factor = get_sensor_default_scaling(sen_id);
        sensors_active[sen_id] = true;

        /* Flush sensor data from the FIFO */
        BHY2_ASSERT(bhy2_flush_fifo(sen_id, bhy2));

        /* Enable sensor and set sample rate. Disable if there is no source requesting it */
        if ((sample_rate > 0.0f) || (sensor_details->parse_flag == PARSE_FLAG_NONE))
        {
            BHY2_ASSERT(bhy2_set_virt_sensor_cfg(sen_id, sample_rate, sen_latency, bhy2));
        }
    }
    else
    {
        /* Flush sensor data from the FIFO */
        BHY2_ASSERT(bhy2_flush_fifo(sen_id, bhy2));

        /* Enable sensor and set sample rate. Disable if there is no source requesting it */
        if ((sample_rate > 0.0f) || (sensor_details->parse_flag == PARSE_FLAG_NONE))
        {
            BHY2_ASSERT(bhy2_set_virt_sensor_cfg(sen_id, sample_rate, sen_latency, bhy2));
        }

        sensor_details->parse_flag = PARSE_CLEAR_FLAG(sensor_details->parse_flag, parse_flag);

        /* Disable if there is no source requesting it */
        if (sensor_details->parse_flag == PARSE_FLAG_NONE)
        {
            sensors_active[sen_id] = false;
            BHY2_ASSERT(bhy2_deregister_fifo_parse_callback(sen_id, bhy2));
            sensor_details->id = 0;
        }
    }

    /* Sensor data will be parsed and printed after processing all arguments */
}

static void parse_custom_sensor_default(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint8_t this_sensor_id;
    uint8_t this_sensor_payload;
    uint8_t parse_flag;
    uint32_t s, ns;
    uint64_t tns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    struct parse_sensor_details *sensor_details;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;

    this_sensor_id = callback_info->sensor_id;
    this_sensor_payload =
        (uint8_t)custom_driver_information[this_sensor_id - BHY2_SENSOR_ID_CUSTOM_START].sensor_payload;

    if (this_sensor_payload > callback_info->data_size)
    {
        ERROR("Mismatch in payload size\r\n");

        return;
    }

    time_to_s_ns(*callback_info->time_stamp, &s, &ns, &tns);

    if (parse_flag & PARSE_FLAG_STREAM)
    {
        /* Print sensor ID */
        DATA("SID: %u; T: %lu.%09lu; ", this_sensor_id, s, ns);

        for (uint16_t i = 0; i < this_sensor_payload - 1; i++)
        {
            /* Output raw data in hex */
            PRINT_D("%x ", callback_info->data_ptr[i]);
        }

        PRINT_D("\r\n");
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 tns,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }
}

static void parse_custom_sensor(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint8_t idx;
    char *strtok_ptr;
    char *parameter_delimiter = ":";
    char tmp_output_formats[BHY2CLI_MAX_STRING_LENGTH];
    uint8_t rel_sensor_id; /* Relative sensor ID  */
    uint32_t s, ns;
    struct parse_ref *parse_table = (struct parse_ref *)callback_ref;
    uint8_t parse_flag;
    struct parse_sensor_details *sensor_details;

    union
    {
        uint32_t data_u32;
        float data_float;
    }
    u32_to_float;

    uint8_t tmp_u8 = 0;
    uint16_t tmp_u16 = 0;
    uint32_t tmp_u32 = 0;
    int8_t tmp_s8 = 0;
    int16_t tmp_s16 = 0;
    int32_t tmp_s32 = 0;
    uint8_t tmp_data_c = 0;

    if (!parse_table || !callback_info)
    {
        ERROR("Null reference\r\r\n");

        return;
    }

    sensor_details = parse_get_sensor_details(callback_info->sensor_id, parse_table);
    if (!sensor_details)
    {
        ERROR("Parse slot not defined\r\n");

        return;
    }

    parse_flag = sensor_details->parse_flag;

    /* Get sensor_id to access correct parsing information from global linked list  */
    rel_sensor_id = callback_info->sensor_id - BHY2_SENSOR_ID_CUSTOM_START;

    /* Fetch output_formats string from linked list */
    strcpy(tmp_output_formats, custom_driver_information[rel_sensor_id].output_formats);
    strtok_ptr = strtok(tmp_output_formats, parameter_delimiter);

    uint64_t timestamp = *callback_info->time_stamp; /* Store the last timestamp */

    timestamp = timestamp * 15625; /* Timestamp is now in nanoseconds */
    s = (uint32_t)(timestamp / UINT64_C(1000000000));
    ns = (uint32_t)(timestamp - (s * UINT64_C(1000000000)));

    if ((custom_driver_information[rel_sensor_id].sensor_payload + 1) != callback_info->data_size)
    {
        ERROR("Mismatch in payload size\r\n");

        return;
    }

    idx = 0;
    if (parse_flag & PARSE_FLAG_STREAM)
    {
        /* Print sensor id and timestamp */
        DATA("%u; %lu.%09lu; ", callback_info->sensor_id, s, ns);

        /* Parse output_formats and output data depending on the individual format of an output */

        while (strtok_ptr != NULL)
        {

            if (strcmp(strtok_ptr, "u8") == 0)
            {
                tmp_u8 = callback_info->data_ptr[idx];
                idx += 1;

                PRINT_D("%u ", tmp_u8);
            }
            else if (strcmp(strtok_ptr, "u16") == 0)
            {
                tmp_u16 = BHY2_LE2U16(&callback_info->data_ptr[idx]);
                idx += 2;

                PRINT_D("%u ", tmp_u16);
            }
            else if (strcmp(strtok_ptr, "u32") == 0)
            {
                tmp_u32 = BHY2_LE2U32(&callback_info->data_ptr[idx]);
                idx += 4;

                PRINT_D("%lu ", tmp_u32);
            }
            else if (strcmp(strtok_ptr, "s8") == 0)
            {
                tmp_s8 = (int8_t)callback_info->data_ptr[idx];
                idx += 1;

                PRINT_D("%d ", tmp_s8);
            }
            else if (strcmp(strtok_ptr, "s16") == 0)
            {
                tmp_s16 = BHY2_LE2S16(&callback_info->data_ptr[idx]);
                idx += 2;

                PRINT_D("%d ", tmp_s16);
            }
            else if (strcmp(strtok_ptr, "s32") == 0)
            {
                tmp_s32 = BHY2_LE2S32(&callback_info->data_ptr[idx]);
                idx += 4;

                PRINT_D("%ld ", tmp_s32);
            }
            else if (strcmp(strtok_ptr, "c") == 0)
            {
                tmp_data_c = callback_info->data_ptr[idx];
                idx += 1;

                PRINT_D("%c ", tmp_data_c);
            }
            else if (strcmp(strtok_ptr, "f") == 0)
            {
                /* Float values have to be read as unsigned and then interpreted as float */
                u32_to_float.data_u32 = BHY2_LE2U32(&callback_info->data_ptr[idx]);
                idx += 4;

                /* The binary data has to be interpreted as a float */
                PRINT_D("%6.4f ", u32_to_float.data_float);
            }

            strtok_ptr = strtok(NULL, parameter_delimiter);
        }

        PRINT_D("\r\n");
    }

    if (parse_flag & PARSE_FLAG_LOG)
    {
        log_data(callback_info->sensor_id,
                 timestamp,
                 callback_info->data_size - 1,
                 callback_info->data_ptr,
                 &parse_table->logdev);
    }

    if (idx != custom_driver_information[rel_sensor_id].sensor_payload)
    {
        ERROR("Provided Output format sizes don't add up to total sensor payload!\r\n");

        return;
    }
}

static void add_sensor(const char *payload, struct bhy2_cli_ref *cli_ref)
{
    struct bhy2_dev *bhy2 = &cli_ref->bhy2;
    struct parse_ref *parse_table = &cli_ref->parse_table;
    char *start;
    char *end;
    char str_sensor_id[BHY2CLI_MAX_STRING_LENGTH];
    char str_sensor_payload[BHY2CLI_MAX_STRING_LENGTH];
    char output_formats[BHY2CLI_MAX_STRING_LENGTH];
    char sensor_name[BHY2CLI_MAX_STRING_LENGTH];
    uint8_t sensor_id;
    uint8_t sensor_payload;
    uint8_t len_of_output_formats;
    struct bhy2_sensor_info sensor_info;

    start = (char *)payload;
    end = strchr(start, ':');

    if (end == NULL)
    {
        ERROR("Add Sensor format error\r\n");

        return;
    }

    /* Parse sensor ID */

    /* Check length of string */
    if (((uint32_t)(end - start)) > BHY2CLI_MAX_STRING_LENGTH)
    {
        ERROR("Too many characters for sensor ID!\r\n");

        return;
    }

    strncpy(str_sensor_id, start, (size_t)(end - start));
    str_sensor_id[end - start] = '\0';

    /* Convert string to int */
    sensor_id = (uint8_t)strtol(str_sensor_id, NULL, 10);
    INFO("Sensor ID: %u \r\n", sensor_id);

    /* Parse sensor name */
    start = end + 1;
    end = strchr(start, ':');
    if (end == NULL)
    {
        ERROR("Add Sensor name error\r\n");

        return;
    }

    /* Check length of string */
    if (((uint32_t)(end - start)) > BHY2CLI_MAX_STRING_LENGTH)
    {
        ERROR("Too many characters for sensor name. Only %u characters allowed\r\n", BHY2CLI_MAX_STRING_LENGTH);

        return;
    }

    strncpy(sensor_name, start, (size_t)(end - start));
    sensor_name[end - start] = '\0';
    INFO("Sensor Name: %s \r\n", sensor_name);

    /* Parse sensor payload */
    start = end + 1;
    end = strchr(start, ':');

    if (end == NULL)
    {
        ERROR("Add Sensor payload error\r\n");

        return;
    }

    strncpy(str_sensor_payload, start, (size_t)(end - start));
    str_sensor_payload[end - start] = '\0';
    sensor_payload = (uint8_t)strtol(str_sensor_payload, NULL, 10);
    INFO("Sensor Payload: %u \r\n", sensor_payload);

    /* Parse output formats string, final parsing of each output is done in the parsing callback function */
    start = end + 1;
    len_of_output_formats = (uint8_t)strlen(start);
    end = start + len_of_output_formats;
    strncpy(output_formats, start, (size_t)((size_t)(end - start)));
    output_formats[end - start] = '\0';

    /* Get the sensor information */
    BHY2_ASSERT(bhy2_get_sensor_info(sensor_id, &sensor_info, bhy2));

    /* Check if supplied payload matches the event size. Note event size includes sensor id in the payload */
    if (sensor_info.event_size != (sensor_payload + 1))
    {
        ERROR("Provided total payload size of sensor ID %u doesn't match the actual payload size!\r\n", sensor_id);

        return;
    }

    /* Store parsed data into the custom driver information table */
    custom_driver_information[sensor_id - BHY2_SENSOR_ID_CUSTOM_START].sensor_id = sensor_id;
    custom_driver_information[sensor_id - BHY2_SENSOR_ID_CUSTOM_START].sensor_payload = sensor_payload;
    strncpy(custom_driver_information[sensor_id - BHY2_SENSOR_ID_CUSTOM_START].output_formats,
            output_formats,
            BHY2CLI_MAX_STRING_LENGTH);
    custom_driver_information[sensor_id - BHY2_SENSOR_ID_CUSTOM_START].is_registered = 1;
    strncpy(custom_driver_information[sensor_id - BHY2_SENSOR_ID_CUSTOM_START].sensor_name,
            sensor_name,
            BHY2CLI_MAX_STRING_LENGTH);

    /* Register the custom sensor callback function*/
    BHY2_ASSERT(bhy2_register_fifo_parse_callback(sensor_id, parse_custom_sensor, parse_table, bhy2));

    PRINT("Adding custom driver payload successful\r\n");
}

static void klio_enable(struct bhy2_dev *bhy2)
{
    if (!klio_enabled)
    {
        uint8_t buf[255];
        uint16_t size = sizeof(buf);
        uint32_t klio_driver_status;

        int major = -1;
        int minor = -1;
        int version = -1;
        int count = 0;

        BHY2_ASSERT(bhy2_klio_get_parameter(KLIO_PARAM_ALGORITHM_VERSION, buf, &size, bhy2));
        BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));

        if (size > 0)
        {
            count = sscanf((char *)buf, "%d.%d.%d", &major, &minor, &version);
        }

        if (major < 0 || minor < 0 || version < 0 || count != 3)
        {
            PRINT("Unable to get Klio firmware version.\r\n");
            BHY2_ASSERT(BHY2_E_MAGIC); /* Invalid firmware error */
        }

        if (major != 3)
        {
            PRINT("The supported Klio firmware is version 3.x.x.\r\n");
            BHY2_ASSERT(BHY2_E_MAGIC); /* Invalid firmware error */
        }

        BHY2_ASSERT(bhy2_klio_get_parameter(KLIO_PARAM_RECOGNITION_MAX_PATTERNS, buf, &size, bhy2));
        BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));

        memcpy(&klio_vars.max_patterns, buf, 2);

        size = sizeof(buf);

        BHY2_ASSERT(bhy2_klio_get_parameter(KLIO_PARAM_PATTERN_BLOB_SIZE, buf, &size, bhy2));
        BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));

        memcpy(&klio_vars.max_pattern_blob_size, buf, 2);

        klio_vars.auto_load_pattern_write_index = 0;

        klio_enabled = true;
    }
}

static void klio_status(struct bhy2_dev *bhy2)
{
    uint32_t klio_driver_status;

    BHY2_ASSERT(bhy2_klio_read_reset_driver_status(&klio_driver_status, bhy2));
    INFO("[kstatus] ");
    PRINT("Status: %lu\r\n", klio_driver_status);
}

static void klio_set_state(const char *arg1, const char *arg2, const char *arg3, const char *arg4,
                           struct bhy2_dev *bhy2)
{
    bhy2_klio_sensor_state_t state =
    { .learning_enabled = atoi(arg1), .learning_reset = atoi(arg2), .recognition_enabled = atoi(arg3),
      .recognition_reset = atoi(arg4) };

    INFO("[ksetstate] ");
    PRINT("Learning enabled    : %u\r\n", state.learning_enabled);
    INFO("[ksetstate] ");
    PRINT("Learning reset      : %u\r\n", state.learning_reset);
    INFO("[ksetstate] ");
    PRINT("Recognition enabled : %u\r\n", state.recognition_enabled);
    INFO("[ksetstate] ");
    PRINT("Recognition reset   : %u\r\n", state.recognition_reset);
    BHY2_ASSERT(bhy2_klio_set_state(&state, bhy2));
}

static void klio_get_state(struct bhy2_dev *bhy2)
{
    bhy2_klio_sensor_state_t state;

    BHY2_ASSERT(bhy2_klio_get_state(&state, bhy2));
    INFO("[kgetstate] ");
    PRINT("Learning enabled    : %u\r\n", state.learning_enabled);
    INFO("[kgetstate] ");
    PRINT("Learning reset      : %u\r\n", state.learning_reset);
    INFO("[kgetstate] ");
    PRINT("Recognition enabled : %u\r\n", state.recognition_enabled);
    INFO("[kgetstate] ");
    PRINT("Recognition reset   : %u\r\n", state.recognition_reset);
}

static int32_t hex_to_char(char c)
{
    c = tolower(c);

    if (c >= '0' && c <= '9')
    {
        c -= '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        c -= 'a' - 10;
    }
    else
    {
        return -1;
    }

    return c;
}

static int32_t pattern_blob_to_bytes(const uint8_t *pattern_blob_char, uint8_t *pattern_blob)
{
    size_t length = strlen((char *)pattern_blob_char);

    for (int i = 0; i < length; i += 2)
    {
        int32_t u = hex_to_char(pattern_blob_char[i]);
        int32_t l = hex_to_char(pattern_blob_char[i + 1]);

        if (u < 0 || l < 0)
        {
            return -1;
        }

        pattern_blob[i / 2] = ((char)u << 4) | (char)l;
    }

    return length / 2;
}

static void klio_load_pattern(const char *arg1, const char *arg2, struct bhy2_dev *bhy2)
{
    uint8_t pattern_data[244];
    uint16_t size = pattern_blob_to_bytes((uint8_t *)arg2, (uint8_t *)pattern_data);
    int index = atoi(arg1);

    if (index < klio_vars.max_patterns)
    {
        BHY2_ASSERT(bhy2_klio_write_pattern(index, pattern_data, size, bhy2));

        if (index >= klio_vars.auto_load_pattern_write_index)
        {
            klio_vars.auto_load_pattern_write_index = index + 1;
        }
    }
    else
    {
        INFO("[kldpatt] ");
        ERROR("Pattern index: %d >= Max patterns %d\r\n", index, klio_vars.max_patterns);
        kldpatt_help(NULL);

        return;
    }
}

static void klio_get_parameter(const uint8_t *arg, struct bhy2_dev *bhy2)
{
    int param_id = atoi((char *)arg);
    uint8_t buf[255];
    uint16_t size = sizeof(buf);
    float similarity = 0.0f;
    uint16_t max_patterns = 0;

    BHY2_ASSERT(bhy2_klio_get_parameter(param_id, buf, &size, bhy2));

    switch (param_id)
    {
        case KLIO_PARAM_ALGORITHM_VERSION:
            INFO("[kgetparam] ");
            buf[sizeof(buf) - 1] = '\0';
            PRINT("Parameter %d: %s\r\n", param_id, buf);
            break;
        case KLIO_PARAM_RECOGNITION_RESPONSIVNESS:
        case KLIO_PARAM_LEARNING_SIMILARITY_THRESHOLD:
            INFO("[kgetparam] ");
            memcpy(&similarity, buf, 4);
            PRINT("Parameter %d: %f\r\n", param_id, similarity);
            break;
        case KLIO_PARAM_PATTERN_BLOB_SIZE:
        case KLIO_PARAM_RECOGNITION_MAX_PATTERNS:
            INFO("[kgetparam] ");
            memcpy(&max_patterns, buf, 2);
            PRINT("Parameter %d: %u\r\n", param_id, max_patterns);
            break;
        case KLIO_PARAM_LEARNING_IGNORE_INSIG_MOVEMENT:
            INFO("[kgetparam] ");
            PRINT("Parameter %d: %u\r\n", param_id, buf[0]);
            break;
        default:
            for (uint16_t q = 0; q < size; q++)
            {
                if (q % 16 == 0)
                {
                    INFO("[kgetparam] ");
                    PRINT("Parameter %d: ", param_id);
                }

                PRINT("%02x ", buf[q]);
                if (q % 16 == 15 || q == size - 1)
                {
                    PRINT("\r\n");
                }
            }

            break;
    }
}

static void klio_set_parameter(const char *arg1, char *arg2, struct bhy2_dev *bhy2)
{
    int param_id = atoi(arg1);
    char *param_value = arg2;
    float cycle_count;
    uint8_t ignore_insig_movement;

    switch (param_id)
    {
        case KLIO_PARAM_RECOGNITION_RESPONSIVNESS:
            cycle_count = atof(param_value);
            BHY2_ASSERT(bhy2_klio_set_parameter(param_id, &cycle_count, sizeof(cycle_count), bhy2));
            break;
        case KLIO_PARAM_LEARNING_IGNORE_INSIG_MOVEMENT:
            ignore_insig_movement = atoi(param_value);
            BHY2_ASSERT(bhy2_klio_set_parameter(param_id, &ignore_insig_movement, sizeof(ignore_insig_movement), bhy2));
            break;
        default:
            break;
    }
}

static void klio_similarity_score(const uint8_t *arg1, const uint8_t *arg2, struct bhy2_dev *bhy2)
{
    uint8_t first_pattern_data[244] = { 0 }, second_pattern_data[244] = { 0 };
    uint16_t pattern1_size = pattern_blob_to_bytes(arg1, first_pattern_data);
    uint16_t pattern2_size = pattern_blob_to_bytes(arg2, second_pattern_data);

    if (pattern1_size != pattern2_size)
    {
        INFO("[ksimscore] ");
        ERROR("Patterns for similarity calculation differ in size\r\n");
    }
    else
    {
        float similarity;
        BHY2_ASSERT(bhy2_klio_similarity_score(first_pattern_data, second_pattern_data, pattern1_size, &similarity,
                                               bhy2));
        INFO("[ksimscore] ");
        PRINT("Similarity: %f\r\n", similarity);
    }
}

static void klio_similarity_score_multiple(const char *arg1, const char *arg2, struct bhy2_dev *bhy2)
{
    uint8_t index = atoi(arg1);
    char *indexes_str = strdup(arg2), *strtok_ptr, *strtok_saveptr;
    uint8_t indexes[klio_vars.max_patterns];
    uint8_t count = 0;
    float similarity[klio_vars.max_patterns];

    strtok_ptr = strtok_r(indexes_str, ",", &strtok_saveptr);
    while (strtok_ptr != NULL)
    {
        indexes[count] = atoi(strtok_ptr);
        count++;
        strtok_ptr = strtok_r(NULL, ",", &strtok_saveptr);
    }

    BHY2_ASSERT(bhy2_klio_similarity_score_multiple(index, indexes, count, similarity, bhy2));

    INFO("[kmsimscore] ");
    PRINT("Using pattern id %d as reference: ", index);
    for (uint32_t i = 0; i < count; i++)
    {
        PRINT("%d:%6f ", indexes[i], similarity[i]);
    }

    PRINT("\r\n");

    free(indexes_str);
}

static void klio_pattern_state_operation(const uint8_t operation, const char *arg1, struct bhy2_dev *bhy2)
{
    uint8_t count = 0;
    uint8_t pattern_states[klio_vars.max_patterns];

    char *sep = ",";
    char *str = strdup(arg1);

    if (str == NULL)
    {
        kenpatt_help(NULL);
        kdispatt_help(NULL);

        return;
    }

    char *token = strtok(str, sep);

    while (token != NULL)
    {
        int32_t index = atoi(token);

        if (index < klio_vars.max_patterns && count < klio_vars.max_patterns)
        {
            pattern_states[count++] = index;
        }
        else
        {
            free(str);
            kenpatt_help(NULL);
            kdispatt_help(NULL);

            return;
        }

        token = strtok(NULL, sep);
    }

    free(str);

    BHY2_ASSERT(bhy2_klio_set_pattern_states(operation, pattern_states, count, bhy2));
}

static void log_data(uint8_t sid, uint64_t tns, uint8_t event_size, uint8_t *event_payload, struct logbin_dev *logdev)
{
    if (logdev && logdev->logfile)
    {
#ifndef PC
        coines_set_led(COINES_LED_GREEN, COINES_LED_STATE_ON);
#endif
        logbin_add_data(sid, tns, event_size, event_payload, logdev);
#ifndef PC
        coines_set_led(COINES_LED_GREEN, COINES_LED_STATE_OFF);
#endif
    }
}

static void write_meta_info(struct logbin_dev *log, struct bhy2_dev *bhy2)
{
    logbin_start_meta(log);

    bhy2_update_virtual_sensor_list(bhy2);
#ifndef PC
    coines_set_pin_config(COINES_APP30_LED_G, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_LOW);
#endif
    for (uint8_t i = 1; i < BHY2_SENSOR_ID_MAX; i++)
    {
        if (bhy2_is_sensor_available(i, bhy2))
        {
            logbin_add_meta(i,
                            get_sensor_name(i),
                            bhy2->event_size[i] - 1,
                            get_sensor_parse_format(i),
                            get_sensor_axis_names(i),
                            get_sensor_default_scaling(i),
                            log);
        }
    }

    logbin_end_meta(log);

#ifndef PC
    coines_set_pin_config(COINES_APP30_LED_G, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
#endif
}
