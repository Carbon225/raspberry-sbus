#include "sbus/sbus_low_latency_impl.h"
#ifdef RPISBUS_LOW_LATENCY_IMPL_LINUX

#include "sbus/sbus_low_latency.h"

// thanks to https://github.com/projectgus/hairless-midiserial/blob/add59f04c3b75044f3033f70d5523685b6b9dd0a/src/PortLatency_linux.cpp

#include <sys/ioctl.h>
#include <linux/serial.h>
#include <stdio.h>

enum sbus_err_t sbus_set_low_latency(int fd, bool setLowLatency)
{
    struct serial_struct ser_info;
    int err = ioctl(fd, TIOCGSERIAL, &ser_info);
    if (err)
        goto error;

    if ((ser_info.flags & ASYNC_LOW_LATENCY) == setLowLatency)
        // mode already set
        return SBUS_OK;

    if (setLowLatency)
        ser_info.flags |= ASYNC_LOW_LATENCY;
    else
        ser_info.flags &= ~ASYNC_LOW_LATENCY;

    err = ioctl(fd, TIOCSSERIAL, &ser_info);
    if (err)
        goto error;

    return SBUS_OK;

    error:
    perror("Could not set low latency mode\n");
    return SBUS_FAIL;
}

#endif // RPISBUS_LOW_LATENCY_IMPL_LINUX
