#include "sbus/sbus_driver.h"

#include <unistd.h>
#include <fcntl.h>
#include <asm/termbits.h>
#include <asm/ioctls.h>
#include <sys/ioctl.h>

sbus_err_t sbus_decode(const uint8_t buf[],
                       sbus_packet_t *packet)
{
    if (!packet || !buf) {
        return SBUS_ERR_INVALID_ARG;
    }
    if (buf[0] != SBUS_HEADER || buf[24] != SBUS_END) {
        return SBUS_FAIL;
    }

    uint16_t *channels = packet->channels;
    const uint8_t *payload = buf + 1;
    channels[0]  = (uint16_t)((payload[0]    | payload[1] << 8)                          & 0x07FF);
    channels[1]  = (uint16_t)((payload[1] >> 3 | payload[2] << 5)                        & 0x07FF);
    channels[2]  = (uint16_t)((payload[2] >> 6 | payload[3] << 2 | payload[4] << 10)     & 0x07FF);
    channels[3]  = (uint16_t)((payload[4] >> 1 | payload[5] << 7)                        & 0x07FF);
    channels[4]  = (uint16_t)((payload[5] >> 4 | payload[6] << 4)                        & 0x07FF);
    channels[5]  = (uint16_t)((payload[6] >> 7 | payload[7] << 1 | payload[8] << 9)      & 0x07FF);
    channels[6]  = (uint16_t)((payload[8] >> 2 | payload[9] << 6)                        & 0x07FF);
    channels[7]  = (uint16_t)((payload[9] >> 5 | payload[10] << 3)                       & 0x07FF);
    channels[8]  = (uint16_t)((payload[11]   | payload[12] << 8)                         & 0x07FF);
    channels[9]  = (uint16_t)((payload[12] >> 3 | payload[13] << 5)                      & 0x07FF);
    channels[10] = (uint16_t)((payload[13] >> 6 | payload[14] << 2 | payload[15] << 10)  & 0x07FF);
    channels[11] = (uint16_t)((payload[15] >> 1 | payload[16] << 7)                      & 0x07FF);
    channels[12] = (uint16_t)((payload[16] >> 4 | payload[17] << 4)                      & 0x07FF);
    channels[13] = (uint16_t)((payload[17] >> 7 | payload[18] << 1 | payload[19] << 9)   & 0x07FF);
    channels[14] = (uint16_t)((payload[19] >> 2 | payload[20] << 6)                      & 0x07FF);
    channels[15] = (uint16_t)((payload[20] >> 5 | payload[21] << 3)                      & 0x07FF);

    uint8_t opt = buf[23] & 0xf;
    packet->ch17      = opt & SBUS_OPT_C17;
    packet->ch18      = opt & SBUS_OPT_C18;
    packet->failsafe  = opt & SBUS_OPT_FS;
    packet->frameLost = opt & SBUS_OPT_FL;

    return SBUS_OK;
}

sbus_err_t sbus_encode(uint8_t buf[],
                       const sbus_packet_t *packet)
{
    if (!packet || !buf) {
        return SBUS_ERR_INVALID_ARG;
    }

    const uint16_t *channels = packet->channels;

    buf[0] = SBUS_HEADER;
    buf[24] = SBUS_END;

    buf[1] = channels[0] & 0xff;
    buf[2] = channels[0] >> 8 & 0b111;
    int currentByte = 2;
    int usedBits = 3; // from LSB

    for (int ch = 1; ch < 16; ch++)
    {
        // while channel not fully encoded
        for (int bitsWritten = 0; bitsWritten < 11;)
        {
            // strip written bits, shift over used bits
            buf[currentByte] |= channels[ch] >> bitsWritten << usedBits & 0xff;

            int hadToWrite = 11 - bitsWritten;
            int couldWrite = 8 - usedBits;

            int wrote = couldWrite;
            if (hadToWrite < couldWrite)
            {
                wrote = hadToWrite;
            }
            else
            {
                currentByte++;
            }

            bitsWritten += wrote;
            usedBits += wrote;
            usedBits %= 8;
        }
    }

    buf[23] = 0;

    if (packet->ch17)
        buf[23] |= SBUS_OPT_C17;

    if (packet->ch18)
        buf[23] |= SBUS_OPT_C18;

    if (packet->failsafe)
        buf[23] |= SBUS_OPT_FS;

    if (packet->frameLost)
        buf[23] |= SBUS_OPT_FL;

    return SBUS_OK;
}

int sbus_install(const char path[], bool blocking, uint8_t timeout)
{
    int fd = open(path, O_RDWR | O_NOCTTY | (blocking ? 0 : O_NONBLOCK));
    if (fd < 0)
    {
        return SBUS_ERR_OPEN;
    }

    struct termios2 options;
    if (ioctl(fd, TCGETS2, &options))
    {
        return SBUS_ERR_TCGETS2;
    }

    // sbus options
    // see man termios(3)

    options.c_cflag |= PARENB;  // enable parity
    options.c_cflag &= ~PARODD; // even parity
    options.c_cflag |= CSTOPB;  // enable 2 stop bits
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
    options.c_ispeed = options.c_ospeed = SBUS_BAUD;

    if (ioctl(fd, TCSETS2, &options))
    {
        return SBUS_ERR_TCSETS2;
    }

    return fd;
}

sbus_err_t sbus_uninstall(int fd)
{
    return close(fd);
}

int sbus_read(int fd, uint8_t buf[], int bufSize)
{
    return read(fd, buf, bufSize);
}

sbus_err_t sbus_write(int fd, const sbus_packet_t *packet)
{
    if (!packet)
        return SBUS_ERR_INVALID_ARG;

    uint8_t buf[SBUS_PACKET_SIZE] = { 0 };
    sbus_encode(buf, packet);

    if (write(fd, packet, SBUS_PACKET_SIZE) != SBUS_PACKET_SIZE)
    {
        return SBUS_FAIL;
    }

    return SBUS_OK;
}
