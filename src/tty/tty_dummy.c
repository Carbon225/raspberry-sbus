#include "rcdrivers/tty/tty_impl.h"
#ifdef RCDRIVERS_TTY_IMPL_DUMMY

#include "rcdrivers/tty/tty.h"
#include <stdlib.h>


int rcdrivers_tty_install(const char path[], bool blocking, uint8_t timeout)
{
    exit(1);
}

enum rcdrivers_err_t rcdrivers_tty_uninstall(int fd)
{
    exit(1);
}

int rcdrivers_tty_read(int fd, uint8_t buf[], int bufSize)
{
    exit(1);
}

enum rcdrivers_err_t rcdrivers_tty_write(int fd, const uint8_t buf[], int count)
{
    exit(1);
}

#endif // RCDRIVERS_TTY_IMPL_DUMMY
