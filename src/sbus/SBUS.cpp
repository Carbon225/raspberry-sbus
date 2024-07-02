#include "rcdrivers/SBUS.h"
#include "rcdrivers/tty/tty.h"
#include "rcdrivers/tty/tty_low_latency.h"
#include "rcdrivers/sbus/packet_decoder.h"

SBUS::SBUS() noexcept
    : _fd(-1)
{}

SBUS::~SBUS() noexcept
{
    uninstall();
}

rcdriver_err_t SBUS::install(const char path[], bool blocking, uint8_t timeout)
{
    _fd = sbus_install(path, blocking, timeout);
    return _fd < 0 ? RCDRIVER_FAIL : RCDRIVER_OK;
}

rcdriver_err_t SBUS::uninstall()
{
    return _fd < 0 ? RCDRIVER_OK : sbus_uninstall(_fd);
}

rcdriver_err_t SBUS::setLowLatencyMode(bool enable)
{
    return _fd < 0 ? RCDRIVER_FAIL : tty_set_low_latency(_fd, enable);
}

rcdriver_err_t SBUS::onPacket(sbus_packet_cb cb)
{
    return _decoder.onPacket(cb);
}

rcdriver_err_t SBUS::read()
{
    if (_fd < 0)
        return RCDRIVER_FAIL;

    int nRead = sbus_read(_fd, _readBuf, READ_BUF_SIZE);

    // TODO SBUS_OK if timeout, else error
    if (nRead <= 0)
        return RCDRIVER_OK;

    bool hadDesync = false;
    _decoder.feed(_readBuf, nRead, &hadDesync);

    return hadDesync ? RCDRIVER_ERR_DESYNC : RCDRIVER_OK;
}

rcdriver_err_t SBUS::write(const sbus_packet_t &packet)
{
    rcdriver_err_t err = sbus_encode(_writeBuf, &packet);
    if (err)
        return err;
    return sbus_write(_fd, _writeBuf, SBUS_PACKET_SIZE);
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
