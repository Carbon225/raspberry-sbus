#include "SBUS.h"
#include <cstdio>

#define SBUS_DESYNC_END 2
#define SBUS_DESYNC_HDR 1
#define READ_BUF_SIZE (SBUS_PACKET_SIZE * 2)

SBUS::SBUS()
{}

SBUS::~SBUS()
{
    sbus_uninstall(&_fd);
}

sbus_err_t SBUS::install(const char *path, bool blocking)
{
    return sbus_install(&_fd, path, blocking);
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
    uint8_t readBuf[READ_BUF_SIZE];
    int nRead = sbus_read(&_fd, readBuf, READ_BUF_SIZE);

    bool hadDesync = false;

    for (int i = 0; i < nRead; i++)
    {
        switch (_state)
        {
            case State::WAIT_FOR_HEADER:
                if (readBuf[i] == SBUS_HEADER)
                {
                    _packet[0] = SBUS_HEADER;
                    _packetPos = 1;
                    _state = State::PACKET;
                }
                break;

            case State::PACKET:
                _packet[_packetPos++] = readBuf[i];
                if (_packetPos >= SBUS_PACKET_SIZE)
                {
                    if (verifyPacket())
                    {
                        decodePacket();
                    }
                    else
                    {
                        _state = State::WAIT_FOR_HEADER;
                        hadDesync = true;
                    }
                }
                break;
        }
    }

    return hadDesync ? SBUS_ERR_DESYNC : SBUS_OK;
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

bool SBUS::verifyPacket()
{
    return (_packet[0] == SBUS_HEADER) && (_packet[SBUS_PACKET_SIZE - 1] == SBUS_END);
}

void SBUS::decodePacket()
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
