#ifndef RPISBUS_SBUS_DRIVER_H
#define RPISBUS_SBUS_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#include <cstdint>
#include <cstdbool>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "sbus/sbus_spec.h"

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

sbus_err_t sbus_decode(const uint8_t buf[SBUS_PACKET_SIZE],
                       sbus_packet_t *packet);

sbus_err_t sbus_encode(uint8_t buf[SBUS_PACKET_SIZE],
                       const sbus_packet_t *packet);

       int sbus_install(const char path[], bool blocking, uint8_t timeout);
sbus_err_t sbus_uninstall(int fd);

       int sbus_read(int fd, uint8_t buf[], int bufSize);
sbus_err_t sbus_write(int fd, const sbus_packet_t *packet);

#ifdef __cplusplus
}
#endif

#endif //RPISBUS_SBUS_DRIVER_H
