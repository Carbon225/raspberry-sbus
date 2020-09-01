#include "sbus_driver.h"

#include <unistd.h>
#include <fcntl.h>
#include <asm/termbits.h>
#include <asm/ioctls.h>
#include <sys/ioctl.h>

sbus_err_t sbus_decode(const uint8_t packet[],
                       uint16_t channels[],
                       uint8_t *opt)
{
    if (!packet || !channels || !opt) {
        return SBUS_ERR_INVALID_ARG;
    }
    if (*packet != SBUS_HEADER || packet[24] != SBUS_END) {
        return SBUS_FAIL;
    }

    // TODO rewrite decoder

    uint8_t *payload = packet + 1;
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

    *opt = packet[23] & 0xf;

    return SBUS_OK;
}

sbus_err_t sbus_encode(uint8_t packet[25],
                       const uint16_t channels[16],
                       uint8_t opt)
{
    if (!packet || !channels) {
        return SBUS_ERR_INVALID_ARG;
    }

    packet[0] = SBUS_HEADER;
    packet[24] = SBUS_END;

    packet[1] = channels[0] & 0xff;
    packet[2] = channels[0] >> 8 & 0b111;
    int currentByte = 2;
    int usedBits = 3; // from LSB

    for (int ch = 1; ch < 16; ch++)
    {
        // while channel not fully encoded
        for (int bitsWritten = 0; bitsWritten < 11;)
        {
            // strip written bits, shift over used bits
            packet[currentByte] |= channels[ch] >> bitsWritten << usedBits & 0xff;

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

    packet[23] = opt;

    return SBUS_OK;
}

sbus_err_t sbus_install(int *fd, const char *path, int blocking)
{
    *fd = open(path, O_RDWR | O_NOCTTY | (blocking ? 0 : O_NONBLOCK));
    if (*fd == -1)
    {
        return SBUS_ERR_OPEN;
    }

    struct termios2 options;
    if (ioctl(*fd, TCGETS2, &options) != 0)
    {
        return SBUS_ERR_TCGETS2;
    }

    options.c_cflag |= PARENB;
    options.c_cflag |= CSTOPB;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CREAD | CLOCAL;

    options.c_lflag &= ~ICANON;
    options.c_lflag &= ~ECHO;
    options.c_lflag &= ~ECHOE;
    options.c_lflag &= ~ECHONL;
    options.c_lflag &= ~ISIG;

    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    options.c_oflag &= ~OPOST;
    options.c_oflag &= ~ONLCR;

    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = SBUS_PACKET_SIZE;

    options.c_cflag &= ~CBAUD;
    options.c_cflag |= BOTHER;
    options.c_ispeed = options.c_ospeed = SBUS_BAUD;

    if (ioctl(*fd, TCSETS2, &options) != 0)
    {
        return SBUS_ERR_TCSETS2;
    }

    return SBUS_OK;
}

sbus_err_t sbus_uninstall(const int *fd)
{
    if (*fd)
        close(*fd);
}

int sbus_read(const int *fd, uint8_t *out, int bufSize)
{
    return read(*fd, out, bufSize);
}

sbus_err_t sbus_write(const int *fd, const uint16_t *channels, uint8_t opt)
{
    uint8_t packet[SBUS_PACKET_SIZE] = { 0 };
    sbus_encode(packet, channels, opt);

    if (write(*fd, packet, SBUS_PACKET_SIZE) != SBUS_PACKET_SIZE)
    {
        return SBUS_FAIL;
    }

    return SBUS_OK;
}
