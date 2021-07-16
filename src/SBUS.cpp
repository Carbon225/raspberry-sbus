#include "SBUS.h"
#include <cstdio>
#include <termios.h>

#define READ_BUF_SIZE (SBUS_PACKET_SIZE * 2)

SBUS::SBUS()
    : _nextRead(READ_BUF_SIZE)
    , _fd(-1)
{}

SBUS::~SBUS()
{
    uninstall();
}

sbus_err_t SBUS::install(const char *path, bool blocking, uint8_t timeout)
{
    return sbus_install(&_fd, path, blocking, timeout);
}

sbus_err_t SBUS::uninstall()
{
    return _fd < 0 ? SBUS_OK : sbus_uninstall(&_fd);
}

sbus_err_t SBUS::onPacket(sbus_packet_cb cb)
{
    return _decoder.onPacket(cb);
}

sbus_err_t SBUS::read()
{
    uint8_t readBuf[READ_BUF_SIZE];

    if (_fd < 0)
        return SBUS_FAIL;

    int nRead = sbus_read(&_fd, readBuf, _nextRead);

    // TODO SBUS_OK if timeout, else error
    if (nRead <= 0)
        return SBUS_OK;

    bool hadDesync = false;
    _decoder.feed(readBuf, nRead, &hadDesync, &_nextRead);

    if (hadDesync)
    {
        tcflush(_fd, TCIFLUSH);
        _nextRead = READ_BUF_SIZE;
    }

    return hadDesync ? SBUS_ERR_DESYNC : SBUS_OK;
}

sbus_err_t SBUS::write(sbus_packet_t packet)
{
    uint8_t opt = 0;

    if (packet.ch17)
        opt |= SBUS_OPT_C17;

    if (packet.ch18)
        opt |= SBUS_OPT_C18;

    if (packet.failsafe)
        opt |= SBUS_OPT_FS;

    if (packet.frameLost)
        opt |= SBUS_OPT_FL;

    return sbus_write(&_fd, packet.channels, opt);
}

uint16_t SBUS::channel(int num) const
{
    if (num >= 0 & num < 16)
        return _decoder.lastPacket().channels[num];
    else
        return 0;
}

const sbus_packet_t& SBUS::lastPacket() const
{
    return _decoder.lastPacket();
}
