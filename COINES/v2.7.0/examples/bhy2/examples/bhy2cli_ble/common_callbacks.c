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
 * @file    common_callbacks.c
 * @brief   Source file for the command line utility callbacks
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include <sys/stat.h>
#if defined(PC)
#include <dirent.h>
#endif

#include "common_callbacks.h"
#include "coines.h"
#include "verbose.h"

extern bool echo_on;
extern bool heartbeat_on;
extern uint16_t stream_buff_len;

static size_t get_file_size(const char *file_name);

#ifndef PC
int8_t echo_help(void *ref)
{
    PRINT("  echo <on / off>\r\n");
    PRINT("    \t= Set the echo on or off\r\n");

    return CLI_OK;
}

int8_t echo_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    if (!strcmp((char *)argv[1], "on"))
    {
        echo_on = true;
    }
    else
    {
        echo_on = false;
    }

    PRINT("Setting echo to %s\r\n", echo_on ? "on" : "off");
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t heartbeat_help(void *ref)
{
    PRINT("  heart <on / off>\r\n");
    PRINT("    \t= Set the heartbeat message on or off\r\n");

    return CLI_OK;
}

int8_t heartbeat_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s\r\n", argv[0], argv[1]);
    if (!strcmp((char *)argv[1], "on"))
    {
        heartbeat_on = true;
    }
    else
    {
        heartbeat_on = false;
    }

    PRINT("Setting Heartbeat message to %s\r\n", heartbeat_on ? "on" : "off");
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t streambuff_help(void *ref)
{
    PRINT("  strbuf <buffer size>\r\n");
    PRINT("    \t= Set the streaming buffer size. Maximum of %u bytes\r\n", CLI_STREAM_BUF_MAX);

    return CLI_OK;
}

int8_t streambuff_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    uint16_t buffer_size;

    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    buffer_size = atoi((char *)argv[1]);

    if (buffer_size <= CLI_STREAM_BUF_MAX)
    {
        PRINT("Streaming buffer size set to %u\r\n", buffer_size);
        stream_buff_len = buffer_size;
    }
    else
    {
        ERROR("Invalid streaming buffer size of %u\r\n", buffer_size);
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

#endif

int8_t mklog_help(void *ref)
{
    PRINT("  mklog <filename.ext>\r\n");
    PRINT("    \t= Create a log file (write-only)\r\n");

    return CLI_OK;
}

int8_t mklog_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    FILE *fp = fopen((char *)argv[1], "wb");

    if (fp)
    {
        PRINT("File %s was created\r\n", argv[1]);
        fclose(fp);
    }
    else
    {
        ERROR("File %s could not be created\r\n", argv[1]);
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t rm_help(void *ref)
{
    PRINT("  rm <filename.ext>\r\n");
    PRINT("    \t= Remove a file\r\n");

    return CLI_OK;
}

int8_t rm_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s\r\n", argv[0], argv[1]);

    remove((char *)argv[1]);

    PRINT("File %s was removed\r\n", argv[1]);
    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t ls_help(void *ref)
{
    PRINT("  ls\t= List files in the flash\r\n");

    return CLI_OK;
}

int8_t ls_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    DIR *d;
    struct dirent *dir;

    INFO("Executing %s\r\n", argv[0]);

    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            /* Max File name size allowed in FLogFS is 40 characters */
            PRINT("%40s | %u B\r\n", dir->d_name, get_file_size(dir->d_name));
        }

        closedir(d);
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t wrlog_help(void *ref)
{
    PRINT("  wrlog <filename.ext> <content>\r\n");
    PRINT("    \t= Write some data to a log file\r\n");

    return CLI_OK;
}

int8_t wrlog_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    INFO("Executing %s %s %s\r\n", argv[0], argv[1], argv[2]);

    FILE *fp = fopen((char *)argv[1], "wb");

    if (fp)
    {
        fprintf(fp, "%s", argv[2]);
        fclose(fp);
    }
    else
    {
        ERROR("File not found or could not be created\r\n");
    }

    PRINT("\r\n\r\n");

    return CLI_OK;
}

int8_t cls_callback(uint8_t argc, uint8_t *argv[], void *ref)
{
    PRINT("\033c\033[2J");
#ifdef MCU_APP30
    fflush(bt_w);
#endif

    return CLI_OK;
}

int8_t cls_help(void *ref)
{
    PRINT("  cls \r\n");
    PRINT("    \t= Clear screen\r\n");

    return CLI_OK;
}

static size_t get_file_size(const char *file_name)
{
    struct stat st;

    stat(file_name, &st);

    return st.st_size;
}
