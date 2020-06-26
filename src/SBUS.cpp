#include "SBUS.h"
#include <cstdio>

#define SBUS_DESYNC_END 2
#define SBUS_DESYNC_HDR 1

SBUS::SBUS()
    : _desync(SBUS_DESYNC_END)
{}

SBUS::~SBUS()
{
    sbus_uninstall(&_fd);
}

sbus_err_t SBUS::install(const char *path)
{
    return sbus_install(&_fd, path);
}

uint16_t SBUS::channel(int num) const
{
    if (num >= 0 & num < 16)
        return channel(num);
    else
        return 0;
}

sbus_err_t SBUS::onPacket(sbus_packet_cb cb)
{
    _packetCb = cb;
    return SBUS_OK;
}

sbus_err_t SBUS::read()
{
    int nRead = sbus_read(&_fd, _packet + _packetPos);
    if (nRead == 1)
    {
        const uint8_t newByte = _packet[_packetPos];
        _packetPos += nRead;

        if (_packetPos >= SBUS_PACKET_SIZE)
        {
            if (newByte == SBUS_END && _packet[0] == SBUS_HEADER)
            {
                uint8_t opt = 0;
                sbus_err_t ret = sbus_decode(_packet, _channels, &opt);
                if (ret == SBUS_OK && _packetCb != nullptr)
                {
                    sbus_packet_t packet = {
                            _channels,
                            (bool) (opt & SBUS_OPT_C17),
                            (bool) (opt & SBUS_OPT_C18),
                            (bool) (opt & SBUS_OPT_FS),
                            (bool) (opt & SBUS_OPT_FL)
                    };
                    _packetCb(packet);
                }
            }
            else if (_desync == SBUS_OK)
            {
                fprintf(stderr, "Desync\n");
                _desync = SBUS_DESYNC_END;
            }
            _packetPos = 0;
        }

        if (_desync == SBUS_DESYNC_END && newByte == SBUS_END)
        {
            printf("Got packet end\n");
            _desync = SBUS_DESYNC_HDR;
        }
        else if (_desync == SBUS_DESYNC_HDR && newByte == SBUS_HEADER)
        {
            printf("Got packet header. Desync cleared\n");;
            _desync = SBUS_OK;
            _packetPos = 1;
            _packet[0] = newByte;
        }
    }

    return SBUS_OK;
}

sbus_err_t SBUS::write(sbus_packet_t packet)
{
    uint8_t opt = 0;
    opt |= packet.ch17 * SBUS_OPT_C17;
    opt |= packet.ch18 * SBUS_OPT_C18;
    opt |= packet.failsafe * SBUS_OPT_FS;
    opt |= packet.frameLost * SBUS_OPT_FL;
    sbus_write(&_fd, packet.channels, opt);
    return SBUS_OK;
}
