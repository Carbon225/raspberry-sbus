#include "sbus/sbus_low_latency_impl.h"
#ifndef RPISBUS_LOW_LATENCY_IMPL

#include "sbus/sbus_low_latency.h"

#include <stdio.h>

enum sbus_err_t sbus_set_low_latency(int fd, bool setLowLatency)
{
    fprintf(stderr, "Low latency mode not supported on this system.\n");
    return SBUS_FAIL;
}

#endif // RPISBUS_LOW_LATENCY_IMPL
