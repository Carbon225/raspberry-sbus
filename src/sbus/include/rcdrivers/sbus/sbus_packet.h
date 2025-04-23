#ifndef RCDRIVERS_SBUS_PACKET_H
#define RCDRIVERS_SBUS_PACKET_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "rcdrivers/sbus/sbus_spec.h"

struct sbus_packet_t
{
    uint16_t channels[SBUS_NUM_CHANNELS];
    bool ch17, ch18;
    bool failsafe;
    bool frameLost;
};

#endif
