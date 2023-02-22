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
 * @file       bhy2_head_tracker.h
 * @date       2022-04-01
 * @version    v1.4.0
 *
 */

#ifndef _BHY2_HEAD_TRACKER_H_
#define _BHY2_HEAD_TRACKER_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdlib.h>

#include "bhy2.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#define BHY2_SENSOR_ID_HEAD_MIS_CALIB  UINT8_C(120)
#define BHY2_SENSOR_ID_HEAD_ORIENT     UINT8_C(121)

struct bhy2_head_tracker_data
{
    int16_t x, y, z, w;
    uint16_t accuracy;
};

void bhy2_head_tracker_parsing(uint8_t *payload, struct bhy2_head_tracker_data *data);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _BHY2_HEAD_TRACKER_H_ */
