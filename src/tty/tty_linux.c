#include "rcdrivers/tty/tty_impl.h"
#ifdef RCDRIVERS_TTY_IMPL_LINUX

#include "rcdrivers/tty/tty.h"

#include <unistd.h>
#include <fcntl.h>
#include <asm/termbits.h>
#include <sys/ioctl.h>

int rcdrivers_tty_install(const char path[], bool blocking, uint8_t timeout, int baud, bool stopBits2, bool parityEven)
{
    int fd = open(path, O_RDWR | O_NOCTTY | (blocking ? 0 : O_NONBLOCK));
    if (fd < 0)
    {
        return RCDRIVERS_ERR_OPEN;
    }

    struct termios2 options;
    if (ioctl(fd, TCGETS2, &options))
    {
        return RCDRIVERS_ERR_TCGETS2;
    }

    // sbus options
    // see man termios(3)

    if (parityEven)
    {
        options.c_cflag |= PARENB;  // enable parity
        options.c_cflag &= ~PARODD; // even parity
    }
    else
    {
        options.c_cflag &= ~PARENB;  // disable parity
        options.c_cflag &= ~PARODD; // even parity
    }

    if (stopBits2)
    {
        options.c_cflag |= CSTOPB;  // enable 2 stop bits
    }
    else
    {
        options.c_cflag &= ~CSTOPB;  // disable 2 stop bits
    }

    options.c_cflag &= ~CSIZE;  // clear character size mask
    options.c_cflag |= CS8;     // 8 bit characters
    options.c_cflag &= ~CRTSCTS;  // disable hardware flow control
    options.c_cflag |= CREAD;   // enable receiver
    options.c_cflag |= CLOCAL;  // ignore modem lines

    options.c_lflag &= ~ICANON;  // receive characters as they come in
    options.c_lflag &= ~ECHO;    // do not echo
    options.c_lflag &= ~ISIG;    // do not generate signals
    options.c_lflag &= ~IEXTEN;  // disable implementation-defined processing

    options.c_iflag &= ~(IXON | IXOFF | IXANY);  // disable XON/XOFF flow control
    options.c_iflag |= IGNBRK;   // ignore BREAK condition
    options.c_iflag |= INPCK;    // enable parity checking
    options.c_iflag |= IGNPAR;   // ignore framing and parity errors
    options.c_iflag &= ~ISTRIP;  // do not strip off 8th bit
    options.c_iflag &= ~INLCR;   // do not translate NL to CR
    options.c_iflag &= ~ICRNL;   // do not translate CR to NL
    options.c_iflag &= ~IGNCR;   // do not ignore CR

    options.c_oflag &= ~OPOST;  // disable implementation-defined processing
    options.c_oflag &= ~ONLCR;  // do not map NL to CR-NL
    options.c_oflag &= ~OCRNL;  // do not map CR to NL
    options.c_oflag &= ~(ONOCR | ONLRET);  // output CR like a normal person
    options.c_oflag &= ~OFILL;  // no fill characters

    // set timeouts
    if (blocking && timeout == 0)
    {
        // wait for at least 1 byte
        options.c_cc[VTIME] = 0;
        options.c_cc[VMIN] = 1;
    }
    else if (blocking) // timeout > 0
    {
        // wait for at least 1 byte or timeout
        options.c_cc[VTIME] = timeout;
        options.c_cc[VMIN] = 0;
    }
    else // !blocking
    {
        // non-blocking
        options.c_cc[VTIME] = 0;
        options.c_cc[VMIN] = 0;
    }

    // set SBUS baud
    options.c_cflag &= ~CBAUD;
    options.c_cflag |= BOTHER;
    options.c_ispeed = options.c_ospeed = baud;

    if (ioctl(fd, TCSETS2, &options))
    {
        return RCDRIVERS_ERR_TCSETS2;
    }

    return fd;
}

enum rcdrivers_err_t rcdrivers_tty_uninstall(int fd)
{
    return close(fd);
}

int rcdrivers_tty_read(int fd, uint8_t buf[], int bufSize)
{
    if (!buf)
        return RCDRIVERS_ERR_INVALID_ARG;
    return read(fd, buf, bufSize);
}

enum rcdrivers_err_t rcdrivers_tty_write(int fd, const uint8_t buf[], int count)
{
    if (!buf)
        return RCDRIVERS_ERR_INVALID_ARG;

    if (write(fd, buf, count) != count)
    {
        return RCDRIVERS_FAIL;
    }

    return RCDRIVERS_OK;
}

#endif // RCDRIVERS_TTY_IMPL_LINUX
