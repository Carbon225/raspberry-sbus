#ifndef RPISBUS_SBUS_DRIVER_H
#define RPISBUS_SBUS_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#define SBUS_OK 0
#define SBUS_ERR_OPEN -1
#define SBUS_ERR_TCGETS2 -2
#define SBUS_ERR_TCSETS2 -3
#define SBUS_FAIL -4
#define SBUS_ERR_INVALID_ARG -5
#define SBUS_ERR_DESYNC -6

#define SBUS_BAUD 100000
#define SBUS_PACKET_SIZE 25
#define SBUS_HEADER 0x0f
#define SBUS_END 0x00

#define SBUS_OPT_C17 (uint8_t) 0b0001
#define SBUS_OPT_C18 (uint8_t) 0b0010
#define SBUS_OPT_FS  (uint8_t) 0b1000
#define SBUS_OPT_FL  (uint8_t) 0b0100

typedef int sbus_err_t;

sbus_err_t sbus_decode(const uint8_t packet[SBUS_PACKET_SIZE],
                       uint16_t channels[16],
                       uint8_t *opt);

sbus_err_t sbus_encode(uint8_t packet[SBUS_PACKET_SIZE],
                       const uint16_t channels[16],
                       uint8_t opt);

sbus_err_t sbus_install(int *fd, const char *path, int blocking, uint8_t timeout);
sbus_err_t sbus_uninstall(const int *fd);

int sbus_read(const int *fd, uint8_t *out, int bufSize);
sbus_err_t sbus_write(const int *fd, const uint16_t *channels, uint8_t opt);

#ifdef __cplusplus
}
#endif

#endif //RPISBUS_SBUS_DRIVER_H
