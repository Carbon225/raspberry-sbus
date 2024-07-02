#ifndef RPISBUS_SBUS_LOW_LATENCY_H
#define RPISBUS_SBUS_LOW_LATENCY_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "rcdrivers/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

enum rcdriver_err_t tty_set_low_latency(int fd, bool setLowLatency);

#ifdef __cplusplus
}
#endif

#endif  // RPISBUS_SBUS_LOW_LATENCY_H
