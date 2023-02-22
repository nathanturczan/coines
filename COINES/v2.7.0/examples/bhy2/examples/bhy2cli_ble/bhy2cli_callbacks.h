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
 * @file    bhy2cli_callbacks.h
 * @brief   Header file for the command line utility callbacks
 *
 */

#ifndef _BHY2CLI_CALLBACKS_H_
#define _BHY2CLI_CALLBACKS_H_

#include <stdint.h>

#include "cli.h"
#include "bhy2.h"
#include "bhy2_klio.h"
#include "bhy2_pdr.h"
#include "bhy2_swim.h"
#include "bhy2_bsec.h"
#include "bhy2_head_tracker.h"
#include "parse.h"

/* Start of CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

struct bhy2_cli_ref
{
    struct bhy2_dev bhy2;
    cli_dev_t cli_dev;
    struct parse_ref parse_table;
};

cli_callback_table_t * bhy2_get_cli_callbacks(void);

uint8_t bhy2_get_n_cli_callbacks(void);

void bhy2_callbacks_init(struct bhy2_cli_ref  *cli_ref);

void bhy2_install_callbacks(struct bhy2_dev *bhy2, struct parse_ref *parse_table);

bool bhy2_are_sensors_active(void);

void bhy2_exit(struct bhy2_cli_ref  *cli_ref);

void bhy2_data_parse_callback(struct bhy2_cli_ref *cli_ref);

bhy2_fifo_parse_callback_t bhy2_get_callback(uint8_t sensor_id);

void klio_callback(uint32_t seconds, uint32_t nanoseconds, bhy2_klio_sensor_frame_t *data, struct bhy2_dev *dev);

int8_t kstatus_help(void *ref);

int8_t kstatus_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t ksetstate_help(void *ref);

int8_t ksetstate_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kgetstate_help(void *ref);

int8_t kgetstate_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kldpatt_help(void *ref);

int8_t kldpatt_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kenpatt_help(void *ref);

int8_t kenpatt_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kdispatt_help(void *ref);

int8_t kdispatt_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kdisapatt_help(void *ref);

int8_t kdisapatt_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kswpatt_help(void *ref);

int8_t kswpatt_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kautldpatt_help(void *ref);

int8_t kautldpatt_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kgetparam_help(void *ref);

int8_t kgetparam_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t ksetparam_help(void *ref);

int8_t ksetparam_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t kmsimscore_help(void *ref);

int8_t kmsimscore_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t ksimscore_help(void *ref);

int8_t ksimscore_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t version_help(void *ref);

int8_t version_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t help_help(void *ref);

int8_t help_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t info_help(void *ref);

int8_t info_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t ramb_help(void *ref);

int8_t ramb_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t flb_help(void *ref);

int8_t flb_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t reset_help(void *ref);

int8_t reset_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t addse_help(void *ref);

int8_t addse_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t rd_help(void *ref);

int8_t rd_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t wr_help(void *ref);

int8_t wr_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t rdp_help(void *ref);

int8_t rdp_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t wrp_help(void *ref);

int8_t wrp_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t ram_help(void *ref);

int8_t ram_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t fl_help(void *ref);

int8_t fl_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t boot_help(void *ref);

int8_t boot_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t erase_help(void *ref);

int8_t erase_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t efd_help(void *ref);

int8_t efd_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t actse_help(void *ref);

int8_t actse_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t logse_help(void *ref);

int8_t logse_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t logtxt_help(void *ref);

int8_t logtxt_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t attlog_help(void *ref);

int8_t attlog_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t detlog_help(void *ref);

int8_t detlog_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t slabel_help(void *ref);

int8_t slabel_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t pfullreset_help(void *ref);

int8_t pfullreset_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t ptrackreset_help(void *ref);

int8_t ptrackreset_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t prefheaddel_help(void *ref);

int8_t prefheaddel_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t pstepinfo_help(void *ref);

int8_t pstepinfo_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t psethand_help(void *ref);

int8_t psethand_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t pdrver_help(void *ref);

int8_t pdrver_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t palver_help(void *ref);

int8_t palver_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t pvariant_help(void *ref);

int8_t pvariant_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t pdevpos_help(void *ref);

int8_t pdevpos_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t swim_help(void *ref);

int8_t swim_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t swimgetaxes_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t swimsetaxes_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t swimgetaxes_help(void *ref);

int8_t swimsetaxes_help(void *ref);

int8_t swimver_help(void *ref);

int8_t swimver_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t swimgetfreq_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t swimsetfreq_callback(uint8_t argc, uint8_t *argv[], void *ref);

int8_t swimgetfreq_help(void *ref);

int8_t swimsetfreq_help(void *ref);

/* End of CPP guard */
#ifdef __cplusplus
}
#endif

#endif /* _BHY2CLI_CALLBACKS_H_ */
