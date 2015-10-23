

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "usb_acm.h"
#include "dload.h"

static int acm_at_fd = NULL;

int acm_write(const void *data, int len)
{
    int n;
    n = write(acm_at_fd, data, len);
    if(n<0)
    {
	Dbug("Error:about to write (fd=%d, len=%d)\n", acm_at_fd, len);
        return ERROR;
    }
    return n;
}

int acm_read(void *data, int len)
{
    int n;
	
    n = read(acm_at_fd, data, len);
    if(n<0)
    {
	Dbug("Error:about to read (fd=%d, len=%d)\n", acm_at_fd, len);
        return ERROR;
    }
	
    return n;
}

int  acm_usb_open(char* dev)
{
    int fd;
    
    Dbug("[ opening device ... ]\n");
    do {
        /* XXX use inotify? */
        fd = open(DEV_ACM_AT_NAME, O_RDWR);
        if (fd < 0) {
            usleep(1000*1000);
        }
    } while (fd < 0);
    
    Dbug("[ opening device succeeded ]\n");
    acm_at_fd = fd;
    
    return fd;    
}

int acm_usb_close()
{
    close(acm_at_fd);
    
    Dbug("[ closeing device succeeded ]\n");
    return 0;
}



