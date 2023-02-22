/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <setupapi.h>

#include <libgen.h>

#include "serial_com.h"

#define ROBERT_BOSCH_USB_VID   (0x108C)

#define BST_APP30_CDC_USB_PID  (0xAB3C)
#define BST_APP20_CDC_USB_PID  (0xAB2C)

static HANDLE serial_handle = NULL;

static int8_t usb_cdc_acm_open(uint32_t baud_rate, uint16_t vid, uint16_t pid);
static int8_t usb_cdc_acm_close(void);

int8_t scom_open_id(uint32_t baud_rate, uint16_t vendor_id, uint16_t product_id)
{
    return usb_cdc_acm_open(baud_rate, vendor_id, product_id);
}

int8_t scom_open(void)
{
    int8_t com_rslt;

    com_rslt = scom_open_id(115200, ROBERT_BOSCH_USB_VID, BST_APP30_CDC_USB_PID);

    if (com_rslt != SCOM_OK)
    {
        com_rslt = scom_open_id(115200, ROBERT_BOSCH_USB_VID, BST_APP20_CDC_USB_PID);
    }

    return com_rslt;
}

int8_t scom_close(void)
{
    return usb_cdc_acm_close();
}

int8_t scom_read(void *buffer, uint32_t n_bytes, uint32_t *n_bytes_read)
{
    if (serial_handle)
    {
        if (ReadFile(serial_handle, buffer, n_bytes, (PDWORD)n_bytes_read, NULL))
        {
            return SCOM_OK;
        }
        else
        {
            return SCOM_E_READ_FAILED;
        }
    }
    else
    {
        return SCOM_E_PORT_NOT_OPEN;
    }
}

int8_t scom_write(void *buffer, uint32_t n_bytes)
{
    DWORD bytes_written = 0;

    if (serial_handle)
    {
        if (WriteFile(serial_handle, buffer, n_bytes, &bytes_written, NULL))
        {
            if (bytes_written == n_bytes)
            {
                return SCOM_OK;
            }
            else
            {
                return SCOM_E_WRITE_FAILED;
            }
        }
        else
        {
            return SCOM_E_WRITE_FAILED;
        }
    }
    else
    {
        return SCOM_E_PORT_NOT_OPEN;
    }
}

static int8_t usb_cdc_acm_open(uint32_t baud_rate, uint16_t vid, uint16_t pid)
{
    unsigned index;
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    char HardwareID[1024] = {};
    char usb_id[20] = {};

    sprintf(usb_id, "VID_%04X&PID_%04X", vid, pid);

    hDevInfo = SetupDiGetClassDevs(NULL, TEXT("USB"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    for (index = 0;; index++)
    {
        DeviceInfoData.cbSize = sizeof(DeviceInfoData);
        if (!SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData))
        {
            return SCOM_E_DEV_NOT_FOUND;
        }

        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (BYTE *)HardwareID,
                                         sizeof(HardwareID), NULL);
        fflush(stdout);
        if (strstr(HardwareID, usb_id))
        {
            char COM_PortName[20] = { 0 };
            DWORD dwSize = sizeof(COM_PortName);
            DWORD dwType = 0;
            HKEY hDeviceRegistryKey = SetupDiOpenDevRegKey(hDevInfo,
                                                           &DeviceInfoData,
                                                           DICS_FLAG_GLOBAL,
                                                           0,
                                                           DIREG_DEV,
                                                           KEY_READ);

            if ((RegQueryValueEx(hDeviceRegistryKey, "PortName", NULL, &dwType, (LPBYTE)COM_PortName,
                                 &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
            {
                char str[20];

                snprintf(str, sizeof(str) - 1, "\\\\.\\%s", COM_PortName);
                serial_handle = CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

                if (serial_handle == INVALID_HANDLE_VALUE)
                {
                    return SCOM_E_PORT_IN_USE;
                }

                /* Reference for the code below https://www.pololu.com/docs/0J73/15.6 */

                /* Flush away any bytes previously read or written. */
                BOOL success = TRUE;
                success = FlushFileBuffers(serial_handle);
                if (!success)
                {
                    CloseHandle(serial_handle);

                    return SCOM_E_CONFIG_FAILED;
                }

                /* Configure read and write operations to time out after 100 ms. */
                COMMTIMEOUTS timeouts = { 0 };

                /*
                 * success = GetCommTimeouts(serial_handle, &timeouts);
                 * printf("TO: %x %x %x %x %x\r\n", timeouts.ReadIntervalTimeout, timeouts.ReadTotalTimeoutConstant,
                 * timeouts.ReadTotalTimeoutMultiplier, timeouts.WriteTotalTimeoutConstant,
                 * timeouts.WriteTotalTimeoutMultiplier);
                 */

                timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
                timeouts.ReadTotalTimeoutConstant = 0;
                timeouts.ReadTotalTimeoutMultiplier = 0;
                timeouts.WriteTotalTimeoutConstant = 0;
                timeouts.WriteTotalTimeoutMultiplier = 0;

                success = SetCommTimeouts(serial_handle, &timeouts);
                if (!success)
                {
                    CloseHandle(serial_handle);

                    return SCOM_E_CONFIG_FAILED;
                }

                /* Set the baud rate and other options. */
                DCB state = { 0 };
                state.DCBlength = sizeof(DCB);
                state.BaudRate = baud_rate;
                state.ByteSize = 8;
                state.Parity = NOPARITY;
                state.StopBits = ONESTOPBIT;
                state.fDtrControl = DTR_CONTROL_ENABLE;
                success = SetCommState(serial_handle, &state);
                if (!success)
                {
                    CloseHandle(serial_handle);

                    return SCOM_E_CONFIG_FAILED;
                }

                return SCOM_OK;
            }
        }
    }
}

static int8_t usb_cdc_acm_close(void)
{
    if (serial_handle)
    {
        CloseHandle(serial_handle);
        serial_handle = NULL;

        return SCOM_OK;
    }
    else
    {
        return SCOM_E_PORT_NOT_OPEN;
    }
}
