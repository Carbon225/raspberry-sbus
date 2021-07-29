#ifndef RPISBUS_SBUS_LOW_LATENCY_H
#define RPISBUS_SBUS_LOW_LATENCY_H

#include "sbus/sbus_driver.h"
#include <stdbool.h>

// compiler flag to disable low latency mode
#ifndef SBUS_DISABLE_LOW_LATENCY_MODE
// the headers required by low latency mode might not be available everywhere
// so I added an option to disable compiling it.
#define SBUS_HAS_LOW_LATENCY_MODE
#endif // SBUS_DISABLE_LOW_LATENCY_MODE

#ifdef __cplusplus
extern "C" {
#endif

sbus_err_t sbus_set_low_latency(int fd, bool setLowLatency);

#ifdef __cplusplus
}
#endif

#endif  // RPISBUS_SBUS_LOW_LATENCY_H
