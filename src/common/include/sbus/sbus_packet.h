#ifndef RPISBUS_SBUS_PACKET_H
#define RPISBUS_SBUS_PACKET_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "sbus/sbus_spec.h"

typedef struct
{
    uint16_t channels[SBUS_NUM_CHANNELS];
    bool ch17, ch18;
    bool failsafe;
    bool frameLost;
} sbus_packet_t;

#endif
