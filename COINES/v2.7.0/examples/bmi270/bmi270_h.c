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
* @file       bmi270_h.c
* @date       2022-02-02
* @version    v2.73.0
*
*/

/***************************************************************************/

/*!             Header files
 ****************************************************************************/
#include "bmi270_h.h"

/******************************************************************************/
/*!  @name         Structure Declaration                                      */
/******************************************************************************/
/*! @name Structure to define tap configuration */
struct bmi2_tap_config *bmi270_h_config;

/***************************************************************************/

/*!              Global Variable
 ****************************************************************************/

/*! @name  Global array that stores the configuration file of BMI270_H */
const uint8_t bmi270_h_config_file[] = {
    0xc8, 0x2e, 0x00, 0x2e, 0x80, 0x2e, 0xbf, 0xb0, 0xc8, 0x2e, 0x00, 0x2e, 0x80, 0x2e, 0xea, 0x00, 0x80, 0x2e, 0x22,
    0xb4, 0x80, 0x2e, 0x91, 0x03, 0xc8, 0x2e, 0x00, 0x2e, 0x80, 0x2e, 0x00, 0xb0, 0x50, 0x30, 0x21, 0x2e, 0x59, 0xf5,
    0x10, 0x30, 0x21, 0x2e, 0x6a, 0xf5, 0x80, 0x2e, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x65, 0x01, 0x00, 0x22,
    0x00, 0x74, 0x00, 0x00, 0x10, 0x00, 0x10, 0xd1, 0x00, 0xe6, 0x9a, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0xfd, 0x2d, 0xe3, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1a, 0x24, 0x22, 0x00, 0x80, 0x2e, 0xa8, 0x01, 0x20, 0x50, 0xe7, 0x7f, 0xf6, 0x7f, 0x06,
    0x32, 0x0f, 0x2e, 0x61, 0xf5, 0xfe, 0x09, 0xc0, 0xb3, 0x04, 0x2f, 0x17, 0x30, 0x2f, 0x2e, 0x98, 0x00, 0x2d, 0x2e,
    0x61, 0xf5, 0xf6, 0x6f, 0xe7, 0x6f, 0xe0, 0x5f, 0xc8, 0x2e, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x65, 0x00, 0x00,
    0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xe0, 0xaa, 0x38, 0x2d, 0x01, 0xd4, 0x7b, 0x3b, 0x01, 0xdb,
    0x7a, 0x04, 0x00, 0x3f, 0x7b, 0xcd, 0x6c, 0xc3, 0x04, 0x85, 0x09, 0xc3, 0x04, 0xec, 0xe6, 0x0c, 0x46, 0x01, 0x00,
    0x27, 0x00, 0x19, 0x00, 0x96, 0x00, 0xa0, 0x00, 0x01, 0x00, 0x0c, 0x00, 0xf0, 0x3c, 0x00, 0x01, 0x01, 0x00, 0x03,
    0x00, 0x01, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x32, 0x00, 0x05, 0xe0, 0x90, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x07, 0x00, 0x1e, 0x00, 0x90, 0x00, 0x06, 0x00, 0x10, 0x00, 0x08, 0x00, 0x50, 0x00, 0x01, 0x00, 0xe8,
    0x03, 0x02, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0x00, 0xc4, 0x00, 0x93, 0x00, 0x00, 0x0c, 0xff,
    0x0f, 0x00, 0x04, 0xc0, 0x00, 0x5b, 0xf5, 0x98, 0xf1, 0xb2, 0x00, 0xb1, 0x00, 0x1e, 0xf2, 0x80, 0x00, 0x3f, 0xff,
    0x19, 0xf4, 0x58, 0xf5, 0x66, 0xf5, 0xff, 0x00, 0xb4, 0xf1, 0x99, 0x00, 0xe0, 0x00, 0xad, 0x00, 0x7d, 0x00, 0x9c,
    0x00, 0x7f, 0x00, 0x81, 0x00, 0xff, 0x3f, 0xff, 0xfb, 0x29, 0x01, 0x00, 0x38, 0x00, 0x30, 0xfd, 0xf5, 0xfc, 0xf5,
    0xb5, 0x00, 0xa0, 0x00, 0x5f, 0xff, 0x00, 0x40, 0x52, 0x0f, 0xeb, 0x00, 0x7f, 0xff, 0xc2, 0xf5, 0x68, 0xf7, 0x4c,
    0x0f, 0x40, 0x0f, 0x46, 0x0f, 0x65, 0x0f, 0x58, 0xf7, 0x5b, 0xf7, 0x68, 0x0f, 0x00, 0x80, 0xff, 0x7f, 0x86, 0x00,
    0x57, 0x0f, 0x6a, 0x0f, 0xb3, 0xf1, 0x64, 0x0f, 0x6c, 0xf7, 0xb9, 0xf1, 0xc6, 0xf1, 0x00, 0xe0, 0x00, 0xff, 0xd1,
    0xf5, 0x6c, 0x0f, 0x6f, 0x0f, 0xff, 0x03, 0x00, 0xfc, 0xf0, 0x3f, 0x0b, 0x01, 0x0e, 0x01, 0x2b, 0x01, 0xb9, 0x00,
    0x2d, 0xf5, 0xca, 0xf5, 0xb3, 0x00, 0x20, 0xf2, 0xf0, 0x00, 0xb5, 0x00, 0x00, 0x08, 0x00, 0xf8, 0xe8, 0x03, 0x01,
    0x80, 0xb8, 0x7e, 0xe1, 0x7a, 0x64, 0xf5, 0xba, 0xf1, 0xae, 0xf1, 0xfe, 0xaf, 0xdd, 0x00, 0xc7, 0x00, 0x08, 0xd0,
    0x74, 0xf7, 0xda, 0x00, 0xde, 0x00, 0x6f, 0xf7, 0xcb, 0x00, 0xc3, 0xf5, 0x40, 0x50, 0x98, 0x2e, 0xbd, 0x0e, 0x50,
    0x32, 0x98, 0x2e, 0x47, 0x03, 0x00, 0x30, 0xf0, 0x7f, 0xe0, 0x7f, 0x00, 0x2e, 0x00, 0x2e, 0xd0, 0x2e, 0x12, 0x30,
    0x00, 0x2e, 0x01, 0x80, 0x08, 0xa2, 0xfb, 0x2f, 0x01, 0x2e, 0x30, 0x01, 0x8f, 0xbd, 0x8e, 0xbc, 0xbf, 0xb9, 0x9f,
    0xb8, 0x0d, 0xbc, 0x0f, 0xb8, 0x59, 0x0a, 0x48, 0x0a, 0x01, 0x2e, 0xc2, 0x00, 0x23, 0x2e, 0x19, 0x00, 0x00, 0xb2,
    0x18, 0x2f, 0x05, 0x2e, 0x0a, 0x01, 0x91, 0x52, 0x98, 0x2e, 0xc7, 0xc1, 0x05, 0x2e, 0x30, 0x01, 0x2c, 0xbd, 0x2f,
    0xb9, 0x80, 0x90, 0x25, 0x2e, 0x92, 0x00, 0xf0, 0x7f, 0x03, 0x2f, 0x91, 0x52, 0x98, 0x2e, 0xce, 0xb2, 0x03, 0x2d,
    0x93, 0x52, 0x98, 0x2e, 0xce, 0xb2, 0x12, 0x30, 0xe0, 0x7f, 0x25, 0x2e, 0x77, 0x00, 0x01, 0x2e, 0x96, 0x00, 0x00,
    0x90, 0x02, 0x2f, 0x25, 0x2e, 0x77, 0x00, 0x15, 0x2d, 0x05, 0x2e, 0x0a, 0x01, 0x95, 0x52, 0x98, 0x2e, 0xc7, 0xc1,
    0x05, 0x2e, 0x18, 0x00, 0xf0, 0x7f, 0x80, 0xb2, 0x0a, 0x2f, 0x95, 0x50, 0x98, 0x2e, 0x4d, 0xc3, 0x95, 0x50, 0x98,
    0x2e, 0x5a, 0xc7, 0x98, 0x2e, 0x3a, 0xb1, 0x10, 0x30, 0x21, 0x2e, 0x77, 0x00, 0x01, 0x2e, 0x98, 0x00, 0x00, 0xb2,
    0x04, 0x2f, 0x98, 0x2e, 0x1f, 0xb2, 0x00, 0x30, 0x21, 0x2e, 0x98, 0x00, 0x01, 0x2e, 0x85, 0x00, 0x04, 0xae, 0x0b,
    0x2f, 0x01, 0x2e, 0x96, 0x00, 0x00, 0xb2, 0x07, 0x2f, 0x95, 0x52, 0x98, 0x2e, 0x74, 0x0e, 0x00, 0xb2, 0x02, 0x2f,
    0x10, 0x30, 0x21, 0x2e, 0x7c, 0x00, 0x01, 0x2e, 0x7c, 0x00, 0x00, 0x90, 0x90, 0x2e, 0xe5, 0x02, 0x01, 0x2e, 0x88,
    0x00, 0x00, 0xb2, 0x04, 0x2f, 0x98, 0x2e, 0x15, 0x0e, 0x00, 0x30, 0x21, 0x2e, 0x7a, 0x00, 0x01, 0x2e, 0x7a, 0x00,
    0x00, 0xb2, 0x12, 0x2f, 0x01, 0x2e, 0x85, 0x00, 0x00, 0x90, 0x02, 0x2f, 0x98, 0x2e, 0x05, 0x0e, 0x09, 0x2d, 0x98,
    0x2e, 0x5d, 0x0d, 0x01, 0x2e, 0x85, 0x00, 0x04, 0x90, 0x02, 0x2f, 0x50, 0x32, 0x98, 0x2e, 0x47, 0x03, 0x00, 0x30,
    0x21, 0x2e, 0x7a, 0x00, 0x01, 0x2e, 0x7b, 0x00, 0x00, 0xb2, 0x90, 0x2e, 0xfd, 0x02, 0x01, 0x2e, 0x7b, 0x00, 0x02,
    0x31, 0x02, 0x08, 0x00, 0xb2, 0x04, 0x2f, 0x98, 0x2e, 0x47, 0xcb, 0x10, 0x30, 0x21, 0x2e, 0x77, 0x00, 0x82, 0x30,
    0x01, 0x2e, 0x7b, 0x00, 0x02, 0x08, 0x00, 0xb2, 0x61, 0x2f, 0x05, 0x2e, 0x09, 0x01, 0x01, 0x2e, 0x85, 0x00, 0x28,
    0xbd, 0x28, 0xb9, 0x05, 0xb2, 0x9f, 0x58, 0x23, 0x2f, 0x07, 0x90, 0x99, 0x52, 0x00, 0x30, 0x37, 0x2f, 0x15, 0x41,
    0x04, 0x41, 0xdc, 0xbe, 0x44, 0xbe, 0xdc, 0xba, 0x2c, 0x01, 0xa2, 0x00, 0x9f, 0x56, 0x51, 0x0f, 0x0c, 0x2f, 0xd2,
    0x42, 0x24, 0xb9, 0xc2, 0x42, 0x12, 0x30, 0x03, 0x2e, 0x6a, 0xf7, 0x9c, 0xbc, 0x9f, 0xb8, 0x40, 0xb2, 0x10, 0x22,
    0x98, 0x2e, 0x6d, 0xb2, 0x21, 0x2d, 0x62, 0x30, 0x25, 0x2e, 0x85, 0x00, 0x98, 0x2e, 0x6d, 0xb2, 0x00, 0x30, 0x21,
    0x2e, 0x5a, 0xf5, 0x18, 0x2d, 0xd2, 0x7f, 0x50, 0x30, 0x98, 0x2e, 0x47, 0x03, 0x9f, 0x54, 0x97, 0x50, 0x90, 0x42,
    0x70, 0x30, 0x9d, 0x52, 0x81, 0x42, 0xbe, 0x84, 0xd1, 0x6f, 0x40, 0xb2, 0x81, 0x42, 0x05, 0x2f, 0x21, 0x2e, 0x85,
    0x00, 0x10, 0x30, 0x98, 0x2e, 0x6d, 0xb2, 0x03, 0x2d, 0x60, 0x30, 0x21, 0x2e, 0x85, 0x00, 0x01, 0x2e, 0x85, 0x00,
    0x06, 0x90, 0x18, 0x2f, 0x01, 0x2e, 0x75, 0x00, 0x9b, 0x54, 0x97, 0x52, 0xd0, 0x7f, 0x98, 0x2e, 0x7a, 0xc1, 0xd2,
    0x6f, 0x10, 0x1a, 0x40, 0x30, 0x08, 0x2f, 0x21, 0x2e, 0x85, 0x00, 0x20, 0x30, 0x98, 0x2e, 0x59, 0xb2, 0x50, 0x32,
    0x98, 0x2e, 0x47, 0x03, 0x05, 0x2d, 0x98, 0x2e, 0x1e, 0x0e, 0x00, 0x30, 0x21, 0x2e, 0x85, 0x00, 0x42, 0x30, 0x01,
    0x2e, 0x7b, 0x00, 0x02, 0x08, 0x00, 0xb2, 0x26, 0x2f, 0x01, 0x2e, 0x8b, 0x00, 0x00, 0xb2, 0x0d, 0x2f, 0x01, 0x90,
    0x1d, 0x2f, 0x05, 0x2e, 0x3f, 0x01, 0x07, 0x2e, 0x8c, 0x00, 0xa1, 0x32, 0xa3, 0x58, 0x98, 0x2e, 0x9a, 0xb2, 0x00,
    0x30, 0x21, 0x2e, 0x8b, 0x00, 0x12, 0x2d, 0xa1, 0x58, 0xa3, 0x32, 0x10, 0x41, 0x11, 0x41, 0x18, 0xb9, 0x04, 0x41,
    0x98, 0xbc, 0x41, 0x0a, 0x94, 0x0a, 0x98, 0x2e, 0x8c, 0xb2, 0x12, 0x30, 0x21, 0x2e, 0x8c, 0x00, 0x21, 0x2e, 0xb2,
    0x00, 0x25, 0x2e, 0x8b, 0x00, 0x10, 0x30, 0x21, 0x2e, 0x77, 0x00, 0x00, 0x30, 0x21, 0x2e, 0x7b, 0x00, 0x18, 0x2d,
    0x01, 0x2e, 0x85, 0x00, 0x03, 0xaa, 0x01, 0x2f, 0x98, 0x2e, 0x2b, 0x0e, 0x01, 0x2e, 0x85, 0x00, 0x3f, 0x80, 0x03,
    0xa2, 0x01, 0x2f, 0x00, 0x2e, 0x02, 0x2d, 0x98, 0x2e, 0x41, 0x0e, 0x30, 0x30, 0x98, 0x2e, 0x78, 0xb2, 0x00, 0x30,
    0x21, 0x2e, 0x7c, 0x00, 0x50, 0x32, 0x98, 0x2e, 0x47, 0x03, 0x01, 0x2e, 0x77, 0x00, 0x00, 0xb2, 0x24, 0x2f, 0x98,
    0x2e, 0xf5, 0xcb, 0x05, 0x2e, 0x86, 0x00, 0x02, 0x0a, 0x21, 0x2e, 0xb1, 0x00, 0xa5, 0x50, 0x3c, 0x80, 0x02, 0x86,
    0x02, 0x40, 0xa8, 0xb8, 0xa7, 0x50, 0xc3, 0x40, 0x12, 0x42, 0x11, 0x42, 0x33, 0xbd, 0xe1, 0x6f, 0x8a, 0x0a, 0x12,
    0x42, 0xd0, 0x7f, 0xf2, 0x31, 0x00, 0x40, 0x02, 0x08, 0xf2, 0x6f, 0x25, 0xbd, 0x02, 0x0a, 0xc0, 0x7f, 0x98, 0x2e,
    0xa8, 0xcf, 0x06, 0xbc, 0xc2, 0x6f, 0xd1, 0x6f, 0x10, 0x0a, 0x40, 0x42, 0x98, 0x2e, 0xfd, 0xb1, 0x00, 0x30, 0x21,
    0x2e, 0xc2, 0x00, 0x21, 0x2e, 0x77, 0x00, 0x21, 0x2e, 0x96, 0x00, 0x80, 0x2e, 0xb1, 0x01, 0xcf, 0x52, 0x00, 0x2e,
    0x60, 0x40, 0x41, 0x40, 0x0d, 0xbc, 0x98, 0xbc, 0xc0, 0x2e, 0x01, 0x0a, 0x0f, 0xb8, 0xd1, 0x52, 0x53, 0x3c, 0x52,
    0x40, 0x40, 0x40, 0x4b, 0x00, 0x82, 0x16, 0x26, 0xb9, 0x01, 0xb8, 0x41, 0x40, 0x10, 0x08, 0x97, 0xb8, 0x01, 0x08,
    0xc0, 0x2e, 0x11, 0x30, 0x01, 0x08, 0x21, 0x2e, 0x59, 0xf5, 0x10, 0x30, 0xc0, 0x2e, 0x21, 0x2e, 0x4a, 0xf1, 0x80,
    0x2e, 0x00, 0xc1, 0xfd, 0x2d, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x01,
    0x34, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xf0, 0x50, 0xe7, 0x7f, 0xf6, 0x7f, 0x46, 0x30, 0x0f, 0x2e, 0xa4, 0xf1, 0xbe, 0x09, 0x80, 0xb3, 0xd6,
    0x7f, 0xc5, 0x7f, 0xb4, 0x7f, 0xa3, 0x7f, 0x92, 0x7f, 0x81, 0x7f, 0x70, 0x7f, 0x6b, 0x7f, 0x36, 0x2f, 0x01, 0x2e,
    0x69, 0xf7, 0x0d, 0xbc, 0x0f, 0xb8, 0x00, 0x90, 0x30, 0x2f, 0xb5, 0x54, 0x0a, 0x25, 0xa1, 0x40, 0x83, 0x40, 0x83,
    0x84, 0x18, 0xbe, 0xa5, 0x40, 0x81, 0x40, 0x83, 0x84, 0x23, 0x0b, 0x58, 0xbf, 0xa3, 0x40, 0x14, 0x7f, 0x71, 0x0a,
    0x85, 0x40, 0xb8, 0xbd, 0x21, 0x7f, 0x31, 0x80, 0xbc, 0x84, 0x5d, 0x0a, 0x31, 0x7f, 0x52, 0x7f, 0x40, 0x7f, 0x98,
    0x2e, 0x82, 0xb5, 0x40, 0x6f, 0x98, 0x2e, 0x29, 0xb6, 0x12, 0x6f, 0xb3, 0x50, 0x51, 0x6f, 0xd0, 0x08, 0xa8, 0xb6,
    0x24, 0x6f, 0x53, 0x42, 0x68, 0x09, 0x48, 0xb5, 0xe0, 0x08, 0x55, 0x42, 0x53, 0x42, 0x90, 0x08, 0x34, 0x6f, 0xe0,
    0x08, 0xc8, 0xb6, 0x52, 0x42, 0x28, 0x08, 0x53, 0x42, 0x40, 0x42, 0x00, 0x2e, 0xd0, 0x6f, 0x00, 0xb2, 0x10, 0x30,
    0x05, 0x2f, 0x03, 0x2e, 0x85, 0x00, 0x44, 0xae, 0x01, 0x2f, 0x21, 0x2e, 0x7a, 0x00, 0x80, 0x30, 0x21, 0x2e, 0x60,
    0xf5, 0xf6, 0x6f, 0xe7, 0x6f, 0x70, 0x6f, 0x81, 0x6f, 0x92, 0x6f, 0xa3, 0x6f, 0xb4, 0x6f, 0xc5, 0x6f, 0x6b, 0x6f,
    0x10, 0x5f, 0xc8, 0x2e, 0x43, 0x86, 0x25, 0x40, 0x04, 0x40, 0xd8, 0xbe, 0x2c, 0x0b, 0x22, 0x11, 0x54, 0x42, 0x03,
    0x80, 0x4b, 0x0e, 0xf6, 0x2f, 0xb8, 0x2e, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x90, 0x50, 0xf7, 0x7f,
    0xe6, 0x7f, 0xd5, 0x7f, 0xc4, 0x7f, 0xb3, 0x7f, 0xa1, 0x7f, 0x90, 0x7f, 0x82, 0x7f, 0x7b, 0x7f, 0x98, 0x2e, 0x2f,
    0x03, 0x00, 0xb2, 0x90, 0x2e, 0x9a, 0xb0, 0x05, 0x2e, 0x29, 0x01, 0x01, 0x2e, 0x0f, 0x01, 0x03, 0x2e, 0x2c, 0x01,
    0x23, 0xbe, 0x0f, 0xb8, 0x9f, 0xb8, 0xa4, 0xbd, 0x01, 0x0a, 0x4f, 0xba, 0xbf, 0xb8, 0x04, 0x0a, 0x22, 0xbd, 0x01,
    0x0a, 0x2f, 0xb9, 0x82, 0x0a, 0x25, 0x2e, 0x18, 0x00, 0x05, 0x2e, 0xc1, 0xf5, 0x2e, 0xbd, 0x2e, 0xb9, 0x01, 0x2e,
    0x76, 0x00, 0x31, 0x30, 0x8a, 0x04, 0x00, 0x90, 0x0b, 0x2f, 0x01, 0x2e, 0x85, 0x00, 0x04, 0xa2, 0x07, 0x2f, 0x01,
    0x2e, 0x18, 0x00, 0x00, 0x90, 0x03, 0x2f, 0x01, 0x2e, 0x79, 0x00, 0x00, 0xb2, 0x19, 0x2f, 0xad, 0x50, 0x95, 0x52,
    0x98, 0x2e, 0xf1, 0x03, 0x05, 0x2e, 0x78, 0x00, 0x25, 0x2e, 0x96, 0x00, 0x05, 0x2e, 0x78, 0x00, 0x80, 0x90, 0x02,
    0x2f, 0x12, 0x30, 0x25, 0x2e, 0x78, 0x00, 0x01, 0x2e, 0x79, 0x00, 0x00, 0xb2, 0x10, 0x30, 0x05, 0x2e, 0x18, 0x00,
    0x01, 0x2f, 0x21, 0x2e, 0x18, 0x00, 0x25, 0x2e, 0x79, 0x00, 0x98, 0x2e, 0x38, 0x03, 0x00, 0xb2, 0x22, 0x30, 0x21,
    0x30, 0x03, 0x2f, 0x01, 0x2e, 0x76, 0x00, 0x00, 0x90, 0x05, 0x2f, 0x01, 0x2e, 0x19, 0x00, 0x00, 0x90, 0x30, 0x30,
    0x01, 0x30, 0x41, 0x22, 0x01, 0x2e, 0x97, 0x00, 0x08, 0x1a, 0x0e, 0x2f, 0x23, 0x2e, 0x97, 0x00, 0x33, 0x30, 0xaf,
    0x50, 0x0b, 0x09, 0x01, 0x40, 0xab, 0x56, 0x46, 0xbe, 0x4b, 0x08, 0x4c, 0x0a, 0x01, 0x42, 0x0a, 0x80, 0xa9, 0x52,
    0x01, 0x42, 0x00, 0x2e, 0x01, 0x2e, 0x18, 0x00, 0x00, 0xb2, 0x1f, 0x2f, 0x03, 0x2e, 0xc0, 0xf5, 0xf0, 0x30, 0x48,
    0x08, 0x47, 0xaa, 0x74, 0x30, 0x07, 0x2e, 0x91, 0x00, 0x61, 0x22, 0x4b, 0x1a, 0x05, 0x2f, 0x07, 0x2e, 0x66, 0xf5,
    0xbf, 0xbd, 0xbf, 0xb9, 0xc0, 0x90, 0x0b, 0x2f, 0xb1, 0x56, 0x04, 0x30, 0xd4, 0x42, 0xd2, 0x42, 0x81, 0x04, 0x24,
    0xbd, 0xfe, 0x80, 0x81, 0x84, 0xc4, 0x42, 0x23, 0x2e, 0x91, 0x00, 0x02, 0x42, 0x02, 0x32, 0x25, 0x2e, 0x62, 0xf5,
    0x05, 0x2e, 0x87, 0x00, 0x81, 0x82, 0x05, 0x2e, 0x0a, 0x01, 0x23, 0x2e, 0x87, 0x00, 0x9b, 0x50, 0x90, 0x08, 0x01,
    0x31, 0x23, 0x2e, 0x60, 0xf5, 0x80, 0xb2, 0x0b, 0x2f, 0xf2, 0x3e, 0x01, 0x2e, 0xca, 0xf5, 0x82, 0x08, 0x25, 0x2e,
    0xca, 0xf5, 0x05, 0x2e, 0x59, 0xf5, 0xe0, 0x3f, 0x90, 0x08, 0x25, 0x2e, 0x59, 0xf5, 0x90, 0x6f, 0xb3, 0x6f, 0xc4,
    0x6f, 0xd5, 0x6f, 0xe6, 0x6f, 0xf7, 0x6f, 0x7b, 0x6f, 0x82, 0x6f, 0xa1, 0x6f, 0x70, 0x5f, 0xc8, 0x2e, 0xc0, 0x50,
    0xe7, 0x7f, 0xf6, 0x7f, 0x26, 0x30, 0x0f, 0x2e, 0x61, 0xf5, 0x2f, 0x2e, 0x7b, 0x00, 0x0f, 0x2e, 0x7b, 0x00, 0xbe,
    0x09, 0xa2, 0x7f, 0x91, 0x7f, 0x80, 0x7f, 0x80, 0xb3, 0xd5, 0x7f, 0xc4, 0x7f, 0xb3, 0x7f, 0x7b, 0x7f, 0x11, 0x2f,
    0xb7, 0x50, 0x1a, 0x25, 0x12, 0x40, 0x42, 0x7f, 0x74, 0x82, 0x12, 0x40, 0x52, 0x7f, 0x00, 0x2e, 0x00, 0x40, 0x60,
    0x7f, 0x98, 0x2e, 0x6a, 0xd6, 0x01, 0x2e, 0x61, 0xf7, 0x01, 0x31, 0x01, 0x0a, 0x21, 0x2e, 0x61, 0xf7, 0x80, 0x30,
    0x03, 0x2e, 0x7b, 0x00, 0x08, 0x08, 0x00, 0xb2, 0x40, 0x2f, 0x03, 0x2e, 0x09, 0x01, 0x17, 0xbc, 0x96, 0xbc, 0x0f,
    0xb8, 0x9f, 0xb8, 0x40, 0x90, 0x21, 0x2e, 0x89, 0x00, 0x10, 0x30, 0x01, 0x30, 0x2a, 0x2f, 0x03, 0x2e, 0x85, 0x00,
    0x44, 0xb2, 0x05, 0x2f, 0x47, 0xb2, 0x00, 0x30, 0x2d, 0x2f, 0x21, 0x2e, 0x7b, 0x00, 0x2b, 0x2d, 0x03, 0x2e, 0xfd,
    0xf5, 0x9e, 0xbc, 0x9f, 0xb8, 0x40, 0x90, 0x14, 0x2f, 0x03, 0x2e, 0xfc, 0xf5, 0x99, 0xbc, 0x9f, 0xb8, 0x40, 0x90,
    0x0e, 0x2f, 0x03, 0x2e, 0x49, 0xf1, 0xb9, 0x54, 0x4a, 0x08, 0x40, 0x90, 0x08, 0x2f, 0x98, 0x2e, 0x2f, 0x03, 0x00,
    0xb2, 0x10, 0x30, 0x03, 0x2f, 0x50, 0x30, 0x21, 0x2e, 0x85, 0x00, 0x10, 0x2d, 0x98, 0x2e, 0x59, 0xb2, 0x00, 0x30,
    0x21, 0x2e, 0x7b, 0x00, 0x0a, 0x2d, 0x05, 0x2e, 0x69, 0xf7, 0x2d, 0xbd, 0x2f, 0xb9, 0x80, 0xb2, 0x01, 0x2f, 0x21,
    0x2e, 0x7c, 0x00, 0x23, 0x2e, 0x7b, 0x00, 0xe0, 0x31, 0x21, 0x2e, 0x61, 0xf5, 0xf6, 0x6f, 0xe7, 0x6f, 0x80, 0x6f,
    0xa2, 0x6f, 0xb3, 0x6f, 0xc4, 0x6f, 0xd5, 0x6f, 0x7b, 0x6f, 0x91, 0x6f, 0x40, 0x5f, 0xc8, 0x2e, 0xc9, 0x50, 0xf0,
    0x50, 0x12, 0x40, 0x06, 0x40, 0x1a, 0x25, 0x6c, 0xbe, 0x76, 0x8a, 0x74, 0x80, 0xfb, 0x7f, 0x68, 0xbf, 0xcb, 0x56,
    0x0b, 0x30, 0xd3, 0x08, 0x4c, 0xba, 0x6c, 0xbb, 0x5b, 0x7f, 0x4b, 0x43, 0x0b, 0x42, 0xc0, 0xb2, 0xc6, 0x7f, 0xb4,
    0x7f, 0xd0, 0x7f, 0xe5, 0x7f, 0xa3, 0x7f, 0x90, 0x2e, 0xe9, 0xb1, 0x01, 0x2e, 0x83, 0x00, 0x00, 0xb2, 0x0b, 0x2f,
    0xbf, 0x52, 0x01, 0x2e, 0x7e, 0x00, 0x92, 0x7f, 0x98, 0x2e, 0xbb, 0xcc, 0x01, 0x30, 0x23, 0x2e, 0x83, 0x00, 0x92,
    0x6f, 0xa3, 0x6f, 0x1a, 0x25, 0x26, 0xbc, 0x86, 0xba, 0x25, 0xbc, 0x0f, 0xb8, 0x54, 0xb1, 0x00, 0xb2, 0x96, 0x7f,
    0x0c, 0x2f, 0xc1, 0x50, 0xc3, 0x54, 0x0b, 0x30, 0x0b, 0x2e, 0x29, 0x01, 0xc7, 0x58, 0x1b, 0x42, 0x9b, 0x42, 0x6c,
    0x09, 0x0b, 0x42, 0x2b, 0x2e, 0x29, 0x01, 0x8b, 0x42, 0x71, 0x84, 0xcd, 0x50, 0x58, 0x09, 0xc5, 0x52, 0x95, 0x50,
    0x82, 0x7f, 0x75, 0x7f, 0x98, 0x2e, 0xc2, 0xc0, 0x01, 0x2e, 0x7e, 0x00, 0xe5, 0x6f, 0xd4, 0x6f, 0x73, 0x6f, 0x82,
    0x6f, 0xbf, 0x52, 0xbd, 0x5c, 0x98, 0x2e, 0x06, 0xcd, 0x65, 0x6f, 0xa0, 0x6f, 0xc1, 0x52, 0x40, 0xb3, 0x04, 0xbd,
    0x53, 0x40, 0xaf, 0xba, 0x44, 0x40, 0xe1, 0x7f, 0x02, 0x30, 0x06, 0x2f, 0x40, 0xb3, 0x02, 0x30, 0x03, 0x2f, 0xc3,
    0x5c, 0x12, 0x30, 0x93, 0x43, 0x84, 0x43, 0x03, 0xbf, 0x6f, 0xbb, 0x80, 0xb3, 0x20, 0x2f, 0x46, 0x6f, 0xde, 0x00,
    0x56, 0x6f, 0x26, 0x03, 0x44, 0x42, 0x40, 0x91, 0x27, 0x2e, 0x7f, 0x00, 0xc3, 0x52, 0x14, 0x2f, 0xc3, 0x5c, 0x00,
    0x2e, 0x95, 0x41, 0x86, 0x41, 0x5d, 0x05, 0xa6, 0x07, 0x80, 0xab, 0x04, 0x2f, 0x80, 0x91, 0x0a, 0x2f, 0x96, 0x6f,
    0x75, 0x0f, 0x07, 0x2f, 0x95, 0x6f, 0x40, 0xb3, 0x04, 0x2f, 0x53, 0x42, 0x44, 0x42, 0x12, 0x30, 0x04, 0x2c, 0x11,
    0x30, 0x02, 0x2c, 0x11, 0x30, 0x11, 0x30, 0x02, 0xbc, 0x0f, 0xb8, 0xd2, 0x7f, 0x00, 0x90, 0x06, 0x2f, 0x31, 0x30,
    0x23, 0x2e, 0xaf, 0x00, 0x23, 0x2e, 0x7d, 0x00, 0x0b, 0x2c, 0x01, 0x30, 0x01, 0x2e, 0x7d, 0x00, 0x05, 0x2e, 0xaf,
    0x00, 0x10, 0x1a, 0x02, 0x2f, 0x21, 0x2e, 0xaf, 0x00, 0x01, 0x2d, 0x01, 0x30, 0xc0, 0x6f, 0x98, 0x2e, 0x95, 0xcf,
    0xd1, 0x6f, 0xb0, 0x6f, 0x98, 0x2e, 0x95, 0xcf, 0xe2, 0x6f, 0xbb, 0x52, 0x01, 0x2e, 0x7f, 0x00, 0x82, 0x40, 0x50,
    0x42, 0x11, 0x2c, 0x42, 0x42, 0x10, 0x30, 0x31, 0x30, 0x21, 0x2e, 0x83, 0x00, 0x23, 0x2e, 0xaf, 0x00, 0x23, 0x2e,
    0x7d, 0x00, 0xc0, 0x6f, 0x01, 0x30, 0x98, 0x2e, 0x95, 0xcf, 0xb0, 0x6f, 0x01, 0x30, 0x98, 0x2e, 0x95, 0xcf, 0x00,
    0x2e, 0xfb, 0x6f, 0x10, 0x5f, 0xb8, 0x2e, 0xbb, 0x50, 0x10, 0x50, 0xd3, 0x52, 0x05, 0x2e, 0x84, 0x00, 0xfb, 0x7f,
    0x00, 0x2e, 0x13, 0x40, 0x93, 0x42, 0x41, 0x0e, 0xfb, 0x2f, 0x98, 0x2e, 0x4f, 0xb2, 0x98, 0x2e, 0x87, 0xcf, 0x01,
    0x2e, 0x8a, 0x00, 0x00, 0xb2, 0xfb, 0x6f, 0x0b, 0x2f, 0x01, 0x2e, 0x69, 0xf7, 0xb1, 0x3f, 0x01, 0x08, 0x01, 0x30,
    0xf0, 0x5f, 0x23, 0x2e, 0x8a, 0x00, 0x21, 0x2e, 0x69, 0xf7, 0x80, 0x2e, 0x1f, 0xb2, 0xf0, 0x5f, 0xb8, 0x2e, 0x03,
    0x2e, 0xfc, 0xf5, 0x01, 0x2e, 0xc0, 0xf8, 0xa9, 0x54, 0xd5, 0x56, 0x82, 0x08, 0x0b, 0x2e, 0x69, 0xf7, 0xcb, 0x0a,
    0xd7, 0x58, 0x80, 0x90, 0xdd, 0xbe, 0x4c, 0x08, 0x5f, 0xb9, 0x59, 0x22, 0x80, 0x90, 0x07, 0x2f, 0x03, 0x34, 0xc3,
    0x08, 0xf2, 0x3a, 0x0a, 0x08, 0x02, 0x35, 0xc0, 0x90, 0x4a, 0x0a, 0x48, 0x22, 0xc0, 0x2e, 0x23, 0x2e, 0xfc, 0xf5,
    0x10, 0x50, 0xfb, 0x7f, 0x98, 0x2e, 0x56, 0xc7, 0x98, 0x2e, 0x49, 0xc3, 0x31, 0x30, 0x10, 0x30, 0xfb, 0x6f, 0xf0,
    0x5f, 0x23, 0x2e, 0x7d, 0x00, 0x21, 0x2e, 0x77, 0x00, 0x21, 0x2e, 0x83, 0x00, 0x23, 0x2e, 0xaf, 0x00, 0xb8, 0x2e,
    0x03, 0x2e, 0x84, 0x00, 0x16, 0xb8, 0x02, 0x34, 0x4a, 0x0c, 0x21, 0x2e, 0x2d, 0xf5, 0xc0, 0x2e, 0x23, 0x2e, 0x84,
    0x00, 0x03, 0xbc, 0x21, 0x2e, 0x86, 0x00, 0x03, 0x2e, 0x86, 0x00, 0x40, 0xb2, 0x10, 0x30, 0x21, 0x2e, 0x77, 0x00,
    0x01, 0x30, 0x05, 0x2f, 0x05, 0x2e, 0x89, 0x00, 0x80, 0x90, 0x01, 0x2f, 0x23, 0x2e, 0x6f, 0xf5, 0xc0, 0x2e, 0x21,
    0x2e, 0x8a, 0x00, 0x11, 0x30, 0x81, 0x08, 0x01, 0x2e, 0x6a, 0xf7, 0x71, 0x3f, 0x23, 0xbd, 0x01, 0x08, 0x02, 0x0a,
    0xc0, 0x2e, 0x21, 0x2e, 0x6a, 0xf7, 0x30, 0x25, 0x00, 0x30, 0x21, 0x2e, 0x5a, 0xf5, 0x10, 0x50, 0x21, 0x2e, 0x7a,
    0x00, 0x21, 0x2e, 0x7b, 0x00, 0xfb, 0x7f, 0x98, 0x2e, 0x6d, 0xb2, 0x40, 0x30, 0x21, 0x2e, 0x85, 0x00, 0xfb, 0x6f,
    0xf0, 0x5f, 0x03, 0x25, 0x80, 0x2e, 0x59, 0xb2, 0x40, 0x50, 0x0a, 0x25, 0x3c, 0x80, 0xfb, 0x7f, 0x01, 0x42, 0xd2,
    0x7f, 0xe3, 0x7f, 0x32, 0x30, 0x10, 0x25, 0x98, 0x2e, 0x7a, 0xc1, 0xfb, 0x6f, 0xc0, 0x5f, 0xb8, 0x2e, 0x70, 0x50,
    0x0a, 0x25, 0x39, 0x80, 0xf3, 0x7f, 0x03, 0x42, 0xa1, 0x7f, 0xc2, 0x7f, 0xd1, 0x7f, 0x03, 0x30, 0x03, 0x43, 0xe4,
    0x7f, 0xbb, 0x7f, 0x22, 0x30, 0x10, 0x25, 0x98, 0x2e, 0x7a, 0xc1, 0xd2, 0x6f, 0x02, 0x17, 0x04, 0x08, 0xc1, 0x6f,
    0x0c, 0x09, 0x04, 0x1a, 0x10, 0x30, 0x04, 0x30, 0x20, 0x22, 0x01, 0xb2, 0x14, 0x2f, 0x27, 0x59, 0x14, 0x09, 0xf3,
    0x30, 0x93, 0x08, 0x24, 0xbd, 0x44, 0xba, 0x94, 0x0a, 0x02, 0x17, 0xf3, 0x6f, 0x4c, 0x08, 0x9a, 0x08, 0x8a, 0x0a,
    0x09, 0x53, 0x51, 0x08, 0xb3, 0x58, 0x94, 0x08, 0x28, 0xbd, 0x98, 0xb8, 0xe4, 0x6f, 0x51, 0x0a, 0x01, 0x43, 0x00,
    0x2e, 0xbb, 0x6f, 0x90, 0x5f, 0xb8, 0x2e, 0x01, 0x2e, 0x30, 0x01, 0x31, 0x25, 0x08, 0xbd, 0x71, 0x30, 0x20, 0x50,
    0x2c, 0xb9, 0x01, 0x09, 0xf2, 0x7f, 0x00, 0x91, 0xeb, 0x7f, 0x0a, 0x2f, 0x01, 0x30, 0xf0, 0x6f, 0x98, 0x2e, 0x95,
    0xcf, 0x13, 0x30, 0x27, 0x2e, 0x8e, 0x00, 0xeb, 0x6f, 0xc0, 0x2e, 0x00, 0x30, 0xe0, 0x5f, 0x01, 0x2e, 0x8e, 0x00,
    0x00, 0xb2, 0x05, 0x2f, 0x06, 0x30, 0x29, 0x51, 0x98, 0x2e, 0x06, 0xb3, 0x2d, 0x2e, 0x8e, 0x00, 0x05, 0x2e, 0x8d,
    0x00, 0x8b, 0x80, 0x29, 0x53, 0x04, 0x42, 0x98, 0x2e, 0x41, 0xb3, 0x10, 0x25, 0xf0, 0x6f, 0xf1, 0x7f, 0x98, 0x2e,
    0x95, 0xcf, 0xf0, 0x6f, 0x00, 0x90, 0x02, 0x2f, 0x01, 0x2e, 0x8f, 0x00, 0x02, 0x2d, 0x21, 0x2e, 0x8f, 0x00, 0xeb,
    0x6f, 0xe0, 0x5f, 0xb8, 0x2e, 0x0a, 0x82, 0x02, 0x30, 0x12, 0x42, 0x41, 0x0e, 0xfc, 0x2f, 0x37, 0x80, 0x2b, 0x53,
    0x11, 0x42, 0x00, 0x2e, 0x2d, 0x53, 0x01, 0x42, 0xb8, 0x2e, 0x07, 0x88, 0x46, 0x8e, 0x06, 0x41, 0x3c, 0x8b, 0x32,
    0x1a, 0x42, 0x41, 0xc7, 0x41, 0x16, 0x2f, 0x80, 0x91, 0x23, 0x2f, 0x57, 0x0f, 0x16, 0x30, 0x0c, 0x2f, 0x06, 0x80,
    0x00, 0x2e, 0x00, 0x40, 0x00, 0xb2, 0x04, 0x2f, 0x45, 0x82, 0x00, 0x2e, 0x41, 0x40, 0xd1, 0x0f, 0x02, 0x2f, 0x06,
    0x30, 0x06, 0x43, 0x46, 0x43, 0x80, 0xb3, 0x11, 0x2f, 0x03, 0x43, 0x47, 0x43, 0xb8, 0x2e, 0xd7, 0x0e, 0x0c, 0x2f,
    0x43, 0x82, 0x17, 0x04, 0x41, 0x40, 0x41, 0x0f, 0x07, 0x2f, 0x03, 0x43, 0x42, 0x83, 0x47, 0x43, 0x00, 0x2e, 0x43,
    0x40, 0xc1, 0x86, 0x43, 0x42, 0xb8, 0x2e, 0xb8, 0x2e, 0x88, 0x88, 0xc0, 0x50, 0x39, 0x8d, 0x88, 0x81, 0xe2, 0x7f,
    0x02, 0x30, 0x05, 0x40, 0xf2, 0x7f, 0x41, 0xab, 0x22, 0x30, 0x95, 0x22, 0x86, 0x41, 0xd5, 0x5e, 0x9a, 0x00, 0x04,
    0x41, 0x37, 0x18, 0x66, 0x01, 0x2f, 0x57, 0xeb, 0x00, 0xc1, 0x7f, 0xb3, 0x7f, 0xd5, 0x7f, 0x48, 0x82, 0x39, 0x80,
    0x82, 0x40, 0xa0, 0x7f, 0x91, 0x7f, 0x8b, 0x7f, 0x06, 0x30, 0xd9, 0x5a, 0x31, 0x59, 0xd9, 0x56, 0x98, 0x2e, 0x67,
    0xcc, 0x91, 0x6f, 0x7b, 0x82, 0xc6, 0x6f, 0x42, 0x40, 0x81, 0x84, 0x42, 0x42, 0x43, 0x82, 0x83, 0x8b, 0x42, 0x40,
    0x86, 0x89, 0x7b, 0x82, 0x84, 0x87, 0x71, 0x7f, 0x80, 0xb2, 0x95, 0x7f, 0x64, 0x7f, 0x50, 0x7f, 0x02, 0x2f, 0xc5,
    0x40, 0x41, 0x8b, 0xc5, 0x42, 0x87, 0x89, 0x67, 0x40, 0x07, 0x0f, 0x85, 0x8b, 0x41, 0x7f, 0x82, 0x8d, 0x47, 0x40,
    0x3c, 0x2f, 0x81, 0x41, 0x01, 0x0e, 0xe1, 0x6f, 0x2d, 0x2f, 0x46, 0x8c, 0x90, 0x6f, 0x86, 0x41, 0x07, 0x40, 0xfe,
    0x0e, 0x02, 0x2f, 0x04, 0x41, 0x00, 0xb3, 0x21, 0x2f, 0x44, 0x82, 0x04, 0x30, 0x41, 0x40, 0x71, 0x00, 0xf9, 0x0e,
    0x36, 0x2f, 0x46, 0x41, 0x80, 0xa7, 0x01, 0x30, 0x04, 0x30, 0x0d, 0x2f, 0xe4, 0x6f, 0x00, 0x2e, 0x04, 0x41, 0x74,
    0x0f, 0x04, 0x30, 0x07, 0x2f, 0x80, 0x90, 0x00, 0x2f, 0xc1, 0x42, 0x03, 0x86, 0x81, 0x84, 0xc2, 0x42, 0x01, 0x42,
    0x14, 0x30, 0x42, 0x85, 0x41, 0x43, 0xa1, 0x42, 0x00, 0x2e, 0x82, 0x40, 0x80, 0x90, 0x1c, 0x2f, 0x01, 0x42, 0x1b,
    0x2d, 0x06, 0x42, 0x19, 0x2c, 0x04, 0x30, 0xb2, 0x6f, 0xba, 0x04, 0x02, 0x1e, 0x80, 0x43, 0x12, 0x30, 0xc0, 0x6f,
    0x23, 0x30, 0x98, 0x2e, 0x12, 0xb3, 0x0e, 0x2c, 0x04, 0x30, 0xb2, 0x6f, 0xc0, 0x6f, 0xba, 0x00, 0x51, 0x6f, 0x01,
    0x86, 0x4a, 0x1c, 0xc1, 0x42, 0x22, 0x30, 0xe1, 0x6f, 0x13, 0x30, 0x98, 0x2e, 0x12, 0xb3, 0x04, 0x30, 0x52, 0x6f,
    0x33, 0x53, 0x40, 0x6f, 0xc4, 0x7f, 0x98, 0x2e, 0xa0, 0xcc, 0x41, 0x6f, 0x70, 0x6f, 0x42, 0x40, 0x35, 0x53, 0x98,
    0x2e, 0xa0, 0xcc, 0x41, 0x6f, 0x70, 0x6f, 0x42, 0x40, 0x01, 0x40, 0xd3, 0x6f, 0xd3, 0x00, 0xcb, 0x1e, 0x35, 0x53,
    0x13, 0x42, 0xb0, 0x7f, 0x98, 0x2e, 0xa0, 0xcc, 0xb0, 0x6f, 0x3e, 0x84, 0xd1, 0x6f, 0x82, 0x40, 0x03, 0x40, 0x51,
    0x04, 0x59, 0x1c, 0x01, 0x42, 0x02, 0x82, 0xa0, 0x6f, 0x42, 0x40, 0x03, 0x40, 0xd3, 0x0f, 0x00, 0x30, 0x14, 0x30,
    0x60, 0x22, 0xc5, 0x6f, 0x40, 0x91, 0x01, 0x2f, 0x53, 0x0e, 0x26, 0x2f, 0xe5, 0x6f, 0x47, 0x85, 0x00, 0x2e, 0x83,
    0x40, 0x59, 0x0f, 0x20, 0x2f, 0x62, 0x6f, 0x4a, 0x8f, 0x85, 0x40, 0xc7, 0x41, 0x7f, 0x8d, 0x6f, 0x0f, 0xa6, 0x15,
    0x17, 0x30, 0x04, 0x30, 0x0f, 0x2f, 0xe0, 0x6f, 0x0b, 0x80, 0x00, 0x2e, 0x07, 0x40, 0x3e, 0x08, 0x00, 0xb2, 0x00,
    0x30, 0x06, 0x2f, 0x59, 0x1a, 0x04, 0x2f, 0xfd, 0x12, 0xc0, 0x90, 0x13, 0x30, 0x4b, 0x22, 0x06, 0x25, 0x71, 0x25,
    0xc0, 0xb3, 0x04, 0x2f, 0xa4, 0x42, 0xa4, 0x42, 0x83, 0x82, 0x84, 0x42, 0x44, 0x42, 0x00, 0x2e, 0x8b, 0x6f, 0x40,
    0x5f, 0xb8, 0x2e, 0xb0, 0x50, 0x80, 0x7f, 0xe7, 0x7f, 0xd5, 0x7f, 0xc4, 0x7f, 0xb3, 0x7f, 0xa2, 0x7f, 0x91, 0x7f,
    0xf6, 0x7f, 0x7b, 0x7f, 0x00, 0x2e, 0x01, 0x2e, 0x60, 0xf5, 0x60, 0x7f, 0x98, 0x2e, 0x2f, 0x03, 0x0b, 0x30, 0x66,
    0x6f, 0x5b, 0x25, 0x01, 0x32, 0xa9, 0x54, 0x37, 0x57, 0x14, 0x30, 0x6a, 0x2c, 0x60, 0x7f, 0xf1, 0x09, 0xc0, 0xb3,
    0x0b, 0x2f, 0x00, 0xb2, 0x03, 0x2f, 0x0f, 0x2e, 0x18, 0x00, 0xc0, 0x91, 0x03, 0x2f, 0xc2, 0x8e, 0x23, 0x2e, 0x64,
    0xf5, 0xcb, 0x43, 0x23, 0x2e, 0x60, 0xf5, 0xb2, 0x09, 0x80, 0xb3, 0x43, 0x2f, 0x00, 0xb2, 0x03, 0x2f, 0x0d, 0x2e,
    0x19, 0x00, 0x80, 0x91, 0x08, 0x2f, 0x0d, 0x2e, 0x90, 0x00, 0x80, 0x91, 0x31, 0x2f, 0x29, 0x2e, 0x90, 0x00, 0x37,
    0x2e, 0xc3, 0x00, 0x2d, 0x2d, 0x0b, 0x2e, 0xc3, 0x00, 0x41, 0x8b, 0x2b, 0x2e, 0xc3, 0x00, 0x6c, 0x09, 0x40, 0xb3,
    0x05, 0x2f, 0x0b, 0x2e, 0x58, 0xf5, 0xd8, 0xbe, 0xde, 0xba, 0x43, 0x91, 0x1d, 0x2f, 0x01, 0x2e, 0xc1, 0xf5, 0x8e,
    0xbc, 0x30, 0x30, 0x9e, 0xb8, 0x81, 0x04, 0x91, 0x52, 0x39, 0x51, 0x52, 0x7f, 0x98, 0x2e, 0xf1, 0x03, 0x01, 0x2e,
    0x92, 0x00, 0x01, 0x90, 0x3b, 0x51, 0x93, 0x52, 0x02, 0x2f, 0x52, 0x6f, 0x98, 0x2e, 0xf1, 0x03, 0x14, 0x30, 0x29,
    0x2e, 0xc2, 0x00, 0x0b, 0x30, 0x37, 0x2e, 0x90, 0x00, 0x60, 0x6f, 0x01, 0x32, 0xa9, 0x54, 0x37, 0x57, 0x15, 0x30,
    0x40, 0x91, 0x03, 0x2f, 0x37, 0x2e, 0x97, 0x00, 0x25, 0x2e, 0x64, 0xf5, 0x25, 0x2e, 0x60, 0xf5, 0x00, 0x90, 0x0e,
    0x2f, 0x37, 0x2e, 0x78, 0x00, 0xb3, 0x50, 0x21, 0x2e, 0x5a, 0xf2, 0x55, 0x7f, 0x98, 0x2e, 0x3c, 0xb2, 0x60, 0x6f,
    0x55, 0x6f, 0x01, 0x32, 0xa9, 0x54, 0x37, 0x57, 0x0b, 0x30, 0x14, 0x30, 0x0d, 0x2e, 0x60, 0xf5, 0xf1, 0x09, 0xc0,
    0x91, 0x93, 0x2f, 0xf2, 0x09, 0xc0, 0x91, 0x90, 0x2f, 0xf6, 0x6f, 0x80, 0x6f, 0x91, 0x6f, 0xa2, 0x6f, 0xb3, 0x6f,
    0xc4, 0x6f, 0xd5, 0x6f, 0xe7, 0x6f, 0x7b, 0x6f, 0x50, 0x5f, 0xc8, 0x2e, 0x3f, 0x51, 0x3d, 0x53, 0x11, 0x42, 0x01,
    0x30, 0xf3, 0x54, 0x12, 0x42, 0x11, 0x42, 0x11, 0x42, 0x11, 0x42, 0x11, 0x42, 0x11, 0x42, 0x11, 0x42, 0x73, 0x30,
    0xe3, 0x54, 0x01, 0x42, 0x83, 0x42, 0xb3, 0x86, 0x03, 0x2e, 0x25, 0xf3, 0xd4, 0x40, 0x9b, 0xbc, 0xc8, 0xbe, 0x9b,
    0xb4, 0x22, 0x80, 0xca, 0x8c, 0x17, 0xbe, 0xc3, 0x40, 0x14, 0x42, 0xda, 0xb6, 0xab, 0x85, 0x38, 0xbe, 0x15, 0x42,
    0x83, 0x82, 0x86, 0x41, 0xca, 0xb6, 0x82, 0x40, 0x68, 0xbf, 0x15, 0x42, 0x43, 0x86, 0xea, 0xb6, 0x2a, 0xbd, 0x41,
    0x40, 0xfa, 0x88, 0x2a, 0xb5, 0x15, 0x42, 0x12, 0x42, 0x9a, 0xbc, 0xc3, 0x40, 0x3a, 0xbd, 0x9a, 0xb4, 0x11, 0x42,
    0x2a, 0xb5, 0x12, 0x42, 0x3e, 0x83, 0x02, 0x41, 0x44, 0x40, 0x9d, 0x56, 0x45, 0x8c, 0xbe, 0x83, 0xd3, 0x09, 0x86,
    0x41, 0x45, 0x8a, 0xf6, 0xbb, 0x41, 0x40, 0x7e, 0x85, 0xf8, 0xbf, 0xb3, 0x09, 0x45, 0x41, 0x3c, 0x0b, 0x6b, 0x09,
    0x46, 0xbe, 0x66, 0xbb, 0x86, 0x86, 0xd6, 0xba, 0x46, 0xb6, 0x68, 0xbf, 0x82, 0x40, 0xd8, 0xbe, 0x14, 0x42, 0x71,
    0x0a, 0x2a, 0x0b, 0x96, 0xbc, 0x16, 0xb5, 0x12, 0x42, 0xcb, 0x82, 0xc3, 0x40, 0x46, 0xbe, 0x22, 0x30, 0x9a, 0x09,
    0x46, 0xb6, 0x14, 0x42, 0x13, 0x30, 0x6a, 0x8a, 0x44, 0x40, 0x31, 0x30, 0x21, 0x09, 0xc8, 0xbf, 0x61, 0xbb, 0x6a,
    0xbf, 0xb7, 0x0b, 0x47, 0x41, 0xb7, 0x0b, 0x4c, 0x89, 0x0b, 0x8b, 0x07, 0x41, 0xfb, 0x08, 0x6c, 0x89, 0x65, 0xbf,
    0x45, 0x41, 0x65, 0xb7, 0x87, 0x31, 0x6f, 0x09, 0x16, 0x42, 0x53, 0xbb, 0x09, 0x8b, 0xba, 0xbd, 0x68, 0xbf, 0x04,
    0x41, 0xde, 0x0a, 0xdc, 0x0a, 0xb5, 0xbd, 0x4b, 0x89, 0x35, 0xb7, 0x45, 0x41, 0x2f, 0x87, 0x16, 0x42, 0xaa, 0x08,
    0x04, 0x41, 0x06, 0x36, 0x66, 0x09, 0x21, 0xba, 0x55, 0xb9, 0xf6, 0x8a, 0xc3, 0x40, 0x4a, 0xbe, 0x28, 0xbd, 0xa2,
    0x0a, 0x93, 0x0a, 0x25, 0xbd, 0x57, 0x41, 0xa5, 0xb5, 0x54, 0x41, 0x46, 0x41, 0x13, 0x42, 0x53, 0x85, 0xb3, 0x56,
    0x95, 0x40, 0x69, 0x08, 0xfb, 0x09, 0x98, 0xbc, 0x4f, 0x0a, 0x96, 0xbc, 0x95, 0xb4, 0x11, 0x42, 0xc1, 0x30, 0x69,
    0x08, 0x23, 0x09, 0x96, 0xbc, 0x0c, 0x0b, 0x91, 0x40, 0x4b, 0x09, 0x46, 0xbe, 0xc5, 0xb4, 0x94, 0x40, 0x11, 0x42,
    0x63, 0x08, 0x94, 0x40, 0xb3, 0x09, 0xe3, 0x08, 0x82, 0x40, 0x34, 0x30, 0x14, 0x09, 0x48, 0xbe, 0x26, 0x0b, 0x9d,
    0x5c, 0x96, 0x09, 0x62, 0xbf, 0x75, 0x0b, 0xc6, 0x30, 0x96, 0x09, 0x07, 0x33, 0x97, 0x08, 0x24, 0xbd, 0x51, 0x0a,
    0x46, 0xbe, 0x45, 0xb6, 0x14, 0x42, 0x56, 0xbd, 0x25, 0xb5, 0x12, 0x42, 0x66, 0xbf, 0xf3, 0x0a, 0x96, 0xbc, 0xb6,
    0xbd, 0x95, 0xb4, 0x24, 0x34, 0x29, 0x2e, 0xd1, 0xf5, 0xb5, 0xb5, 0x11, 0x42, 0x82, 0x30, 0x25, 0x2e, 0x62, 0xf5,
    0x03, 0x42, 0xb8, 0x2e, 0x03, 0x2e, 0xdd, 0x00, 0x11, 0xb8, 0x12, 0x30, 0x4a, 0x08, 0x21, 0x2e, 0xdd, 0x00, 0x70,
    0x50, 0x40, 0xb2, 0x05, 0x2f, 0x03, 0x2e, 0xdd, 0x00, 0x43, 0x51, 0x08, 0x0c, 0x21, 0x2e, 0xdd, 0x00, 0x45, 0x55,
    0x01, 0x34, 0x94, 0x40, 0x96, 0x40, 0x90, 0x40, 0x82, 0x40, 0x51, 0x08, 0x40, 0x90, 0x05, 0x2f, 0x47, 0x51, 0x01,
    0x30, 0x11, 0x42, 0x11, 0x42, 0x01, 0x42, 0x1a, 0x2d, 0x33, 0x30, 0x53, 0x09, 0xc3, 0x30, 0xb3, 0x52, 0xd3, 0x08,
    0x07, 0x33, 0x21, 0x09, 0xb1, 0x09, 0x97, 0x08, 0x01, 0x08, 0xa4, 0xbc, 0xb6, 0xbd, 0xd8, 0xbe, 0x08, 0x0a, 0xde,
    0x0a, 0x2c, 0x0b, 0xb6, 0xbc, 0x46, 0xbe, 0x47, 0x55, 0xc6, 0xb5, 0x06, 0xbc, 0x96, 0xb4, 0x93, 0x42, 0x91, 0x42,
    0x06, 0xb4, 0x80, 0x42, 0x00, 0x2e, 0x05, 0x53, 0x00, 0x2e, 0x60, 0x40, 0x41, 0x40, 0x08, 0xbc, 0x41, 0x0a, 0x09,
    0x16, 0x3f, 0x90, 0x02, 0x2f, 0xf3, 0x50, 0x08, 0x1a, 0x5e, 0x2f, 0x41, 0x57, 0x11, 0xb4, 0xc2, 0x40, 0xd8, 0x86,
    0x02, 0x00, 0xe0, 0x42, 0x00, 0x2e, 0xc0, 0x40, 0x08, 0x1a, 0x54, 0x2f, 0x4b, 0x57, 0x49, 0x51, 0xe2, 0x40, 0xc4,
    0x40, 0xa8, 0xbe, 0xc3, 0x86, 0x6c, 0x0b, 0xe2, 0x40, 0x95, 0x7f, 0x11, 0x42, 0xc3, 0x8a, 0xc4, 0x40, 0x28, 0xbd,
    0x61, 0x41, 0x14, 0x0b, 0x42, 0x41, 0x98, 0xbc, 0x8a, 0x0a, 0x5a, 0x25, 0x01, 0x40, 0x79, 0x8b, 0x29, 0x86, 0xa4,
    0x7f, 0xd8, 0x80, 0xb2, 0x7f, 0xf5, 0x7f, 0xe1, 0x7f, 0xc9, 0x82, 0xc0, 0x7f, 0x7d, 0x84, 0xe0, 0x6f, 0x41, 0x40,
    0xf4, 0x6f, 0x01, 0x18, 0xd2, 0x7f, 0xf3, 0xbe, 0x11, 0x41, 0x6d, 0xbb, 0x82, 0x40, 0x75, 0x0b, 0xf4, 0x7f, 0x8a,
    0x17, 0x7d, 0xb6, 0xac, 0xbb, 0x64, 0xbf, 0xf7, 0x0b, 0xc6, 0x40, 0x37, 0x25, 0x24, 0xbd, 0x6a, 0x01, 0x06, 0x18,
    0x76, 0xbc, 0x6a, 0xb9, 0x10, 0x0a, 0x7a, 0xb5, 0x69, 0x18, 0xe3, 0x02, 0xd9, 0x18, 0x49, 0x16, 0xcd, 0x18, 0xf8,
    0xbd, 0x68, 0xba, 0x23, 0x0b, 0xf8, 0xb4, 0x9c, 0xbd, 0xc4, 0xba, 0xeb, 0x0a, 0x94, 0xb6, 0x58, 0x18, 0xe8, 0x18,
    0xd3, 0x18, 0xd2, 0x6f, 0xbb, 0x86, 0x78, 0xbc, 0xe8, 0xba, 0xa8, 0x0a, 0xc0, 0x6f, 0x94, 0x00, 0x12, 0x42, 0x78,
    0xb6, 0x61, 0x02, 0x4d, 0x55, 0x5a, 0x0e, 0x11, 0x42, 0xc5, 0x2f, 0x90, 0x5f, 0xb8, 0x2e, 0x60, 0x50, 0x20, 0x25,
    0x4d, 0x51, 0xf0, 0x7f, 0x15, 0x80, 0xdb, 0x7f, 0xe0, 0x7f, 0x3f, 0x82, 0xf3, 0x6f, 0xcf, 0x8a, 0xc0, 0x40, 0x83,
    0x40, 0x7e, 0x88, 0xc2, 0x7f, 0x8b, 0x16, 0x41, 0x40, 0xa4, 0x7f, 0x08, 0x18, 0x27, 0xbe, 0xb9, 0xb8, 0xe2, 0x6f,
    0xb5, 0x7f, 0x61, 0x0a, 0x7a, 0xbc, 0x66, 0xbb, 0x94, 0x40, 0x30, 0x0a, 0x0b, 0x2e, 0xc3, 0xf5, 0xb1, 0x25, 0x96,
    0x40, 0xf6, 0xb4, 0xdd, 0xbe, 0xdd, 0xba, 0x76, 0x25, 0xb7, 0xbd, 0x40, 0xb3, 0xe2, 0x7f, 0x6b, 0x25, 0x0b, 0x2f,
    0x50, 0xa1, 0x03, 0x2f, 0x70, 0x85, 0xe2, 0x15, 0x07, 0x2c, 0x04, 0x30, 0xbd, 0x14, 0x07, 0x31, 0xfd, 0x05, 0xe7,
    0x13, 0x25, 0x15, 0xd7, 0x0b, 0xdc, 0x00, 0xb7, 0x02, 0x3a, 0xba, 0xa6, 0xbe, 0x25, 0x0b, 0xaa, 0xb6, 0x60, 0x18,
    0xe8, 0x18, 0xcc, 0x18, 0xff, 0xbc, 0x61, 0xba, 0x61, 0x0a, 0x35, 0xbc, 0x25, 0xbe, 0x81, 0x00, 0xbb, 0xb9, 0x23,
    0x0a, 0xf1, 0xb4, 0xc1, 0x02, 0x99, 0x50, 0x10, 0x08, 0xc0, 0xa8, 0x07, 0x2f, 0xc0, 0x90, 0x01, 0x2f, 0x80, 0xaa,
    0x03, 0x2f, 0x00, 0x16, 0x01, 0x80, 0x02, 0x2c, 0x04, 0xbe, 0x04, 0xbe, 0xa0, 0x6f, 0xb4, 0xbc, 0x05, 0x40, 0x6c,
    0x0f, 0x31, 0x8a, 0x2c, 0xb9, 0x3c, 0xb4, 0x51, 0x0a, 0x0a, 0x2f, 0xc0, 0xa8, 0x05, 0x2f, 0xc0, 0xb2, 0x03, 0x2f,
    0x7f, 0x82, 0xf2, 0x3f, 0x04, 0x2c, 0x02, 0x02, 0x41, 0x82, 0x02, 0x30, 0x02, 0x02, 0x4f, 0x55, 0xbf, 0x84, 0xb7,
    0x6f, 0x82, 0x40, 0x29, 0xbd, 0x2f, 0xb9, 0x80, 0xb2, 0xf2, 0x6f, 0x04, 0x2f, 0x89, 0x84, 0x00, 0x2e, 0x83, 0x40,
    0x05, 0x2c, 0x8b, 0x16, 0x8c, 0x84, 0x00, 0x2e, 0x83, 0x40, 0x8b, 0x16, 0xb5, 0x7f, 0xf2, 0x8b, 0xc4, 0x41, 0x0d,
    0x2e, 0xc3, 0xf5, 0xdc, 0x00, 0xcc, 0x17, 0x6d, 0xbf, 0x6d, 0xba, 0x97, 0x02, 0x00, 0xb3, 0xf5, 0x7f, 0x0b, 0x2f,
    0x10, 0xa1, 0x03, 0x2f, 0x30, 0x85, 0x9a, 0x14, 0x07, 0x2c, 0x03, 0x30, 0x05, 0x31, 0x6c, 0x05, 0x5d, 0x13, 0x94,
    0x14, 0xdc, 0x14, 0x95, 0x0a, 0x4b, 0x00, 0x82, 0x02, 0xf5, 0x56, 0x98, 0x2e, 0x0c, 0xc1, 0xc2, 0x6f, 0xb3, 0x6f,
    0xf4, 0x6f, 0x90, 0x42, 0xd1, 0x82, 0x63, 0x0e, 0x90, 0x2e, 0x31, 0xb6, 0xdb, 0x6f, 0xa0, 0x5f, 0xb8, 0x2e, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x01, 0x2e, 0x5d, 0xf7, 0x08, 0xbc, 0x80, 0xac, 0x0e, 0xbb, 0x02, 0x2f,
    0x00, 0x30, 0x41, 0x04, 0x82, 0x06, 0xc0, 0xa4, 0x00, 0x30, 0x11, 0x2f, 0x40, 0xa9, 0x03, 0x2f, 0x40, 0x91, 0x0d,
    0x2f, 0x00, 0xa7, 0x0b, 0x2f, 0x80, 0xb3, 0xd9, 0x58, 0x02, 0x2f, 0x90, 0xa1, 0x26, 0x13, 0x20, 0x23, 0x80, 0x90,
    0x10, 0x30, 0x01, 0x2f, 0xcc, 0x0e, 0x00, 0x2f, 0x00, 0x30, 0xb8, 0x2e, 0xe1, 0x56, 0xdb, 0x54, 0xd0, 0x40, 0xc4,
    0x40, 0x0b, 0x2e, 0xfd, 0xf3, 0xe1, 0x52, 0x90, 0x42, 0x94, 0x42, 0x95, 0x42, 0x05, 0x30, 0xe3, 0x50, 0x0f, 0x88,
    0x06, 0x40, 0x04, 0x41, 0x96, 0x42, 0xc5, 0x42, 0x48, 0xbe, 0x73, 0x30, 0x0d, 0x2e, 0x89, 0x00, 0x4f, 0xba, 0x84,
    0x42, 0x03, 0x42, 0x81, 0xb3, 0x02, 0x2f, 0x2b, 0x2e, 0x6f, 0xf5, 0x06, 0x2d, 0x05, 0x2e, 0x77, 0xf7, 0xdf, 0x56,
    0x93, 0x08, 0x25, 0x2e, 0x77, 0xf7, 0xdd, 0x54, 0x25, 0x2e, 0xc2, 0xf5, 0x07, 0x2e, 0xfd, 0xf3, 0x42, 0x30, 0xb4,
    0x33, 0xda, 0x0a, 0x4c, 0x00, 0x27, 0x2e, 0xfd, 0xf3, 0x43, 0x40, 0xd4, 0x3f, 0xdc, 0x08, 0x43, 0x42, 0x00, 0x2e,
    0x00, 0x2e, 0x43, 0x40, 0x24, 0x30, 0xdc, 0x0a, 0x43, 0x42, 0x04, 0x80, 0x03, 0x2e, 0xfd, 0xf3, 0x4a, 0x0a, 0x23,
    0x2e, 0xfd, 0xf3, 0x61, 0x34, 0xc0, 0x2e, 0x01, 0x42, 0x00, 0x2e, 0xd0, 0x51, 0xfb, 0x7f, 0x98, 0x2e, 0xcf, 0x0d,
    0x5a, 0x25, 0x98, 0x2e, 0xf6, 0x0d, 0x6b, 0x87, 0xeb, 0x54, 0xe1, 0x7f, 0xa3, 0x7f, 0xb3, 0x7f, 0xb2, 0x88, 0xe5,
    0x52, 0xc2, 0x7f, 0x65, 0x8b, 0xe7, 0x56, 0x84, 0x7f, 0x61, 0x7f, 0x75, 0x7f, 0xd0, 0x7f, 0x95, 0x7f, 0x53, 0x7f,
    0x14, 0x30, 0xe9, 0x54, 0x81, 0x6f, 0x42, 0x7f, 0x00, 0x2e, 0x53, 0x40, 0x45, 0x8c, 0x42, 0x40, 0x90, 0x41, 0xbb,
    0x83, 0x86, 0x41, 0xd8, 0x04, 0x16, 0x06, 0x00, 0xac, 0x81, 0x7f, 0x02, 0x2f, 0x02, 0x30, 0xd3, 0x04, 0x10, 0x06,
    0xc1, 0x84, 0x01, 0x30, 0xc1, 0x02, 0x0b, 0x16, 0x04, 0x09, 0x14, 0x01, 0x99, 0x02, 0xc1, 0xb9, 0xaf, 0xbc, 0x59,
    0x0a, 0x64, 0x6f, 0x51, 0x43, 0xa1, 0xb4, 0x12, 0x41, 0x13, 0x41, 0x41, 0x43, 0x35, 0x7f, 0x64, 0x7f, 0x26, 0x31,
    0xe5, 0x6f, 0xd4, 0x6f, 0x98, 0x2e, 0x37, 0xca, 0x32, 0x6f, 0x75, 0x6f, 0x83, 0x40, 0x42, 0x41, 0x23, 0x7f, 0x12,
    0x7f, 0xf6, 0x30, 0x40, 0x25, 0x51, 0x25, 0x98, 0x2e, 0x37, 0xca, 0x14, 0x6f, 0x20, 0x05, 0x70, 0x6f, 0x25, 0x6f,
    0x69, 0x07, 0xa2, 0x6f, 0x31, 0x6f, 0x0b, 0x30, 0x04, 0x42, 0x9b, 0x42, 0x8b, 0x42, 0x55, 0x42, 0x32, 0x7f, 0x40,
    0xa9, 0xc3, 0x6f, 0x71, 0x7f, 0x02, 0x30, 0xd0, 0x40, 0xc3, 0x7f, 0x03, 0x2f, 0x40, 0x91, 0x15, 0x2f, 0x00, 0xa7,
    0x13, 0x2f, 0x00, 0xa4, 0x11, 0x2f, 0x84, 0xbd, 0x98, 0x2e, 0x79, 0xca, 0x55, 0x6f, 0xf3, 0x54, 0x54, 0x41, 0x82,
    0x00, 0xf3, 0x3f, 0x45, 0x41, 0xcb, 0x02, 0xf6, 0x30, 0x98, 0x2e, 0x37, 0xca, 0x33, 0x6f, 0xa4, 0x6f, 0xc1, 0x42,
    0x03, 0x2c, 0x00, 0x43, 0xa4, 0x6f, 0x33, 0x6f, 0x00, 0x2e, 0x42, 0x6f, 0x55, 0x6f, 0x91, 0x40, 0x42, 0x8b, 0x00,
    0x41, 0x41, 0x00, 0x01, 0x43, 0x55, 0x7f, 0x14, 0x30, 0xc1, 0x40, 0x95, 0x40, 0x4d, 0x02, 0xc5, 0x6f, 0xf1, 0x50,
    0x68, 0x0e, 0x75, 0x6f, 0xd1, 0x42, 0xa3, 0x7f, 0x8a, 0x2f, 0x09, 0x2e, 0x89, 0x00, 0x01, 0xb3, 0x22, 0x2f, 0xeb,
    0x58, 0x90, 0x6f, 0x17, 0x30, 0x13, 0x41, 0xb6, 0x6f, 0xe4, 0x7f, 0x00, 0x2e, 0x91, 0x41, 0x14, 0x40, 0x92, 0x41,
    0x15, 0x40, 0x17, 0x2e, 0x6f, 0xf5, 0xb6, 0x7f, 0xd0, 0x7f, 0xcb, 0x7f, 0x98, 0x2e, 0x00, 0x0c, 0x07, 0x15, 0xc2,
    0x6f, 0x14, 0x0b, 0x29, 0x2e, 0x6f, 0xf5, 0xc3, 0xa3, 0xc1, 0x8f, 0xe4, 0x6f, 0xd0, 0x6f, 0xe6, 0x2f, 0x14, 0x30,
    0x05, 0x2e, 0x6f, 0xf5, 0x14, 0x0b, 0x29, 0x2e, 0x6f, 0xf5, 0x44, 0x2d, 0xed, 0x54, 0x01, 0x32, 0xf3, 0x58, 0x05,
    0x30, 0xb3, 0x50, 0x83, 0x40, 0xd8, 0x08, 0x91, 0x01, 0xb8, 0xbd, 0x38, 0xb5, 0xe6, 0x7f, 0x0a, 0x16, 0xb1, 0x6f,
    0x2a, 0xbb, 0xa6, 0xbd, 0x1c, 0x01, 0x06, 0xbc, 0x52, 0x40, 0x06, 0x0a, 0x53, 0x40, 0x45, 0x03, 0xb1, 0x7f, 0xf6,
    0x30, 0x98, 0x2e, 0x37, 0xca, 0x1a, 0xbd, 0x16, 0xb6, 0x86, 0xba, 0x00, 0xa9, 0xaa, 0x0a, 0xf5, 0x52, 0x0f, 0x2f,
    0x00, 0x91, 0xf5, 0x52, 0x03, 0x2f, 0xf5, 0x5a, 0x55, 0x0f, 0xf5, 0x52, 0x08, 0x2f, 0x3f, 0xa1, 0x04, 0x2f, 0x3f,
    0x91, 0x03, 0x2f, 0xf3, 0x58, 0xd4, 0x0f, 0x00, 0x2f, 0xf3, 0x54, 0x12, 0x25, 0xf2, 0x33, 0x98, 0x2e, 0xd9, 0xc0,
    0xe4, 0x6f, 0xf5, 0x37, 0x45, 0x09, 0x21, 0x85, 0x05, 0x43, 0x05, 0x30, 0xef, 0x52, 0x51, 0x0e, 0x01, 0x32, 0xf3,
    0x58, 0xc5, 0x2f, 0xa9, 0x54, 0x09, 0x2e, 0x77, 0xf7, 0x22, 0x0b, 0x29, 0x2e, 0x77, 0xf7, 0xfb, 0x6f, 0x30, 0x5e,
    0xb8, 0x2e, 0x10, 0x50, 0x01, 0x2e, 0x85, 0x00, 0x00, 0xb2, 0xfb, 0x7f, 0x5d, 0x2f, 0x01, 0xb2, 0x54, 0x2f, 0x02,
    0xb2, 0x4e, 0x2f, 0x03, 0x90, 0x63, 0x2f, 0xfb, 0x50, 0x39, 0x82, 0x02, 0x40, 0x81, 0x88, 0xfd, 0x54, 0x41, 0x40,
    0x03, 0x57, 0x04, 0x42, 0x00, 0x2e, 0x94, 0x40, 0x95, 0x40, 0xd8, 0xbe, 0x2c, 0x0b, 0x45, 0x40, 0x6c, 0x01, 0x55,
    0x42, 0x0c, 0x17, 0x45, 0x40, 0x2c, 0x03, 0x54, 0x42, 0x53, 0x0e, 0xf2, 0x2f, 0x05, 0x57, 0x3e, 0x82, 0xe2, 0x40,
    0xc3, 0x40, 0x28, 0xbd, 0x93, 0x0a, 0x43, 0x40, 0xda, 0x00, 0x53, 0x42, 0x8a, 0x16, 0x43, 0x40, 0x9a, 0x02, 0x52,
    0x42, 0x00, 0x2e, 0x41, 0x40, 0xa9, 0x54, 0x4a, 0x0e, 0x3b, 0x2f, 0x3a, 0x82, 0x00, 0x30, 0x41, 0x40, 0x21, 0x2e,
    0x6a, 0x0f, 0x40, 0xb2, 0x0a, 0x2f, 0x98, 0x2e, 0x61, 0x0c, 0x98, 0x2e, 0x2b, 0x0e, 0x98, 0x2e, 0x41, 0x0e, 0xfb,
    0x6f, 0xf0, 0x5f, 0x00, 0x30, 0x80, 0x2e, 0x78, 0xb2, 0x01, 0x55, 0xf7, 0x56, 0x83, 0x42, 0x8f, 0x86, 0x74, 0x30,
    0xff, 0x54, 0xc4, 0x42, 0x11, 0x30, 0x23, 0x2e, 0x85, 0x00, 0xa1, 0x42, 0x23, 0x30, 0x27, 0x2e, 0x88, 0x00, 0x21,
    0x2e, 0x87, 0x00, 0xba, 0x82, 0x18, 0x2c, 0x81, 0x42, 0x30, 0x30, 0x21, 0x2e, 0x85, 0x00, 0x13, 0x2d, 0x21, 0x30,
    0x00, 0x30, 0x23, 0x2e, 0x85, 0x00, 0x21, 0x2e, 0x7b, 0xf7, 0x0c, 0x2d, 0x77, 0x30, 0x98, 0x2e, 0x1f, 0x0c, 0xf9,
    0x50, 0x0c, 0x82, 0x12, 0x30, 0x40, 0x42, 0x25, 0x2e, 0x85, 0x00, 0x2f, 0x2e, 0x7b, 0xf7, 0xfb, 0x6f, 0xf0, 0x5f,
    0xb8, 0x2e, 0x70, 0x50, 0x0a, 0x25, 0x39, 0x86, 0xfb, 0x7f, 0xe1, 0x32, 0x62, 0x30, 0x98, 0x2e, 0xc2, 0xc4, 0xb3,
    0x56, 0xa5, 0x6f, 0xab, 0x08, 0x91, 0x6f, 0x4b, 0x08, 0x07, 0x57, 0xc4, 0x6f, 0x23, 0x09, 0x4d, 0xba, 0x93, 0xbc,
    0x8c, 0x0b, 0xd1, 0x6f, 0x0b, 0x09, 0xeb, 0x52, 0x09, 0x5f, 0x56, 0x42, 0xaf, 0x09, 0x4d, 0xba, 0x23, 0xbd, 0x94,
    0x0a, 0xe5, 0x6f, 0x68, 0xbb, 0xeb, 0x08, 0xbd, 0xb9, 0x63, 0xbe, 0xfb, 0x6f, 0x52, 0x42, 0xe3, 0x0a, 0xc0, 0x2e,
    0x43, 0x42, 0x90, 0x5f, 0xf1, 0x50, 0x03, 0x2e, 0x25, 0xf3, 0x12, 0x40, 0x00, 0x40, 0x28, 0xba, 0x9b, 0xbc, 0x88,
    0xbd, 0x93, 0xb4, 0xe3, 0x0a, 0x89, 0x16, 0x08, 0xb6, 0xc0, 0x2e, 0x19, 0x00, 0x62, 0x02, 0x10, 0x50, 0xfb, 0x7f,
    0x98, 0x2e, 0x5d, 0x0d, 0x01, 0x2e, 0x85, 0x00, 0x31, 0x30, 0x08, 0x04, 0xfb, 0x6f, 0x01, 0x30, 0xf0, 0x5f, 0x23,
    0x2e, 0x87, 0x00, 0x21, 0x2e, 0x88, 0x00, 0xb8, 0x2e, 0x01, 0x2e, 0x88, 0x00, 0x03, 0x2e, 0x87, 0x00, 0x48, 0x0e,
    0x01, 0x2f, 0x80, 0x2e, 0x05, 0x0e, 0xb8, 0x2e, 0x0b, 0x51, 0x21, 0x34, 0x01, 0x42, 0x82, 0x30, 0xc1, 0x32, 0x25,
    0x2e, 0x62, 0xf5, 0x01, 0x00, 0x22, 0x30, 0x01, 0x40, 0x4a, 0x0a, 0x01, 0x42, 0xb8, 0x2e, 0x0b, 0x55, 0xf0, 0x3b,
    0x83, 0x40, 0xd8, 0x08, 0x0d, 0x53, 0x83, 0x42, 0x00, 0x30, 0x83, 0x30, 0x50, 0x42, 0xc4, 0x32, 0x27, 0x2e, 0x64,
    0xf5, 0x94, 0x00, 0x50, 0x42, 0x40, 0x42, 0xd3, 0x3f, 0x84, 0x40, 0x7d, 0x82, 0xe3, 0x08, 0x40, 0x42, 0x83, 0x42,
    0xb8, 0x2e, 0x01, 0x53, 0x00, 0x30, 0x40, 0x42, 0x7c, 0x86, 0xdb, 0x52, 0x09, 0x2e, 0x55, 0x0f, 0xe1, 0x54, 0xc4,
    0x42, 0xd3, 0x86, 0x54, 0x40, 0x55, 0x40, 0x94, 0x42, 0x85, 0x42, 0x21, 0x2e, 0x88, 0x00, 0x42, 0x40, 0x25, 0x2e,
    0xfd, 0xf3, 0xc0, 0x42, 0x7e, 0x82, 0x05, 0x2e, 0x7c, 0x00, 0x80, 0xb2, 0x14, 0x2f, 0x05, 0x2e, 0x09, 0x01, 0x27,
    0xbd, 0x2f, 0xb9, 0x80, 0x90, 0x02, 0x2f, 0x21, 0x2e, 0x6f, 0xf5, 0x0c, 0x2d, 0x07, 0x2e, 0x56, 0x0f, 0x14, 0x30,
    0x1c, 0x09, 0x05, 0x2e, 0x77, 0xf7, 0xdf, 0x56, 0x47, 0xbe, 0x93, 0x08, 0x94, 0x0a, 0x25, 0x2e, 0x77, 0xf7, 0x0f,
    0x55, 0x50, 0x42, 0x4a, 0x0e, 0xfc, 0x2f, 0xb8, 0x2e, 0x50, 0x50, 0x02, 0x30, 0x43, 0x86, 0x0d, 0x51, 0xfb, 0x7f,
    0xe3, 0x7f, 0xd2, 0x7f, 0xc0, 0x7f, 0xb1, 0x7f, 0x00, 0x2e, 0x41, 0x40, 0x00, 0x40, 0x48, 0x04, 0x98, 0x2e, 0x74,
    0xc0, 0x1e, 0xaa, 0xd3, 0x6f, 0x14, 0x30, 0xb1, 0x6f, 0xe3, 0x22, 0xc0, 0x6f, 0x52, 0x40, 0xe4, 0x6f, 0x4c, 0x0e,
    0x12, 0x42, 0xd3, 0x7f, 0xeb, 0x2f, 0x03, 0x2e, 0x6b, 0x0f, 0x40, 0x90, 0x11, 0x30, 0x03, 0x2f, 0x23, 0x2e, 0x6b,
    0x0f, 0x02, 0x2c, 0x00, 0x30, 0xd0, 0x6f, 0xfb, 0x6f, 0xb0, 0x5f, 0xb8, 0x2e, 0x40, 0x50, 0xf1, 0x7f, 0x0a, 0x25,
    0x3c, 0x86, 0xeb, 0x7f, 0x41, 0x33, 0x22, 0x30, 0x98, 0x2e, 0xc2, 0xc4, 0xd3, 0x6f, 0xf4, 0x30, 0xdc, 0x09, 0x13,
    0x59, 0xc2, 0x6f, 0x94, 0x09, 0x15, 0x59, 0x6a, 0xbb, 0xdc, 0x08, 0xb4, 0xb9, 0xb1, 0xbd, 0x11, 0x5b, 0x95, 0x08,
    0x21, 0xbd, 0xf6, 0xbf, 0x77, 0x0b, 0x51, 0xbe, 0xf1, 0x6f, 0xeb, 0x6f, 0x52, 0x42, 0x54, 0x42, 0xc0, 0x2e, 0x43,
    0x42, 0xc0, 0x5f, 0x50, 0x50, 0x1f, 0x51, 0x51, 0x30, 0x11, 0x42, 0xfb, 0x7f, 0x7b, 0x30, 0x0b, 0x42, 0x11, 0x30,
    0x02, 0x80, 0x23, 0x33, 0x01, 0x42, 0x03, 0x00, 0x07, 0x2e, 0x80, 0x03, 0x05, 0x2e, 0x84, 0x00, 0xb7, 0x52, 0xe2,
    0x7f, 0xd3, 0x7f, 0xc0, 0x7f, 0x98, 0x2e, 0x9c, 0x0e, 0xd1, 0x6f, 0x08, 0x0a, 0x1a, 0x25, 0x7b, 0x86, 0xd0, 0x7f,
    0x01, 0x33, 0x12, 0x30, 0x98, 0x2e, 0xc2, 0xc4, 0xd1, 0x6f, 0x08, 0x0a, 0x00, 0xb2, 0x0d, 0x2f, 0xe3, 0x6f, 0x01,
    0x2e, 0x80, 0x03, 0x51, 0x30, 0xc7, 0x86, 0x23, 0x2e, 0x21, 0xf2, 0x08, 0xbc, 0xc0, 0x42, 0x98, 0x2e, 0x4f, 0xb2,
    0x00, 0x2e, 0x00, 0x2e, 0xd0, 0x2e, 0xb0, 0x6f, 0x0b, 0xb8, 0x03, 0x2e, 0x1b, 0x00, 0x08, 0x1a, 0xb0, 0x7f, 0x70,
    0x30, 0x04, 0x2f, 0x21, 0x2e, 0x21, 0xf2, 0x00, 0x2e, 0x00, 0x2e, 0xd0, 0x2e, 0x98, 0x2e, 0x6d, 0xc0, 0x98, 0x2e,
    0x5d, 0xc0, 0x17, 0x51, 0x98, 0x2e, 0x44, 0xcb, 0x19, 0x51, 0x98, 0x2e, 0x46, 0xc3, 0x1b, 0x51, 0x98, 0x2e, 0x53,
    0xc7, 0x20, 0x26, 0xc0, 0x6f, 0x01, 0x31, 0x11, 0x42, 0xe1, 0x33, 0x01, 0x42, 0x37, 0x80, 0xe0, 0x7f, 0x98, 0x2e,
    0xb5, 0xb4, 0xe0, 0x6f, 0x0b, 0x30, 0x0b, 0x42, 0x01, 0x31, 0x21, 0x51, 0xf3, 0x37, 0x02, 0x40, 0x51, 0x0a, 0x01,
    0x42, 0x8b, 0x31, 0x09, 0x2e, 0x5e, 0xf7, 0x23, 0x55, 0xe3, 0x08, 0x25, 0x53, 0x83, 0x42, 0x5b, 0x42, 0x23, 0x33,
    0x03, 0x00, 0xbc, 0x84, 0x4b, 0x40, 0x33, 0x30, 0x83, 0x42, 0x4b, 0x42, 0xe1, 0x7f, 0xd0, 0x7f, 0x98, 0x2e, 0xfd,
    0xb1, 0xd1, 0x6f, 0x80, 0x30, 0x40, 0x42, 0x03, 0x30, 0xe0, 0x6f, 0x1d, 0x55, 0x04, 0x30, 0x00, 0x2e, 0x00, 0x2e,
    0x01, 0x89, 0x62, 0x0e, 0xfa, 0x2f, 0x43, 0x42, 0x11, 0x30, 0xfb, 0x6f, 0xc0, 0x2e, 0x01, 0x42, 0xb0, 0x5f, 0xc1,
    0x4a, 0x00, 0x00, 0x6d, 0x57, 0x00, 0x00, 0x77, 0x8e, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xd3, 0xff, 0xff, 0xff,
    0xe5, 0xff, 0xff, 0xff, 0xee, 0xe1, 0xff, 0xff, 0x7c, 0x13, 0x00, 0x00, 0x46, 0xe6, 0xff, 0xff, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0xfd, 0x2d
};

/*! @name  Global array that stores the feature input configuration of BMI270_H */
const struct bmi2_feature_config bmi270_h_feat_in[BMI270_H_MAX_FEAT_IN] = {
    { .type = BMI2_CONFIG_ID, .page = BMI2_PAGE_1, .start_addr = BMI270_H_CONFIG_ID_STRT_ADDR },
    { .type = BMI2_MAX_BURST_LEN, .page = BMI2_PAGE_1, .start_addr = BMI270_H_MAX_BURST_LEN_STRT_ADDR },
    { .type = BMI2_CRT_GYRO_SELF_TEST, .page = BMI2_PAGE_1, .start_addr = BMI270_H_CRT_GYRO_SELF_TEST_STRT_ADDR },
    { .type = BMI2_ABORT_CRT_GYRO_SELF_TEST, .page = BMI2_PAGE_1, .start_addr = BMI270_H_ABORT_STRT_ADDR },
    { .type = BMI2_AXIS_MAP, .page = BMI2_PAGE_1, .start_addr = BMI270_H_AXIS_MAP_STRT_ADDR },
    { .type = BMI2_NVM_PROG_PREP, .page = BMI2_PAGE_1, .start_addr = BMI270_H_NVM_PROG_PREP_STRT_ADDR },
    { .type = BMI2_ANY_MOTION, .page = BMI2_PAGE_1, .start_addr = BMI270_H_ANY_MOT_STRT_ADDR },
    { .type = BMI2_GYRO_GAIN_UPDATE, .page = BMI2_PAGE_1, .start_addr = BMI270_H_GYRO_GAIN_UPDATE_STRT_ADDR },
    { .type = BMI2_GYRO_SELF_OFF, .page = BMI2_PAGE_1, .start_addr = BMI270_H_GYRO_SELF_OFF_STRT_ADDR },
    { .type = BMI2_NO_MOTION, .page = BMI2_PAGE_5, .start_addr = BMI270_H_NO_MOT_STRT_ADDR },
    { .type = BMI2_STEP_COUNTER_PARAMS, .page = BMI2_PAGE_2, .start_addr = BMI270_H_STEP_CNTER_PARAMS_START_ADDR },
    { .type = BMI2_STEP_COUNTER, .page = BMI2_PAGE_5, .start_addr = BMI270_H_STEP_CNT_4_STRT_ADDR },
    { .type = BMI2_STEP_DETECTOR, .page = BMI2_PAGE_5, .start_addr = BMI270_H_STEP_CNT_4_STRT_ADDR },
    { .type = BMI2_STEP_ACTIVITY, .page = BMI2_PAGE_5, .start_addr = BMI270_H_STEP_CNT_4_STRT_ADDR },
    { .type = BMI2_SINGLE_TAP, .page = BMI2_PAGE_6, .start_addr = BMI270_H_TAP_DETECT_1_START_ADDR },
    { .type = BMI2_DOUBLE_TAP, .page = BMI2_PAGE_6, .start_addr = BMI270_H_TAP_DETECT_1_START_ADDR },
    { .type = BMI2_TRIPLE_TAP, .page = BMI2_PAGE_6, .start_addr = BMI270_H_TAP_DETECT_1_START_ADDR },
    { .type = BMI2_TAP_DETECTOR_1, .page = BMI2_PAGE_6, .start_addr = BMI270_H_TAP_DETECT_1_START_ADDR },
    { .type = BMI2_TAP_DETECTOR_2, .page = BMI2_PAGE_7, .start_addr = BMI270_H_TAP_DETECT_2_START_ADDR }
};

/*! @name  Global array that stores the feature output configuration */
const struct bmi2_feature_config bmi270_h_feat_out[BMI270_H_MAX_FEAT_OUT] = {
    { .type = BMI2_STEP_COUNTER, .page = BMI2_PAGE_0, .start_addr = BMI270_H_STEP_CNT_OUT_STRT_ADDR },
    { .type = BMI2_STEP_ACTIVITY, .page = BMI2_PAGE_0, .start_addr = BMI270_H_STEP_ACT_OUT_STRT_ADDR },
    { .type = BMI2_GYRO_GAIN_UPDATE, .page = BMI2_PAGE_0, .start_addr = BMI270_H_GYR_USER_GAIN_OUT_STRT_ADDR },
    { .type = BMI2_GYRO_CROSS_SENSE, .page = BMI2_PAGE_0, .start_addr = BMI270_H_GYRO_CROSS_SENSE_STRT_ADDR },
    { .type = BMI2_NVM_STATUS, .page = BMI2_PAGE_0, .start_addr = BMI270_H_NVM_VFRM_OUT_STRT_ADDR },
    { .type = BMI2_VFRM_STATUS, .page = BMI2_PAGE_0, .start_addr = BMI270_H_NVM_VFRM_OUT_STRT_ADDR }
};

/*! @name  Global array that stores the feature interrupts of BMI270_H */
struct bmi2_map_int bmi270_h_map_int[BMI270_H_MAX_INT_MAP] = {
    { .type = BMI2_STEP_COUNTER, .sens_map_int = BMI270_H_INT1_STEP_COUNTER_MASK },
    { .type = BMI2_STEP_DETECTOR, .sens_map_int = BMI270_H_INT1_STEP_DETECTION_MASK },
    { .type = BMI2_STEP_ACTIVITY, .sens_map_int = BMI270_H_INT1_STEP_ACTIVITY_MASK },
    { .type = BMI2_TAP, .sens_map_int = BMI270_H_INT1_TAP_DETECTION_MASK },
    { .type = BMI2_SINGLE_TAP, .sens_map_int = BMI270_H_INT1_TAP_DETECTION_MASK },
    { .type = BMI2_DOUBLE_TAP, .sens_map_int = BMI270_H_INT1_TAP_DETECTION_MASK },
    { .type = BMI2_TRIPLE_TAP, .sens_map_int = BMI270_H_INT1_TAP_DETECTION_MASK },
    { .type = BMI2_NO_MOTION, .sens_map_int = BMI270_H_INT1_NO_MOTION_MASK },
    { .type = BMI2_ANY_MOTION, .sens_map_int = BMI270_H_INT1_ANY_MOTION_MASK }
};

/******************************************************************************/

/*!         Local Function Prototypes
 ******************************************************************************/

/*!
 * @brief This internal API gets tap configurations like data register enable,
 * tap sensitivity threshold, max gesture duration, quite time after gesture,
 * axis selection and wait for timeout.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_tap_config(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API gets tap configurations like data register enable,
 * tap sensor threshold, maximum gesture duration, tap shock duration,
 * time after gesture and output-configuration from primary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 *--------------------------|--------------------------------------------------
 *                          | By enabling this bit, accel data according to the
 *  data_reg_en             | user defined accel configuration is taken for tap
 *                          | detector feature.
 * -------------------------|---------------------------------------------------
 *  tap_sens_thres          | Scaling factor of additional threshold increment
 *                          | for detection of positive and negative peak of tap.
 * -------------------------|---------------------------------------------------
 *  max_gest_dur            | Maximum duration after the first tap within which the
 *                          | second and/or third tap have to be performed for being
 *                          | detected as double-tap.
 * -------------------------|---------------------------------------------------
 *  quite_time_after_gest   | Minimum quite time between the two gesture detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_config_page_primary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API gets tap configurations like time out and
 * axis selection from secondary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 * -------------------------|---------------------------------------------------
 *  wait_for_timeout        | Wait for the duration set by max_gest_dur after the
 *                          | first tap and report the tap-gesture based on number
 *                          | of taps detected.
 * -------------------------|---------------------------------------------------
 *                          | Selection of axis from 3D-acceleration signal vector
 *  axis_sel                | for tap detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_config_page_secondary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API sets tap configurations like data register enable,
 * tap sensitivity threshold, max gesture duration, quite time after gesture,
 * axis selection and wait for timeout.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_tap_config(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API sets the tap configurations like data register enable,
 * tap sensor threshold, maximum gesture duration, tap shock duration,
 * time after gesture and output-configuration to primary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 *--------------------------|--------------------------------------------------
 *                          | By enabling this bit, accel data according to the
 *  data_reg_en             | user defined accel configuration is taken for tap
 *                          | detector feature.
 * -------------------------|---------------------------------------------------
 *  tap_sens_thres          | Scaling factor of additional threshold increment
 *                          | for detection of positive and negative peak of tap.
 * -------------------------|---------------------------------------------------
 *  max_gest_dur            | Maximum duration after the first tap within which the
 *                          | second and/or third tap have to be performed for being
 *                          | detected as double-tap.
 * -------------------------|---------------------------------------------------
 *  quite_time_after_gest   | Minimum quite time between the two gesture detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_config_page_primary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API sets the tap configurations like time out and
 * axis selection to secondary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 * -------------------------|---------------------------------------------------
 *  wait_for_timeout        | Wait for the duration set by max_gest_dur after the
 *                          | first tap and report the tap-gesture based on number
 *                          | of taps detected.
 * -------------------------|---------------------------------------------------
 *                          | Selection of axis from 3D-acceleration signal vector
 *  axis_sel                | for tap detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_config_page_secondary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API is used to validate the device pointer for
 * null conditions.
 *
 * @param[in] dev : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t null_ptr_check(const struct bmi2_dev *dev);

/*!
 * @brief This internal API enables the selected sensor/features.
 *
 * @param[in]       sensor_sel    : Selects the desired sensor.
 * @param[in, out]  dev           : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t sensor_enable(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API disables the selected sensor/features.
 *
 * @param[in]       sensor_sel    : Selects the desired sensor.
 * @param[in, out]  dev           : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t sensor_disable(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API selects the sensors/features to be enabled or
 * disabled.
 *
 * @param[in]  sens_list    : Pointer to select the sensor.
 * @param[in]  n_sens       : Number of sensors selected.
 * @param[out] sensor_sel   : Gets the selected sensor.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t select_sensor(const uint8_t *sens_list, uint8_t n_sens, uint64_t *sensor_sel);

/*!
 * @brief This internal API is used to enable/disable any-motion feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables any-motion.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables any-motion.
 * BMI2_ENABLE  | Enables any-motion.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_any_motion(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable no-motion feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables no-motion.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables no-motion.
 * BMI2_ENABLE  | Enables no-motion.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_no_motion(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable step detector feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables step-detector.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables step detector
 * BMI2_ENABLE  | Enables step detector
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_detector(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable step counter feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables step counter.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables step counter
 * BMI2_ENABLE  | Enables step counter
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_counter(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * single tap.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables single tap.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables single tap.
 * BMI2_ENABLE  | Enables single tap
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_single_tap(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * double tap.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables double tap.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables double tap.
 * BMI2_ENABLE  | Enables double tap
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_double_tap(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * triple tap.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables triple tap.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables triple tap.
 * BMI2_ENABLE  | Enables triple tap
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_triple_tap(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable gyroscope user gain
 * feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables gyroscope user gain.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables gyroscope user gain
 * BMI2_ENABLE  | Enables gyroscope user gain
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_gyro_user_gain(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[in]      config      : Structure instance of bmi2_any_motion_config.
 * @param[in, out] dev         : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_any_motion_config  |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for in 5.11g format.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0xAA = 83mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_any_motion_config(const struct bmi2_any_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[in]      config       : Structure instance of bmi2_no_motion_config.
 * @param[in, out] dev          : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_no_motion_config   |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for no-motion detection.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0x90 = 70mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_no_motion_config(const struct bmi2_no_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 *
 * @param[in]      config         : Structure instance of bmi2_tap_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[out] config        : Structure instance of bmi2_any_motion_config.
 * @param[in, out]  dev      : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_any_motion_config  |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for in 5.11g format.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0xAA = 83mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_any_motion_config(struct bmi2_any_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[out]      config    : Structure instance of bmi2_no_motion_config.
 * @param[in, out]  dev       : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_no_motion_config   |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for no-motion detection.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0x90 = 70mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_no_motion_config(struct bmi2_no_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 *
 * @param[out]      config    : Structure instance of bmi2_tap_config.
 * @param[in, out]  dev       : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the output values of step activity.
 *
 * @param[out] step_act         : Pointer to the stored step activity data.
 * @param[in]  dev      : Structure instance of bmi2_dev.
 *
 * *step_act  |  Output
 * -----------|------------
 * 0x00       |  STILL
 * 0x01       |  WALKING
 * 0x02       |  RUNNING
 * 0x03       |  UNKNOWN
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_step_activity_output(uint8_t *step_act, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the output values of step counter.
 *
 * @param[out] step_count       : Pointer to the stored step counter data.
 * @param[in]  dev              : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_step_counter_output(uint32_t *step_count, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the error status related to NVM.
 *
 * @param[out] nvm_err_stat     : Stores the NVM error status.
 * @param[in]  dev              : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_nvm_error_status(struct bmi2_nvm_err_status *nvm_err_stat, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the error status related to virtual frames.
 *
 * @param[out] vfrm_err_stat    : Stores the VFRM related error status.
 * @param[in]  dev              : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_vfrm_error_status(struct bmi2_vfrm_err_status *vfrm_err_stat, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to get enable status of gyroscope user gain
 * update.
 *
 * @param[out] status         : Stores status of gyroscope user gain update.
 * @param[in]  dev            : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_user_gain_upd_status(uint8_t *status, struct bmi2_dev *dev);

/*!
 * @brief This internal API enables/disables compensation of the gain defined
 * in the GAIN register.
 *
 * @param[in] enable    : Enables/Disables gain compensation
 * @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  enable      |  Description
 * -------------|---------------
 * BMI2_ENABLE  | Enable gain compensation.
 * BMI2_DISABLE | Disable gain compensation.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t enable_gyro_gain(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to extract the output feature configuration
 * details like page and start address from the look-up table.
 *
 * @param[out] feat_output      : Structure that stores output feature
 *                              configurations.
 * @param[in] type              : Type of feature or sensor.
 * @param[in] dev               : Structure instance of bmi2_dev.
 *
 * @return Returns the feature found flag.
 *
 * @retval  BMI2_FALSE : Feature not found
 *          BMI2_TRUE  : Feature found
 */
static uint8_t extract_output_feat_config(struct bmi2_feature_config *feat_output,
                                          uint8_t type,
                                          const struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable step activity detection.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables step activity.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables step activity
 * BMI2_ENABLE  | Enables step activity
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_activity(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets step counter parameter configurations.
 *
 * @param[in] step_count_params : Array that stores parameters 1 to 25.
 * @param[in] dev               : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_count_params_config(const uint16_t *step_count_params, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets step counter/detector/activity configurations.
 *
 * @param[in] config      : Structure instance of bmi2_step_config.
 * @param[in] dev         : Structure instance of bmi2_dev.
 *
 *---------------------------------------------------------------------------
 *    bmi2_step_config      |
 *  Structure parameters    |                 Description
 *--------------------------|--------------------------------------------------
 *                          | The Step-counter will trigger output every time
 *                          | the number of steps are counted. Holds implicitly
 *  water-mark level        | a 20x factor, so the range is 0 to 20460,
 *                          | with resolution of 20 steps.
 * -------------------------|---------------------------------------------------
 *  reset counter           | Flag to reset the counted steps.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_config(const struct bmi2_step_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets step counter parameter configurations.
 *
 * @param[in] step_count_params : Array that stores parameters 1 to 25.
 * @param[in] dev               : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_step_count_params_config(uint16_t *step_count_params, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets step counter/detector/activity configurations.
 *
 * @param[out] config      : Structure instance of bmi2_step_config.
 * @param[in]  dev         : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *    bmi2_step_config      |
 *  Structure parameters    |                 Description
 *--------------------------|--------------------------------------------------
 *                          | The Step-counter will trigger output every time
 *                          | the number of steps are counted. Holds implicitly
 *  water-mark level        | a 20x factor, so the range is 0 to 20460,
 *                          | with resolution of 20 steps.
 * -------------------------|---------------------------------------------------
 *  reset counter           | Flag to reset the counted steps.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_step_config(struct bmi2_step_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gives an option to enable offset correction
 * feature of gyroscope, either internally or by the host.
 *
 * @param[in] enable    : Enables/Disables self-offset correction.
 * @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  enable      |  Description
 * -------------|---------------
 * BMI2_ENABLE  | gyroscope offset correction values are set internally
 * BMI2_DISABLE | gyroscope offset correction values has to be set by host
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_gyro_self_offset_corr(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets feature configuration to the sensor.
 *
 * @param[in]       sens_cfg     : Structure instance of bmi2_sens_config.
 * @param[in]       loop         : Variable to loop the sensor feature.
 * @param[in, out]  dev          : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets feature configuration from the sensor.
 *
 * @param[in]       sens_cfg     : Structure instance of bmi2_sens_config.
 * @param[in]       loop         : Variable to loop the sensor feature.
 * @param[in, out]  dev          : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable main sensors like accel, gyro, aux and temperature.
 *
 * @param[in] sensor_sel    : Enables the selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t enable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable sensor features.
 *
 * @param[in] sensor_sel    : Enables features of selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t enable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to disable main sensors like accel, gyro, aux and temperature.
 *
 * @param[in] sensor_sel    : Disables the selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t disable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to disable sensor features.
 *
 * @param[in] sensor_sel    : Disables features of selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t disable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev);

/***************************************************************************/

/*!         User Interface Definitions
 ****************************************************************************/

/*!
 *  @brief This API:
 *  1) Updates the device structure with address of the configuration file.
 *  2) Initializes bmi270_h sensor.
 *  3) Writes the configuration file.
 *  4) Updates the feature offset parameters in the device structure.
 *  5) Updates the maximum number of pages, in the device structure.
 */
int8_t bmi270_h_init(struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if (rslt == BMI2_OK)
    {
        /* Assign chip id of BMI270_H */
        dev->chip_id = BMI270_H_CHIP_ID;

        /* Get the size of config array */
        dev->config_size = sizeof(bmi270_h_config_file);

        /* Enable the variant specific features if any */
        dev->variant_feature = BMI2_CRT_RTOSK_ENABLE | BMI2_GYRO_CROSS_SENS_ENABLE;

        /* An extra dummy byte is read during SPI read */
        if (dev->intf == BMI2_SPI_INTF)
        {
            dev->dummy_byte = 1;
        }
        else
        {
            dev->dummy_byte = 0;
        }

        /* If configuration file pointer is not assigned any address */
        if (!dev->config_file_ptr)
        {
            /* Give the address of the configuration file array to
             * the device pointer
             */
            dev->config_file_ptr = bmi270_h_config_file;
        }

        /* Initialize BMI2 sensor */
        rslt = bmi2_sec_init(dev);
        if (rslt == BMI2_OK)
        {
            /* Assign the offsets of the feature input
             * configuration to the device structure
             */
            dev->feat_config = bmi270_h_feat_in;

            /* Assign the offsets of the feature output to
             * the device structure
             */
            dev->feat_output = bmi270_h_feat_out;

            /* Assign the maximum number of pages to the
             * device structure
             */
            dev->page_max = BMI270_H_MAX_PAGE_NUM;

            /* Assign maximum number of input sensors/
             * features to device structure
             */
            dev->input_sens = BMI270_H_MAX_FEAT_IN;

            /* Assign maximum number of output sensors
             * features to device structure
             */
            dev->out_sens = BMI270_H_MAX_FEAT_OUT;

            /* Assign the offsets of the feature interrupt
             * to the device structure
             */
            dev->map_int = bmi270_h_map_int;

            /* Assign maximum number of feature interrupts
             * to device structure
             */
            dev->sens_int_map = BMI270_H_MAX_INT_MAP;

            /* Assign the tap get configurations to function pointer */
            dev->get_tap_config = get_tap_config;

            /* Assign the tap set configurations to function pointer */
            dev->set_tap_config = set_tap_config;
        }
    }

    return rslt;
}

/*!
 * @brief This API selects the sensors/features to be enabled.
 */
int8_t bmi270_h_sensor_enable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to select sensor */
    uint64_t sensor_sel = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_list != NULL))
    {
        /* Get the selected sensors */
        rslt = select_sensor(sens_list, n_sens, &sensor_sel);
        if (rslt == BMI2_OK)
        {
            /* Enable the selected sensors */
            rslt = sensor_enable(sensor_sel, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API selects the sensors/features to be disabled.
 */
int8_t bmi270_h_sensor_disable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to select sensor */
    uint64_t sensor_sel = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_list != NULL))
    {
        /* Get the selected sensors */
        rslt = select_sensor(sens_list, n_sens, &sensor_sel);
        if (rslt == BMI2_OK)
        {
            /* Disable the selected sensors */
            rslt = sensor_disable(sensor_sel, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API sets the sensor/feature configuration.
 */
int8_t bmi270_h_set_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_cfg != NULL))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;

        for (loop = 0; loop < n_sens; loop++)
        {
            if ((sens_cfg[loop].type == BMI2_ACCEL) || (sens_cfg[loop].type == BMI2_GYRO) ||
                (sens_cfg[loop].type == BMI2_AUX) || (sens_cfg[loop].type == BMI2_GYRO_GAIN_UPDATE))
            {
                rslt = bmi2_set_sensor_config(&sens_cfg[loop], 1, dev);
            }
            else
            {
                /* Disable Advance power save if enabled for auxiliary
                 * and feature configurations
                 */
                if (aps_stat == BMI2_ENABLE)
                {
                    /* Disable advance power save if
                     * enabled
                     */
                    rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
                }

                if (rslt == BMI2_OK)
                {
                    rslt = set_feat_config(sens_cfg, loop, dev);
                }
                /* Return error if any of the set configurations fail */
                else
                {
                    break;
                }
            }
        }

        /* Enable Advance power save if disabled while configuring and
         * not when already disabled
         */
        if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
        {
            rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API gets the sensor/feature configuration.
 */
int8_t bmi270_h_get_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_cfg != NULL))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        for (loop = 0; loop < n_sens; loop++)
        {
            if ((sens_cfg[loop].type == BMI2_ACCEL) || (sens_cfg[loop].type == BMI2_GYRO) ||
                (sens_cfg[loop].type == BMI2_AUX) || (sens_cfg[loop].type == BMI2_GYRO_GAIN_UPDATE))
            {
                rslt = bmi2_get_sensor_config(&sens_cfg[loop], 1, dev);
            }
            else
            {
                /* Disable Advance power save if enabled for auxiliary
                 * and feature configurations
                 */
                if ((sens_cfg[loop].type >= BMI2_MAIN_SENS_MAX_NUM) || (sens_cfg[loop].type == BMI2_AUX))
                {
                    if (aps_stat == BMI2_ENABLE)
                    {
                        /* Disable advance power save if
                         * enabled
                         */
                        rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
                    }
                }

                if (rslt == BMI2_OK)
                {
                    rslt = get_feat_config(sens_cfg, loop, dev);
                }
                /* Return error if any of the get configurations fail */
                else
                {
                    break;
                }
            }
        }

        /* Enable Advance power save if disabled while configuring and
         * not when already disabled
         */
        if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
        {
            rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API gets the feature data
 */
int8_t bmi270_h_get_feature_data(struct bmi2_feat_sensor_data *feature_data, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (feature_data != NULL))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        for (loop = 0; loop < n_sens; loop++)
        {
            if ((feature_data[loop].type == BMI2_GYRO_GAIN_UPDATE) ||
                (feature_data[loop].type == BMI2_GYRO_CROSS_SENSE))
            {
                rslt = bmi2_get_feature_data(&feature_data[loop], 1, dev);
            }
            else
            {
                /* Disable Advance power save if enabled for feature
                 * configurations
                 */
                if (feature_data[loop].type >= BMI2_MAIN_SENS_MAX_NUM)
                {
                    if (aps_stat == BMI2_ENABLE)
                    {
                        /* Disable advance power save if
                         * enabled
                         */
                        rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
                    }
                }

                if (rslt == BMI2_OK)
                {
                    switch (feature_data[loop].type)
                    {
                        case BMI2_STEP_COUNTER:

                            /* Get step counter output */
                            rslt = get_step_counter_output(&feature_data[loop].sens_data.step_counter_output, dev);

                            break;
                        case BMI2_STEP_ACTIVITY:

                            /* Get step activity output */
                            rslt = get_step_activity_output(&feature_data[loop].sens_data.activity_output, dev);

                            break;
                        case BMI2_NVM_STATUS:

                            /* Get NVM error status  */
                            rslt = get_nvm_error_status(&feature_data[loop].sens_data.nvm_status, dev);

                            break;
                        case BMI2_VFRM_STATUS:

                            /* Get VFRM error status  */
                            rslt = get_vfrm_error_status(&feature_data[loop].sens_data.vfrm_status, dev);

                            break;
                        default:
                            rslt = BMI2_E_INVALID_SENSOR;

                            break;
                    }
                }

                /* Return error if any of the get sensor data fails */
                if (rslt != BMI2_OK)
                {
                    break;
                }
            }

            /* Enable Advance power save if disabled while
             * configuring and not when already disabled
             */
            if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
            {
                rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
            }
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API updates the gyroscope user-gain.
 */
int8_t bmi270_h_update_gyro_user_gain(const struct bmi2_gyro_user_gain_config *user_gain, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to select sensor */
    uint8_t sens_sel[2] = { BMI2_GYRO, BMI2_GYRO_GAIN_UPDATE };

    /* Structure to define sensor configurations */
    struct bmi2_sens_config sens_cfg;

    /* Variable to store status of user-gain update module */
    uint8_t status = 0;

    /* Variable to define count */
    uint8_t count = 100;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (user_gain != NULL))
    {
        /* Select type of feature */
        sens_cfg.type = BMI2_GYRO_GAIN_UPDATE;

        /* Get the user gain configurations */
        rslt = bmi270_h_get_sensor_config(&sens_cfg, 1, dev);
        if (rslt == BMI2_OK)
        {
            /* Get the user-defined ratio */
            sens_cfg.cfg.gyro_gain_update = *user_gain;

            /* Set rate ratio for all axes */
            rslt = bmi270_h_set_sensor_config(&sens_cfg, 1, dev);
        }

        /* Disable gyroscope */
        if (rslt == BMI2_OK)
        {
            rslt = bmi270_h_sensor_disable(&sens_sel[0], 1, dev);
        }

        /* Enable gyroscope user-gain update module */
        if (rslt == BMI2_OK)
        {
            rslt = bmi270_h_sensor_enable(&sens_sel[1], 1, dev);
        }

        /* Set the command to trigger the computation */
        if (rslt == BMI2_OK)
        {
            rslt = bmi2_set_command_register(BMI2_USR_GAIN_CMD, dev);
        }

        if (rslt == BMI2_OK)
        {
            /* Poll until enable bit of user-gain update is 0 */
            while (count--)
            {
                rslt = get_user_gain_upd_status(&status, dev);
                if ((rslt == BMI2_OK) && (status == 0))
                {
                    /* Enable compensation of gain defined
                     * in the GAIN register
                     */
                    rslt = enable_gyro_gain(BMI2_ENABLE, dev);

                    /* Enable gyroscope */
                    if (rslt == BMI2_OK)
                    {
                        rslt = bmi270_h_sensor_enable(&sens_sel[0], 1, dev);
                    }

                    break;
                }

                dev->delay_us(10000, dev->intf_ptr);
            }

            /* Return error if user-gain update is failed */
            if ((rslt == BMI2_OK) && (status != 0))
            {
                rslt = BMI2_E_GYR_USER_GAIN_UPD_FAIL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API reads the compensated gyroscope user-gain values.
 */
int8_t bmi270_h_read_gyro_user_gain(struct bmi2_gyro_user_gain_data *gyr_usr_gain, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define register data */
    uint8_t reg_data[3] = { 0 };

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (gyr_usr_gain != NULL))
    {
        /* Get the gyroscope compensated gain values */
        rslt = bmi2_get_regs(BMI2_GYR_USR_GAIN_0_ADDR, reg_data, 3, dev);
        if (rslt == BMI2_OK)
        {
            /* Gyroscope user gain correction X-axis */
            gyr_usr_gain->x = (int8_t)BMI2_GET_BIT_POS0(reg_data[0], BMI2_GYR_USR_GAIN_X);

            /* Gyroscope user gain correction Y-axis */
            gyr_usr_gain->y = (int8_t)BMI2_GET_BIT_POS0(reg_data[1], BMI2_GYR_USR_GAIN_Y);

            /* Gyroscope user gain correction z-axis */
            gyr_usr_gain->z = (int8_t)BMI2_GET_BIT_POS0(reg_data[2], BMI2_GYR_USR_GAIN_Z);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API maps/unmaps feature interrupts to that of interrupt pins.
 */
int8_t bmi270_h_map_feat_int(const struct bmi2_sens_int_config *sens_int, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_int != NULL))
    {
        for (loop = 0; loop < n_sens; loop++)
        {
            switch (sens_int[loop].type)
            {
                case BMI2_TAP:
                case BMI2_SINGLE_TAP:
                case BMI2_DOUBLE_TAP:
                case BMI2_TRIPLE_TAP:
                case BMI2_ANY_MOTION:
                case BMI2_NO_MOTION:
                case BMI2_STEP_COUNTER:
                case BMI2_STEP_DETECTOR:
                case BMI2_STEP_ACTIVITY:

                    rslt = bmi2_map_feat_int(sens_int[loop].type, sens_int[loop].hw_int_pin, dev);

                    break;
                default:
                    rslt = BMI2_E_INVALID_SENSOR;

                    break;
            }

            /* Return error if interrupt mapping fails */
            if (rslt != BMI2_OK)
            {
                break;
            }
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/***************************************************************************/

/*!         Local Function Definitions
 ****************************************************************************/

/*!
 * @brief This internal API is used to get the tap configurations.
 */
static int8_t get_tap_config(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = get_config_page_primary(tap, bmi2_dev);

    if (rslt == BMI2_OK)
    {
        rslt = get_config_page_secondary(tap, bmi2_dev);
    }

    return rslt;
}

/*!
 * @brief This internal API is used to get the tap configurations
 * from primary page.
 */
static int8_t get_config_page_primary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint8_t lsb = 0;

    /* Variable to define MSB */
    uint8_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    bmi270_h_config = (struct bmi2_tap_config *)tap;

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_1, dev);

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap */
            idx = tap_config.start_addr;

            /* Get word to calculate single tap, double tap, triple tap, data register
             * enable and output configuration
             */
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get filtered tap data */
            bmi270_h_config->data_reg_en = (lsb_msb & BMI2_TAP_DATA_REG_EN_MASK) >> BMI2_TAP_DATA_REG_EN_POS;

            /* Get word to calculate tap_sens_thres */
            idx = 4;
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get scaling factor of threshold */
            bmi270_h_config->tap_sens_thres = lsb_msb;

            /* Get word to calculate max_gest_dur */
            idx = 6;
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get maximum duration between each taps */
            bmi270_h_config->max_gest_dur = lsb_msb;

            /* Get word to calculate quite_time_after_gest */
            idx = 14;
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get minimum quite time between the two gesture detection */
            bmi270_h_config->quite_time_after_gest = lsb_msb;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to get the tap configurations
 * from secondary page.
 */
static int8_t get_config_page_secondary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint8_t lsb = 0;

    /* Variable to define MSB */
    uint8_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    bmi270_h_config = (struct bmi2_tap_config *)tap;

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_2, dev);

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap */
            idx = tap_config.start_addr;

            /* Get word to calculate wait_for_timeout */
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get number of taps detected based on wait time */
            bmi270_h_config->wait_for_timeout = lsb_msb;

            idx = 4;

            /* Get word to calculate axis_sel */
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Select axis for tap detection */
            bmi270_h_config->axis_sel = (lsb_msb & BMI2_TAP_AXIS_SEL_MASK);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to set the tap configurations.
 */
static int8_t set_tap_config(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = set_config_page_primary(tap, bmi2_dev);

    if (rslt == BMI2_OK)
    {
        rslt = set_config_page_secondary(tap, bmi2_dev);
    }

    return rslt;
}

/*!
 * @brief This internal API is used to set the tap configurations
 * to primary page.
 */
static int8_t set_config_page_primary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    bmi270_h_config = (struct bmi2_tap_config*)tap;

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_1, dev);

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap select */
            idx = tap_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set filtered tap data */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_TAP_DATA_REG_EN, bmi270_h_config->data_reg_en);

            /* Starting address of tap_sens_thres represented in word length */
            idx = 2;

            /* Set scaling factor of threshold */
            *(data_p + idx) = bmi270_h_config->tap_sens_thres;

            /* Starting address of max_gest_dur represented in word length */
            idx = 3;

            /* Set maximum duration between each taps */
            *(data_p + idx) = bmi270_h_config->max_gest_dur;

            /* Starting address of quite_time_after_gest represented in word length */
            idx = 7;

            /* Set minimum quite time between the two gesture detection */
            *(data_p + idx) = bmi270_h_config->quite_time_after_gest;

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - tap_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index <= idx; index++)
            {
                feat_config[tap_config.start_addr + index] = *((uint8_t *) data_p + tap_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to set the tap configurations
 * to secondary page.
 */
static int8_t set_config_page_secondary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    bmi270_h_config = (struct bmi2_tap_config*)tap;

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_2, dev);

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap */
            idx = tap_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set wait time */
            *(data_p + idx) = bmi270_h_config->wait_for_timeout;

            /* Starting address of axis_sel represented in word length */
            idx = 2;

            /* Select axis for tap detection */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_TAP_AXIS_SEL, bmi270_h_config->axis_sel);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - tap_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[tap_config.start_addr + index] = *((uint8_t *) data_p + tap_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to validate the device structure pointer for
 * null conditions.
 */
static int8_t null_ptr_check(const struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_us == NULL))
    {
        /* Device structure pointer is not valid */
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This internal API selects the sensor/features to be enabled or
 * disabled.
 */
static int8_t select_sensor(const uint8_t *sens_list, uint8_t n_sens, uint64_t *sensor_sel)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Variable to define loop */
    uint8_t count;

    for (count = 0; count < n_sens; count++)
    {
        switch (sens_list[count])
        {
            case BMI2_ACCEL:
                *sensor_sel |= BMI2_ACCEL_SENS_SEL;

                break;
            case BMI2_GYRO:
                *sensor_sel |= BMI2_GYRO_SENS_SEL;

                break;
            case BMI2_AUX:
                *sensor_sel |= BMI2_AUX_SENS_SEL;

                break;
            case BMI2_ANY_MOTION:
                *sensor_sel |= BMI2_ANY_MOT_SEL;

                break;
            case BMI2_NO_MOTION:
                *sensor_sel |= BMI2_NO_MOT_SEL;

                break;
            case BMI2_STEP_DETECTOR:
                *sensor_sel |= BMI2_STEP_DETECT_SEL;

                break;
            case BMI2_STEP_COUNTER:
                *sensor_sel |= BMI2_STEP_COUNT_SEL;

                break;
            case BMI2_STEP_ACTIVITY:
                *sensor_sel |= BMI2_STEP_ACT_SEL;

                break;
            case BMI2_GYRO_GAIN_UPDATE:
                *sensor_sel |= BMI2_GYRO_GAIN_UPDATE_SEL;

                break;
            case BMI2_SINGLE_TAP:
                *sensor_sel |= BMI2_SINGLE_TAP_SEL;

                break;
            case BMI2_DOUBLE_TAP:
                *sensor_sel |= BMI2_DOUBLE_TAP_SEL;

                break;
            case BMI2_TRIPLE_TAP:
                *sensor_sel |= BMI2_TRIPLE_TAP_SEL;

                break;
            case BMI2_GYRO_SELF_OFF:
                *sensor_sel |= BMI2_GYRO_SELF_OFF_SEL;

                break;
            default:
                rslt = BMI2_E_INVALID_SENSOR;

                break;
        }
    }

    return rslt;
}

/*!
 * @brief This internal API enables the selected sensor/features.
 */
static int8_t sensor_enable(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    rslt = enable_main_sensors(sensor_sel, dev);

    if ((rslt == BMI2_OK) && (sensor_sel & ~(BMI2_MAIN_SENSORS)))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        if (aps_stat == BMI2_ENABLE)
        {
            /* Disable advance power save if enabled */
            rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
        }

        if (rslt == BMI2_OK)
        {
            rslt = enable_sensor_features(sensor_sel, dev);

            /* Enable Advance power save if disabled while
             * configuring and not when already disabled
             */
            if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
            {
                rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable main sensors like accel, gyro, aux and temperature.
 */
static int8_t enable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to store register values */
    uint8_t reg_data;

    rslt = bmi2_get_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);

    if (rslt == BMI2_OK)
    {
        /* Enable accelerometer */
        if (sensor_sel & BMI2_ACCEL_SENS_SEL)
        {
            reg_data = BMI2_SET_BITS(reg_data, BMI2_ACC_EN, BMI2_ENABLE);
        }

        /* Enable gyroscope */
        if (sensor_sel & BMI2_GYRO_SENS_SEL)
        {
            reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_EN, BMI2_ENABLE);
        }

        /* Enable auxiliary sensor */
        if (sensor_sel & BMI2_AUX_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_AUX_EN, BMI2_ENABLE);
        }

        /* Enable temperature sensor */
        if (sensor_sel & BMI2_TEMP_SENS_SEL)
        {
            reg_data = BMI2_SET_BITS(reg_data, BMI2_TEMP_EN, BMI2_ENABLE);
        }

        /* Enable the sensors that are set in the power control register */
        if (sensor_sel & BMI2_MAIN_SENSORS)
        {
            rslt = bmi2_set_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable sensor features.
 */
static int8_t enable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Enable any motion feature */
    if (sensor_sel & BMI2_ANY_MOT_SEL)
    {
        rslt = set_any_motion(BMI2_ENABLE, dev);
    }

    /* Enable no motion feature */
    if (sensor_sel & BMI2_NO_MOT_SEL)
    {
        rslt = set_no_motion(BMI2_ENABLE, dev);
    }

    /* Enable step detector feature */
    if (sensor_sel & BMI2_STEP_DETECT_SEL)
    {
        rslt = set_step_detector(BMI2_ENABLE, dev);
    }

    /* Enable step counter feature */
    if (sensor_sel & BMI2_STEP_COUNT_SEL)
    {
        rslt = set_step_counter(BMI2_ENABLE, dev);
    }

    /* Enable step activity feature */
    if (sensor_sel & BMI2_STEP_ACT_SEL)
    {
        rslt = set_step_activity(BMI2_ENABLE, dev);
    }

    /* Enable gyroscope user gain */
    if (sensor_sel & BMI2_GYRO_GAIN_UPDATE_SEL)
    {
        rslt = set_gyro_user_gain(BMI2_ENABLE, dev);
    }

    /* Enable single tap feature */
    if (sensor_sel & BMI2_SINGLE_TAP_SEL)
    {
        rslt = set_single_tap(BMI2_ENABLE, dev);
    }

    /* Enable double tap feature */
    if (sensor_sel & BMI2_DOUBLE_TAP_SEL)
    {
        rslt = set_double_tap(BMI2_ENABLE, dev);
    }

    /* Enable triple tap feature */
    if (sensor_sel & BMI2_TRIPLE_TAP_SEL)
    {
        rslt = set_triple_tap(BMI2_ENABLE, dev);
    }

    /* Enable gyroscope self-offset correction feature */
    if (sensor_sel & BMI2_GYRO_SELF_OFF_SEL)
    {
        rslt = set_gyro_self_offset_corr(BMI2_ENABLE, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API disables the selected sensors/features.
 */
static int8_t sensor_disable(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    rslt = disable_main_sensors(sensor_sel, dev);

    if ((rslt == BMI2_OK) && (sensor_sel & ~(BMI2_MAIN_SENSORS)))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        if (aps_stat == BMI2_ENABLE)
        {
            /* Disable advance power save if enabled */
            rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
        }

        if (rslt == BMI2_OK)
        {
            rslt = disable_sensor_features(sensor_sel, dev);

            /* Enable Advance power save if disabled while
             * configuring and not when already disabled
             */
            if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
            {
                rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable any motion feature.
 */
static int8_t set_any_motion(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for any-motion */
    struct bmi2_feature_config any_mot_config = { 0, 0, 0 };

    /* Search for any-motion feature and extract its configurations details */
    feat_found = bmi2_extract_input_feat_config(&any_mot_config, BMI2_ANY_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any-motion feature resides */
        rslt = bmi2_get_feat_config(any_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of any-motion axes */
            idx = any_mot_config.start_addr + BMI2_ANY_MOT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_ANY_NO_MOT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_ANY_MOT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_ANY_MOT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable no-motion feature.
 */
static int8_t set_no_motion(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for no-motion */
    struct bmi2_feature_config no_mot_config = { 0, 0, 0 };

    /* Search for no-motion feature and extract its configurations details */
    feat_found = bmi2_extract_input_feat_config(&no_mot_config, BMI2_NO_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any/no-motion feature resides */
        rslt = bmi2_get_feat_config(no_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of no-motion axes */
            idx = no_mot_config.start_addr + BMI2_NO_MOT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_ANY_NO_MOT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_NO_MOT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_NO_MOT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable step detector feature.
 */
static int8_t set_step_detector(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step detector */
    struct bmi2_feature_config step_det_config = { 0, 0, 0 };

    /* Search for step detector feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_det_config, BMI2_STEP_DETECTOR, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step detector feature resides */
        rslt = bmi2_get_feat_config(step_det_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of step detector */
            idx = step_det_config.start_addr + BMI2_STEP_COUNT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_STEP_DET_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_STEP_DETECT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_STEP_DETECT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable step counter feature.
 */
static int8_t set_step_counter(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step counter */
    struct bmi2_feature_config step_count_config = { 0, 0, 0 };

    /* Search for step counter feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_count_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step-counter feature resides */
        rslt = bmi2_get_feat_config(step_count_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of step counter */
            idx = step_count_config.start_addr + BMI2_STEP_COUNT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_STEP_COUNT_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_STEP_COUNT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_STEP_COUNT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable step activity detection.
 */
static int8_t set_step_activity(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step activity */
    struct bmi2_feature_config step_act_config = { 0, 0, 0 };

    /* Search for step activity feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_act_config, BMI2_STEP_ACTIVITY, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step-activity
         * feature resides
         */
        rslt = bmi2_get_feat_config(step_act_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of step activity */
            idx = step_act_config.start_addr + BMI2_STEP_COUNT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_STEP_ACT_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_STEP_ACT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_STEP_ACT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * single tap.
 */
static int8_t set_single_tap(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_SINGLE_TAP, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of tap */
            idx = tap_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BIT_POS0(feat_config[idx], BMI2_TAP_SINGLE_TAP_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_SINGLE_TAP_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_SINGLE_TAP_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * double tap.
 */
static int8_t set_double_tap(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_DOUBLE_TAP, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of tap */
            idx = tap_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_TAP_DOUBLE_TAP_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_DOUBLE_TAP_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_DOUBLE_TAP_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * triple tap.
 */
static int8_t set_triple_tap(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TRIPLE_TAP, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of tap */
            idx = tap_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_TAP_TRIPLE_TAP_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_TRIPLE_TAP_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_TRIPLE_TAP_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable gyroscope user gain
 * feature.
 */
static int8_t set_gyro_user_gain(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for gyroscope user gain */
    struct bmi2_feature_config gyr_user_gain_cfg = { 0, 0, 0 };

    /* Search for user gain feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&gyr_user_gain_cfg, BMI2_GYRO_GAIN_UPDATE, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where user gain feature resides */
        rslt = bmi2_get_feat_config(gyr_user_gain_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of user gain */
            idx = gyr_user_gain_cfg.start_addr + BMI2_GYR_USER_GAIN_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_GYR_USER_GAIN_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_GYRO_GAIN_UPDATE_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_GYRO_GAIN_UPDATE_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t set_any_motion_config(const struct bmi2_any_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define count */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for any motion */
    struct bmi2_feature_config any_mot_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for any-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&any_mot_config, BMI2_ANY_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any-motion feature resides */
        rslt = bmi2_get_feat_config(any_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for any-motion select */
            idx = any_mot_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set duration */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_DUR, config->duration);

            /* Set x-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_X_SEL, config->select_x);

            /* Set y-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Y_SEL, config->select_y);

            /* Set z-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Z_SEL, config->select_z);

            /* Increment offset by 1 word to set threshold and output configuration */
            idx++;

            /* Set threshold */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_THRES, config->threshold);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - any_mot_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[any_mot_config.start_addr +
                            index] = *((uint8_t *) data_p + any_mot_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t set_no_motion_config(const struct bmi2_no_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define count */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for no-motion */
    struct bmi2_feature_config no_mot_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for no-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&no_mot_config, BMI2_NO_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where no-motion feature resides */
        rslt = bmi2_get_feat_config(no_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for no-motion select */
            idx = no_mot_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set duration */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_DUR, config->duration);

            /* Set x-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_X_SEL, config->select_x);

            /* Set y-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Y_SEL, config->select_y);

            /* Set z-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Z_SEL, config->select_z);

            /* Increment offset by 1 word to set threshold and output configuration */
            idx++;

            /* Set threshold */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_THRES, config->threshold);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - no_mot_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[no_mot_config.start_addr +
                            index] = *((uint8_t *) data_p + no_mot_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/* @brief This internal API sets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 */
static int8_t set_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = dev->set_tap_config(config, dev);

    return rslt;
}

/*!
 * @brief This internal API gets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t get_any_motion_config(struct bmi2_any_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb;

    /* Variable to define MSB */
    uint16_t msb;

    /* Variable to define a word */
    uint16_t lsb_msb;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for any-motion */
    struct bmi2_feature_config any_mot_config = { 0, 0, 0 };

    /* Search for any-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&any_mot_config, BMI2_ANY_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any-motion feature resides */
        rslt = bmi2_get_feat_config(any_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for feature enable for any-motion */
            idx = any_mot_config.start_addr;

            /* Get word to calculate duration, x, y and z select */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get duration */
            config->duration = lsb_msb & BMI2_ANY_NO_MOT_DUR_MASK;

            /* Get x-select */
            config->select_x = (lsb_msb & BMI2_ANY_NO_MOT_X_SEL_MASK) >> BMI2_ANY_NO_MOT_X_SEL_POS;

            /* Get y-select */
            config->select_y = (lsb_msb & BMI2_ANY_NO_MOT_Y_SEL_MASK) >> BMI2_ANY_NO_MOT_Y_SEL_POS;

            /* Get z-select */
            config->select_z = (lsb_msb & BMI2_ANY_NO_MOT_Z_SEL_MASK) >> BMI2_ANY_NO_MOT_Z_SEL_POS;

            /* Get word to calculate threshold, output configuration from the same word */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get threshold */
            config->threshold = lsb_msb & BMI2_ANY_NO_MOT_THRES_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t get_no_motion_config(struct bmi2_no_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for no-motion */
    struct bmi2_feature_config no_mot_config = { 0, 0, 0 };

    /* Search for no-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&no_mot_config, BMI2_NO_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where no-motion feature resides */
        rslt = bmi2_get_feat_config(no_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for feature enable for no-motion */
            idx = no_mot_config.start_addr;

            /* Get word to calculate duration, x, y and z select */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get duration */
            config->duration = lsb_msb & BMI2_ANY_NO_MOT_DUR_MASK;

            /* Get x-select */
            config->select_x = (lsb_msb & BMI2_ANY_NO_MOT_X_SEL_MASK) >> BMI2_ANY_NO_MOT_X_SEL_POS;

            /* Get y-select */
            config->select_y = (lsb_msb & BMI2_ANY_NO_MOT_Y_SEL_MASK) >> BMI2_ANY_NO_MOT_Y_SEL_POS;

            /* Get z-select */
            config->select_z = (lsb_msb & BMI2_ANY_NO_MOT_Z_SEL_MASK) >> BMI2_ANY_NO_MOT_Z_SEL_POS;

            /* Get word to calculate threshold, output configuration from the same word */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get threshold */
            config->threshold = lsb_msb & BMI2_ANY_NO_MOT_THRES_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 */
static int8_t get_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = dev->get_tap_config(config, dev);

    return rslt;
}

/*!
 * @brief This internal API gets the error status related to NVM.
 */
static int8_t get_nvm_error_status(struct bmi2_nvm_err_status *nvm_err_stat, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for NVM error status */
    struct bmi2_feature_config nvm_err_cfg = { 0, 0, 0 };

    /* Search for NVM error status feature and extract its configuration details */
    feat_found = extract_output_feat_config(&nvm_err_cfg, BMI2_NVM_STATUS, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for NVM error status */
        rslt = bmi2_get_feat_config(nvm_err_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for NVM error status */
            idx = nvm_err_cfg.start_addr;

            /* Increment index to get the error status */
            idx++;

            /* Error when NVM load action fails */
            nvm_err_stat->load_error = BMI2_GET_BIT_POS0(feat_config[idx], BMI2_NVM_LOAD_ERR_STATUS);

            /* Error when NVM program action fails */
            nvm_err_stat->prog_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_PROG_ERR_STATUS);

            /* Error when NVM erase action fails */
            nvm_err_stat->erase_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_ERASE_ERR_STATUS);

            /* Error when NVM program limit is exceeded */
            nvm_err_stat->exceed_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_END_EXCEED_STATUS);

            /* Error when NVM privilege mode is not acquired */
            nvm_err_stat->privil_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_PRIV_ERR_STATUS);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to get enable status of gyroscope user gain
 * update.
 */
static int8_t get_user_gain_upd_status(uint8_t *status, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Variable to check APS status */
    uint8_t aps_stat = 0;

    /* Initialize feature configuration for gyroscope user gain */
    struct bmi2_feature_config gyr_user_gain_cfg = { 0, 0, 0 };

    /* Search for user gain feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&gyr_user_gain_cfg, BMI2_GYRO_GAIN_UPDATE, dev);
    if (feat_found)
    {
        /* Disable advance power save */
        aps_stat = dev->aps_status;
        if (aps_stat == BMI2_ENABLE)
        {
            rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
        }

        if (rslt == BMI2_OK)
        {
            /* Get the configuration from the page where user gain feature resides */
            rslt = bmi2_get_feat_config(gyr_user_gain_cfg.page, feat_config, dev);
            if (rslt == BMI2_OK)
            {
                /* Define the offset for enable/disable of user gain */
                idx = gyr_user_gain_cfg.start_addr + BMI2_GYR_USER_GAIN_FEAT_EN_OFFSET;

                /* Set the feature enable status */
                *status = BMI2_GET_BITS(feat_config[idx], BMI2_GYR_USER_GAIN_FEAT_EN);
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    /* Enable Advance power save if disabled while configuring and not when already disabled */
    if ((rslt == BMI2_OK) && (aps_stat == BMI2_ENABLE))
    {
        rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API gets the output values of step counter.
 */
static int8_t get_step_counter_output(uint32_t *step_count, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for step counter */
    struct bmi2_feature_config step_cnt_out_config = { 0, 0, 0 };

    /* Search for step counter output feature and extract its configuration details */
    feat_found = extract_output_feat_config(&step_cnt_out_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for step-counter */
        rslt = bmi2_get_feat_config(step_cnt_out_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for step counter output */
            idx = step_cnt_out_config.start_addr;

            /* Get the step counter output in 4 bytes */
            *step_count = (uint32_t) feat_config[idx++];
            *step_count |= ((uint32_t) feat_config[idx++] << 8);
            *step_count |= ((uint32_t) feat_config[idx++] << 16);
            *step_count |= ((uint32_t) feat_config[idx++] << 24);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets the output values of step activity.
 */
static int8_t get_step_activity_output(uint8_t *step_act, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for step activity */
    struct bmi2_feature_config step_act_out_config = { 0, 0, 0 };

    /* Search for step activity output feature and extract its configuration details */
    feat_found = extract_output_feat_config(&step_act_out_config, BMI2_STEP_ACTIVITY, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for step-activity */
        rslt = bmi2_get_feat_config(step_act_out_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for step activity output */
            idx = step_act_out_config.start_addr;

            /* Get the step activity output */
            *step_act = feat_config[idx];
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API enables/disables compensation of the gain defined
 * in the GAIN register.
 */
static int8_t enable_gyro_gain(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define register data */
    uint8_t reg_data = 0;

    rslt = bmi2_get_regs(BMI2_GYR_OFF_COMP_6_ADDR, &reg_data, 1, dev);
    if (rslt == BMI2_OK)
    {
        reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_GAIN_EN, enable);
        rslt = bmi2_set_regs(BMI2_GYR_OFF_COMP_6_ADDR, &reg_data, 1, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API is used to extract the output feature configuration
 * details from the look-up table.
 */
static uint8_t extract_output_feat_config(struct bmi2_feature_config *feat_output,
                                          uint8_t type,
                                          const struct bmi2_dev *dev)
{
    /* Variable to define loop */
    uint8_t loop = 0;

    /* Variable to set flag */
    uint8_t feat_found = BMI2_FALSE;

    /* Search for the output feature from the output configuration array */
    while (loop < dev->out_sens)
    {
        if (dev->feat_output[loop].type == type)
        {
            *feat_output = dev->feat_output[loop];
            feat_found = BMI2_TRUE;
            break;
        }

        loop++;
    }

    /* Return flag */
    return feat_found;
}

/*!
 * @brief This internal API gets the error status related to virtual frames.
 */
static int8_t get_vfrm_error_status(struct bmi2_vfrm_err_status *vfrm_err_stat, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for VFRM error status */
    struct bmi2_feature_config vfrm_err_cfg = { 0, 0, 0 };

    /* Search for VFRM error status feature and extract its configuration details */
    feat_found = extract_output_feat_config(&vfrm_err_cfg, BMI2_VFRM_STATUS, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for VFRM error status */
        rslt = bmi2_get_feat_config(vfrm_err_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for VFRM error status */
            idx = vfrm_err_cfg.start_addr;

            /* Increment index to get the error status */
            idx++;

            /* Internal error while acquiring lock for FIFO */
            vfrm_err_stat->lock_error = BMI2_GET_BITS(feat_config[idx], BMI2_VFRM_LOCK_ERR_STATUS);

            /* Internal error while writing byte into FIFO */
            vfrm_err_stat->write_error = BMI2_GET_BITS(feat_config[idx], BMI2_VFRM_WRITE_ERR_STATUS);

            /* Internal error while writing into FIFO */
            vfrm_err_stat->fatal_error = BMI2_GET_BITS(feat_config[idx], BMI2_VFRM_FATAL_ERR_STATUS);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets step counter parameter configurations.
 */
static int8_t set_step_count_params_config(const uint16_t *step_count_params, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step counter parameters */
    struct bmi2_feature_config step_params_config = { 0, 0, 0 };

    /* Variable to index the page number */
    uint8_t page_idx;

    /* Variable to define the start page */
    uint8_t start_page;

    /* Variable to define start address of the parameters */
    uint8_t start_addr;

    /* Variable to define number of bytes */
    uint8_t n_bytes = (BMI2_STEP_CNT_N_PARAMS * 2);

    /* Variable to store number of pages */
    uint8_t n_pages = (n_bytes / 16);

    /* Variable to define the end page */
    uint8_t end_page;

    /* Variable to define the remaining bytes to be read */
    uint8_t remain_len;

    /* Variable to define the maximum words(16 bytes or 8 words) to be read in a page */
    uint8_t max_len = 8;

    /* Variable index bytes in a page */
    uint8_t page_byte_idx;

    /* Variable to index the parameters */
    uint8_t param_idx = 0;

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for step counter parameter feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_params_config, BMI2_STEP_COUNTER_PARAMS, dev);
    if (feat_found)
    {
        /* Get the start page for the step counter parameters */
        start_page = step_params_config.page;

        /* Get the end page for the step counter parameters */
        end_page = start_page + n_pages;

        /* Get the start address for the step counter parameters */
        start_addr = step_params_config.start_addr;

        /* Get the remaining length of bytes to be read */
        remain_len = (uint8_t)((n_bytes - (n_pages * 16)) + start_addr);
        for (page_idx = start_page; page_idx <= end_page; page_idx++)
        {
            /* Get the configuration from the respective page */
            rslt = bmi2_get_feat_config(page_idx, feat_config, dev);
            if (rslt == BMI2_OK)
            {
                /* Start from address 0x00 when switched to next page */
                if (page_idx > start_page)
                {
                    start_addr = 0;
                }

                /* Remaining number of words to be read in the page  */
                if (page_idx == end_page)
                {
                    max_len = (remain_len / 2);
                }

                /* Get offset in words since all the features are set in words length */
                page_byte_idx = start_addr / 2;
                for (; page_byte_idx < max_len;)
                {
                    /* Set parameters 1 to 25 */
                    *(data_p + page_byte_idx) = BMI2_SET_BIT_POS0(*(data_p + page_byte_idx),
                                                                  BMI2_STEP_COUNT_PARAMS,
                                                                  step_count_params[param_idx]);

                    /* Increment offset by 1 word to set to the next parameter */
                    page_byte_idx++;

                    /* Increment to next parameter */
                    param_idx++;
                }

                /* Get total length in bytes to copy from local pointer to the array */
                page_byte_idx = (uint8_t)(page_byte_idx * 2) - step_params_config.start_addr;

                /* Copy the bytes to be set back to the array */
                for (index = 0; index < page_byte_idx; index++)
                {
                    feat_config[step_params_config.start_addr +
                                index] = *((uint8_t *) data_p + step_params_config.start_addr + index);
                }

                /* Set the configuration back to the page */
                rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets step counter parameter configurations.
 */
static int8_t get_step_count_params_config(uint16_t *step_count_params, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to set flag */
    uint8_t feat_found;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Initialize feature configuration for step counter 1 */
    struct bmi2_feature_config step_params_config = { 0, 0, 0 };

    /* Variable to index the page number */
    uint8_t page_idx;

    /* Variable to define the start page */
    uint8_t start_page;

    /* Variable to define start address of the parameters */
    uint8_t start_addr;

    /* Variable to define number of bytes */
    uint8_t n_bytes = (BMI2_STEP_CNT_N_PARAMS * 2);

    /* Variable to store number of pages */
    uint8_t n_pages = (n_bytes / 16);

    /* Variable to define the end page */
    uint8_t end_page;

    /* Variable to define the remaining bytes to be read */
    uint8_t remain_len;

    /* Variable to define the maximum words to be read in a page */
    uint8_t max_len = BMI2_FEAT_SIZE_IN_BYTES;

    /* Variable index bytes in a page */
    uint8_t page_byte_idx;

    /* Variable to index the parameters */
    uint8_t param_idx = 0;

    /* Search for step counter parameter feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_params_config, BMI2_STEP_COUNTER_PARAMS, dev);
    if (feat_found)
    {
        /* Get the start page for the step counter parameters */
        start_page = step_params_config.page;

        /* Get the end page for the step counter parameters */
        end_page = start_page + n_pages;

        /* Get the start address for the step counter parameters */
        start_addr = step_params_config.start_addr;

        /* Get the remaining length of bytes to be read */
        remain_len = (uint8_t)((n_bytes - (n_pages * 16)) + start_addr);
        for (page_idx = start_page; page_idx <= end_page; page_idx++)
        {
            /* Get the configuration from the respective page */
            rslt = bmi2_get_feat_config(page_idx, feat_config, dev);
            if (rslt == BMI2_OK)
            {
                /* Start from address 0x00 when switched to next page */
                if (page_idx > start_page)
                {
                    start_addr = 0;
                }

                /* Remaining number of bytes to be read in the page  */
                if (page_idx == end_page)
                {
                    max_len = remain_len;
                }

                /* Get the offset */
                page_byte_idx = start_addr;
                while (page_byte_idx < max_len)
                {
                    /* Get word to calculate the parameter*/
                    lsb = (uint16_t) feat_config[page_byte_idx++];
                    if (page_byte_idx < max_len)
                    {
                        msb = ((uint16_t) feat_config[page_byte_idx++] << 8);
                    }

                    lsb_msb = lsb | msb;

                    /* Get parameters 1 to 25 */
                    step_count_params[param_idx] = lsb_msb & BMI2_STEP_COUNT_PARAMS_MASK;

                    /* Increment to next parameter */
                    param_idx++;
                }
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/* @brief This internal API sets step counter configurations like water-mark
 * level, reset-counter and output-configuration step detector and activity.
 */
static int8_t set_step_config(const struct bmi2_step_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step counter 4 */
    struct bmi2_feature_config step_count_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for step counter feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_count_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step counter resides */
        rslt = bmi2_get_feat_config(step_count_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes */
            idx = step_count_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set water-mark level */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_STEP_COUNT_WM_LEVEL, config->watermark_level);

            /* Set reset-counter */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_STEP_COUNT_RST_CNT, config->reset_counter);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - step_count_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[step_count_config.start_addr +
                            index] = *((uint8_t *) data_p + step_count_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets step counter/detector/activity configurations.
 */
static int8_t get_step_config(struct bmi2_step_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step counter */
    struct bmi2_feature_config step_count_config = { 0, 0, 0 };

    /* Search for step counter 4 feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_count_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step counter 4 parameter resides */
        rslt = bmi2_get_feat_config(step_count_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for feature enable for step counter/detector/activity */
            idx = step_count_config.start_addr;

            /* Get word to calculate water-mark level and reset counter */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get water-mark level */
            config->watermark_level = lsb_msb & BMI2_STEP_COUNT_WM_LEVEL_MASK;

            /* Get reset counter */
            config->reset_counter = (lsb_msb & BMI2_STEP_COUNT_RST_CNT_MASK) >> BMI2_STEP_COUNT_RST_CNT_POS;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gives an option to enable self-offset correction
 * feature of gyroscope, either internally or by the host.
 */
static int8_t set_gyro_self_offset_corr(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for self-offset correction */
    struct bmi2_feature_config self_off_corr_cfg = { 0, 0, 0 };

    /* Search for self-offset correction and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&self_off_corr_cfg, BMI2_GYRO_SELF_OFF, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where self-offset
         * correction feature resides
         */
        rslt = bmi2_get_feat_config(self_off_corr_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of self-offset correction */
            idx = self_off_corr_cfg.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_GYR_SELF_OFF_CORR_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets feature configuration to the sensor.
 */
static int8_t set_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    switch (sens_cfg[loop].type)
    {
        /* Set any motion configuration */
        case BMI2_ANY_MOTION:
            rslt = set_any_motion_config(&sens_cfg[loop].cfg.any_motion, dev);

            break;

        /* Set no motion configuration */
        case BMI2_NO_MOTION:
            rslt = set_no_motion_config(&sens_cfg[loop].cfg.no_motion, dev);

            break;

        /* Set the step counter parameters */
        case BMI2_STEP_COUNTER_PARAMS:
            rslt = set_step_count_params_config(sens_cfg[loop].cfg.step_counter_params, dev);

            break;

        /* Set step counter/detector/activity configuration */
        case BMI2_STEP_DETECTOR:
        case BMI2_STEP_COUNTER:
        case BMI2_STEP_ACTIVITY:
            rslt = set_step_config(&sens_cfg[loop].cfg.step_counter, dev);

            break;

        /* Set tap configuration */
        case BMI2_SINGLE_TAP:
        case BMI2_DOUBLE_TAP:
        case BMI2_TRIPLE_TAP:
        case BMI2_TAP:
            rslt = set_tap_detector_config(&sens_cfg[loop].cfg.tap_conf, dev);

            break;
        default:
            rslt = BMI2_E_INVALID_SENSOR;

            break;
    }

    return rslt;
}

/*!
 * @brief This internal API gets feature configuration from the sensor.
 */
static int8_t get_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    switch (sens_cfg[loop].type)
    {
        /* Get any motion configuration */
        case BMI2_ANY_MOTION:
            rslt = get_any_motion_config(&sens_cfg[loop].cfg.any_motion, dev);

            break;

        /* Get no motion configuration */
        case BMI2_NO_MOTION:
            rslt = get_no_motion_config(&sens_cfg[loop].cfg.no_motion, dev);

            break;

        /* Set the step counter parameters */
        case BMI2_STEP_COUNTER_PARAMS:
            rslt = get_step_count_params_config(sens_cfg[loop].cfg.step_counter_params, dev);

            break;

        /* Get step counter/detector/activity configuration */
        case BMI2_STEP_DETECTOR:
        case BMI2_STEP_COUNTER:
        case BMI2_STEP_ACTIVITY:
            rslt = get_step_config(&sens_cfg[loop].cfg.step_counter, dev);

            break;

        /* Get tap configuration */
        case BMI2_SINGLE_TAP:
        case BMI2_DOUBLE_TAP:
        case BMI2_TRIPLE_TAP:
        case BMI2_TAP:
            rslt = get_tap_detector_config(&sens_cfg[loop].cfg.tap_conf, dev);

            break;
        default:
            rslt = BMI2_E_INVALID_SENSOR;

            break;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to disable main sensors like accel, gyro, aux and temperature.
 */
static int8_t disable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to store register values */
    uint8_t reg_data;

    rslt = bmi2_get_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);

    if (rslt == BMI2_OK)
    {
        /* Disable accelerometer */
        if (sensor_sel & BMI2_ACCEL_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_ACC_EN);
        }

        /* Disable gyroscope */
        if (sensor_sel & BMI2_GYRO_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_GYR_EN);
        }

        /* Disable auxiliary sensor */
        if (sensor_sel & BMI2_AUX_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_AUX_EN);
        }

        /* Disable the sensors that are set in the power control register */
        if (sensor_sel & BMI2_MAIN_SENSORS)
        {
            rslt = bmi2_set_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to disable sensor features.
 */
static int8_t disable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Disable any-motion feature */
    if (sensor_sel & BMI2_ANY_MOT_SEL)
    {
        rslt = set_any_motion(BMI2_DISABLE, dev);
    }

    /* Disable no-motion feature */
    if (sensor_sel & BMI2_NO_MOT_SEL)
    {
        rslt = set_no_motion(BMI2_DISABLE, dev);
    }

    /* Disable step detector feature */
    if (sensor_sel & BMI2_STEP_DETECT_SEL)
    {
        rslt = set_step_detector(BMI2_DISABLE, dev);
    }

    /* Disable step counter feature */
    if (sensor_sel & BMI2_STEP_COUNT_SEL)
    {
        rslt = set_step_counter(BMI2_DISABLE, dev);
    }

    /* Disable step activity feature */
    if (sensor_sel & BMI2_STEP_ACT_SEL)
    {
        rslt = set_step_activity(BMI2_DISABLE, dev);
    }

    /* Disable gyroscope user gain */
    if (sensor_sel & BMI2_GYRO_GAIN_UPDATE_SEL)
    {
        rslt = set_gyro_user_gain(BMI2_DISABLE, dev);
    }

    /* Disable single tap feature */
    if (sensor_sel & BMI2_SINGLE_TAP_SEL)
    {
        rslt = set_single_tap(BMI2_DISABLE, dev);
    }

    /* Disable double tap feature */
    if (sensor_sel & BMI2_DOUBLE_TAP_SEL)
    {
        rslt = set_double_tap(BMI2_DISABLE, dev);
    }

    /* Disable triple tap feature */
    if (sensor_sel & BMI2_TRIPLE_TAP_SEL)
    {
        rslt = set_triple_tap(BMI2_DISABLE, dev);
    }

    /* Disable gyroscope self-offset correction feature */
    if (sensor_sel & BMI2_GYRO_SELF_OFF_SEL)
    {
        rslt = set_gyro_self_offset_corr(BMI2_DISABLE, dev);
    }

    return rslt;
}
