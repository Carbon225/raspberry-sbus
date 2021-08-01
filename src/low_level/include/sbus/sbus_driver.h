#ifndef RPISBUS_SBUS_DRIVER_H
#define RPISBUS_SBUS_DRIVER_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "sbus/sbus_error.h"

#ifdef __cplusplus
extern "C" {
#endif

            int sbus_install(const char path[], bool blocking, uint8_t timeout);
enum sbus_err_t sbus_uninstall(int fd);

            int sbus_read(int fd, uint8_t buf[], int bufSize);
enum sbus_err_t sbus_write(int fd, const uint8_t packet[]);

#ifdef __cplusplus
}
#endif

#endif //RPISBUS_SBUS_DRIVER_H
