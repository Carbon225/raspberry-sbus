#ifndef RPISBUS_SBUS_DEFS_H
#define RPISBUS_SBUS_DEFS_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#define SBUS_NUM_CHANNELS (16)
#define SBUS_BAUD (100000)
#define SBUS_PACKET_SIZE (25)

#define SBUS_HEADER (0x0f)
#define SBUS_END (0x00)

#define SBUS_OPT_C17 (0b0001)
#define SBUS_OPT_C18 (0b0010)
#define SBUS_OPT_FS  (0b1000)
#define SBUS_OPT_FL  (0b0100)

typedef enum
{
    SBUS_OK = 0,
    SBUS_ERR_OPEN = -1,
    SBUS_ERR_TCGETS2 = -2,
    SBUS_ERR_TCSETS2 = -3,
    SBUS_FAIL = -4,
    SBUS_ERR_INVALID_ARG = -5,
    SBUS_ERR_DESYNC = -6,
} sbus_err_t;

typedef struct
{
    uint16_t channels[SBUS_NUM_CHANNELS];
    bool ch17, ch18;
    bool failsafe;
    bool frameLost;
} sbus_packet_t;

#endif
