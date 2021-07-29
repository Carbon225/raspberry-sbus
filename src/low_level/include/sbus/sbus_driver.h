#ifndef RPISBUS_SBUS_DRIVER_H
#define RPISBUS_SBUS_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "sbus/sbus_defs.h"

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
