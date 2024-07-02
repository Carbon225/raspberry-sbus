#ifndef RCDRIVERS_TTY_H
#define RCDRIVERS_TTY_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "rcdrivers/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

int rcdrivers_tty_install(const char path[], bool blocking, uint8_t timeout);
enum rcdrivers_err_t rcdrivers_tty_uninstall(int fd);

int rcdrivers_tty_read(int fd, uint8_t buf[], int bufSize);
enum rcdrivers_err_t rcdrivers_tty_write(int fd, const uint8_t buf[], int count);

#ifdef __cplusplus
}
#endif

#endif // RCDRIVERS_TTY_H
