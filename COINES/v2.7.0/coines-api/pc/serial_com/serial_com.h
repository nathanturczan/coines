/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SERIAL_COM_H
#define SERIAL_COM_H

#ifdef __cplusplus
extern 'C' {
#endif

#include <stdint.h>
#include <stddef.h>

#define SCOM_OK               INT8_C(0)
#define SCOM_E_DEV_NOT_FOUND  INT8_C(-1)
#define SCOM_E_PORT_IN_USE    INT8_C(-2)
#define SCOM_E_PORT_NOT_OPEN  INT8_C(-3)
#define SCOM_E_CONFIG_FAILED  INT8_C(-4)
#define SCOM_E_READ_FAILED    INT8_C(-5)
#define SCOM_E_WRITE_FAILED   INT8_C(-6)

#define SCOM_OUT_BUFFER_SIZE  UINT32_C(2060)
#define SCOM_IN_BUFFER_SIZE   UINT32_C(2060)

int8_t scom_open_id(uint32_t baud_rate, uint16_t vendor_id, uint16_t product_id);

int8_t scom_open(void);

int8_t scom_read(void *buffer, uint32_t n_bytes, uint32_t *n_bytes_read);

int8_t scom_write(void *buffer, uint32_t n_bytes);

int8_t scom_close(void);

#ifdef __cplusplus
}
#endif

#endif