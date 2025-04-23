#include "rcdrivers/tty/tty_low_latency_impl.h"
#ifndef RCDRIVERS_LOW_LATENCY_IMPL

#include "rcdrivers/tty/tty_low_latency.h"

#include <stdio.h>

enum rcdrivers_err_t tty_set_low_latency(int fd, bool setLowLatency)
{
    fprintf(stderr, "Low latency mode not supported on this system.\n");
    return RCDRIVERS_FAIL;
}

#endif // RCDRIVERS_LOW_LATENCY_IMPL
