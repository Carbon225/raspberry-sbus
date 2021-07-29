#include "SBUS.h"
#include "sbus/sbus_driver.h"
#include "sbus/sbus_low_latency.h"

#define READ_BUF_SIZE (SBUS_PACKET_SIZE * 2)

SBUS::SBUS() noexcept
    : _fd(-1)
{}

SBUS::~SBUS() noexcept
{
    uninstall();
}

sbus_err_t SBUS::install(const char path[], bool blocking, uint8_t timeout)
{
    _fd = sbus_install(path, blocking, timeout);
    return _fd < 0 ? (sbus_err_t) _fd : SBUS_OK;
}

sbus_err_t SBUS::uninstall()
{
    return _fd < 0 ? SBUS_OK : sbus_uninstall(_fd);
}

sbus_err_t SBUS::setLowLatencyMode(bool enable)
{
    return _fd < 0 ? SBUS_FAIL : sbus_set_low_latency(_fd, enable);
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

    int nRead = sbus_read(_fd, readBuf, READ_BUF_SIZE);

    // TODO SBUS_OK if timeout, else error
    if (nRead <= 0)
        return SBUS_OK;

    bool hadDesync = false;
    _decoder.feed(readBuf, nRead, &hadDesync);

    return hadDesync ? SBUS_ERR_DESYNC : SBUS_OK;
}

sbus_err_t SBUS::write(sbus_packet_t packet)
{
    return sbus_write(_fd, &packet);
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
