/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define min(a, b) \
    ({ typeof(a) _a = (a); typeof(b) _b = (b); (_a < _b) ? _a : _b; })
#define round_down(a, b) \
    ({ typeof(a) _a = (a); typeof(b) _b = (b); _a - (_a % _b); })

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sparse/sparse.h>

#include "fastboot.h"

static char ERROR[128];

char *fb_get_error(void)
{
    return ERROR;
}

static int check_response(usb_handle *usb, unsigned int size, char *response)
{
    unsigned char status[65];
    int r;

    for(;;) {
        r = usb_read(usb, status, 64);
        if(r < 0) {
            sprintf(ERROR, "status read failed (%s)", strerror(errno));
            usb_close(usb);
            return -1;
        }
        status[r] = 0;

        if(r < 4) {
            sprintf(ERROR, "status malformed (%d bytes)", r);
            usb_close(usb);
            return -1;
        }

        if(!memcmp(status, "INFO", 4)) {
            fprintf(stderr,"(bootloader) %s\n", status + 4);
            continue;
        }

        if(!memcmp(status, "OKAY", 4)) {
            if(response) {
                strcpy(response, (char*) status + 4);
            }
            return 0;
        }

        if(!memcmp(status, "FAIL", 4)) {
            if(r > 4) {
                sprintf(ERROR, "remote: %s", status + 4);
            } else {
                strcpy(ERROR, "remote failure");
            }
            return -1;
        }

        if(!memcmp(status, "DATA", 4) && size > 0){
            unsigned dsize = strtoul((char*) status + 4, 0, 16);
            if(dsize > size) {
                strcpy(ERROR, "data size too large");
                usb_close(usb);
                return -1;
            }
            return dsize;
        }

        strcpy(ERROR,"unknown status code");
        usb_close(usb);
        break;
    }

    return -1;
}

static int _command_start(usb_handle *usb, const char *cmd, unsigned size,
                          char *response)
{
    int cmdsize = strlen(cmd);
    int r;

    if(response) {
        response[0] = 0;
    }

    if(cmdsize > 64) {
        sprintf(ERROR,"command too large");
        return -1;
    }

    if(usb_write(usb, cmd, cmdsize) != cmdsize) {
        sprintf(ERROR,"command write failed (%s)", strerror(errno));
        usb_close(usb);
        return -1;
    }

    return check_response(usb, size, response);
}

static int _command_data(usb_handle *usb, const void *data, unsigned size)
{
    int r;

    r = usb_write(usb, data, size);
    if(r < 0) {
        sprintf(ERROR, "data transfer failure (%s)", strerror(errno));
        usb_close(usb);
        return -1;
    }
    if(r != ((int) size)) {
        sprintf(ERROR, "data transfer failure (short transfer)");
        usb_close(usb);
        return -1;
    }

    return r;
}

static int _command_end(usb_handle *usb)
{
    int r;
    r = check_response(usb, 0, 0);
    if(r < 0) {
        return -1;
    }
    return 0;
}

static int _command_send(usb_handle *usb, const char *cmd,
                         const void *data, unsigned size,
                         char *response)
{
    int r;
    if (size == 0) {
        return -1;
    }

    r = _command_start(usb, cmd, size, response);
    if (r < 0) {
        return -1;
    }

    r = _command_data(usb, data, size);
    if (r < 0) {
        return -1;
    }

    r = _command_end(usb);
    if(r < 0) {
        return -1;
    }

    return size;
}

static int _command_send_no_data(usb_handle *usb, const char *cmd,
                                 char *response)
{
    int r;

    return _command_start(usb, cmd, 0, response);
}

int fb_command(usb_handle *usb, const char *cmd)
{
    return _command_send_no_data(usb, cmd, 0);
}

int fb_command_response(usb_handle *usb, const char *cmd, char *response)
{
    return _command_send_no_data(usb, cmd, response);
}

int fb_download_data(usb_handle *usb, const void *data, unsigned size)
{
    char cmd[64];
    int r;

    sprintf(cmd, "download:%08x", size);
    r = _command_send(usb, cmd, data, size, 0);

    if(r < 0) {
        return -1;
    } else {
        return 0;
    }
}

int fb_oem_upload_data(char* mname, usb_handle *usb, char *data, unsigned int size)
{
    printf("Enter 'fb_oem_upload_data'\n");
    char cmd[64];
    int r;

    /* send upload command */
    sprintf(cmd, "oem upload:%s", mname);
    int cmdsize = strlen(cmd);
    if(usb_write(usb, cmd, cmdsize) != cmdsize) {
        printf("usb_write ERROR\n");
        sprintf(ERROR,"command write failed (%s)", strerror(errno));
        usb_close(usb);
        return -1;
    }

    printf("usb_write OK\n");

    /* recv data from board */
    int read = 0;
    while(1)
    {
        int xfer = size > (1024*1024) ? (1024*1024) : size;

        read = usb_read(usb, data,  xfer);

        if(read < 0)
        {
            printf("usb_read ERROR\n");
            return -1;
        }

        data += read;
        size -= read;

        if(size == 0)
            return 0;
    }

    return 0;
}

int fb_oem_memdump(char* mname, usb_handle *usb, char** data, unsigned int* size)
{
    printf("Enter 'fb_oem_memdump'\n");
    int  status = 0;
    char cmd[FB_COMMAND_SZ+1];
    char response[FB_RESPONSE_SZ+1];

    memset(cmd, 0, FB_COMMAND_SZ+1);
    sprintf(cmd, "oem memory:%s", mname);

    status = _command_start(usb, cmd, 0x40000000, response);
    printf("Data size=0x%x\n", status);
    if(status <= 0 ){
        return -1;
    }

    *size = status;
    *data = malloc(*size);

    status = fb_oem_upload_data(mname, usb, *data, *size);
    if(status < 0){
        return -1;
    }

    return 0;
}

int fb_oem_nanddump(unsigned int flash_addr, unsigned int data_size, unsigned int oob_per_page,
    usb_handle *usb, char** data, unsigned int* size)
{
    int read;
    char *buffer = NULL;
    char cmd[FB_COMMAND_SZ+1];
    char response[FB_RESPONSE_SZ+1];

    memset(cmd, 0, FB_COMMAND_SZ+1);
    sprintf(cmd, "oem nanddump:%x:%x:%x", flash_addr, data_size, oob_per_page);

    int cmdsize = strlen(cmd);
    if(usb_write(usb, cmd, cmdsize) != cmdsize) {
        sprintf(ERROR,"command write failed (%s)", strerror(errno));
        usb_close(usb);
        return -1;
    }

    *size = data_size;
    *data = malloc(data_size);
    if (NULL == *data)
    {
        printf("fail to malloc %x\n", data_size);
        return -1;
    }

    buffer = *data;

    while (data_size)
    {
        int xfer = data_size > (1024*1024) ? (1024*1024) : data_size;
        read = usb_read(usb, buffer,  xfer);

        if(read < 0)
        {
            printf("usb_read ERROR\n");
            return -1;
        }

        buffer += read;
        data_size -= read;
    }

    return 0;
}


int fb_oem_pagenanddump(unsigned int flash_addr, unsigned int data_size,
    usb_handle *usb, char** data, unsigned int* size,unsigned int ecc_enable)
{
    int read;
    char *buffer = NULL;
    char cmd[FB_COMMAND_SZ+1];
    char response[FB_RESPONSE_SZ+1];

    memset(cmd, 0, FB_COMMAND_SZ+1);
    sprintf(cmd, "oem pagenanddump:%x:%x:%x", ecc_enable,flash_addr, data_size);

    int cmdsize = strlen(cmd);
    if(usb_write(usb, cmd, cmdsize) != cmdsize) {
        sprintf(ERROR,"command write failed (%s)", strerror(errno));
        usb_close(usb);
        return -1;
    }

    *size = data_size;
    *data = malloc(data_size);
    if (NULL == *data)
    {
        printf("fail to malloc %x\n", data_size);
        return -1;
    }

    buffer = *data;

    while (data_size)
    {
        int xfer = data_size > (1024*1024) ? (1024*1024) : data_size;
        read = usb_read(usb, buffer,  xfer);

        if(read < 0)
        {
            printf("usb_read ERROR\n");
            return -1;
        }

        buffer += read;
        data_size -= read;
    }

    return 0;
}

int fb_oem_mem_upload(unsigned int addr, unsigned int data_size,
    usb_handle *usb, char** data, unsigned int* size)
{
    int read;
    char *buffer = NULL;
    char cmd[FB_COMMAND_SZ+1];
    char response[FB_RESPONSE_SZ+1];

    memset(cmd, 0, FB_COMMAND_SZ+1);
    sprintf(cmd, "oem memupload:%x:%x", addr, data_size);

    int cmdsize = strlen(cmd);
    if(usb_write(usb, cmd, cmdsize) != cmdsize) {
        sprintf(ERROR,"command write failed (%s)", strerror(errno));
        usb_close(usb);
        return -1;
    }

    printf("command write ok\n");

    *size = data_size;
    *data = malloc(data_size);
    if (NULL == *data)
    {
        printf("fail to malloc %x\n", data_size);
        return -1;
    }

    buffer = *data;
    printf("usb_read start data_size = 0x%x\n",data_size);
    while (data_size)
    {
        int xfer = data_size > (1024*1024) ? (1024*1024) : data_size;
        read = usb_read(usb, buffer,  xfer);

        if(read < 0)
        {
            printf("usb_read ERROR\n");
            return -1;
        }

        buffer += read;
        data_size -= read;
    }
    printf("usb_read end read_size = 0x%x\n",*size );
    return 0;
}



#define USB_BUF_SIZE 512
static char usb_buf[USB_BUF_SIZE];
static int usb_buf_len;

static int fb_download_data_sparse_write(void *priv, const void *data, int len)
{
    int r;
    usb_handle *usb = priv;
    int to_write;
    const char *ptr = data;

    if (usb_buf_len) {
        to_write = min(USB_BUF_SIZE - usb_buf_len, len);

        memcpy(usb_buf + usb_buf_len, ptr, to_write);
        usb_buf_len += to_write;
        ptr += to_write;
        len -= to_write;
    }

    if (usb_buf_len == USB_BUF_SIZE) {
        r = _command_data(usb, usb_buf, USB_BUF_SIZE);
        if (r != USB_BUF_SIZE) {
            return -1;
        }
        usb_buf_len = 0;
    }

    if (len > USB_BUF_SIZE) {
        if (usb_buf_len > 0) {
            sprintf(ERROR, "internal error: usb_buf not empty\n");
            return -1;
        }
        to_write = round_down(len, USB_BUF_SIZE);
        r = _command_data(usb, ptr, to_write);
        if (r != to_write) {
            return -1;
        }
        ptr += to_write;
        len -= to_write;
    }

    if (len > 0) {
        if (len > USB_BUF_SIZE) {
            sprintf(ERROR, "internal error: too much left for usb_buf\n");
            return -1;
        }
        memcpy(usb_buf, ptr, len);
        usb_buf_len = len;
    }

    return 0;
}

static int fb_download_data_sparse_flush(usb_handle *usb)
{
    int r;

    if (usb_buf_len > 0) {
        r = _command_data(usb, usb_buf, usb_buf_len);
        if (r != usb_buf_len) {
            return -1;
        }
        usb_buf_len = 0;
    }

    return 0;
}

int fb_download_data_sparse(usb_handle *usb, struct sparse_file *s)
{
    char cmd[64];
    int r;
    int size = sparse_file_len(s, true, false);
    if (size <= 0) {
        return -1;
    }

    sprintf(cmd, "download:%08x", size);
    r = _command_start(usb, cmd, size, 0);
    if (r < 0) {
        return -1;
    }

    r = sparse_file_callback(s, true, false, fb_download_data_sparse_write, usb);
    if (r < 0) {
        return -1;
    }

    fb_download_data_sparse_flush(usb);

    return _command_end(usb);
}
