#include "rcdrivers/SBUS.h"
#include "rcdrivers/tty/tty.h"
#include "rcdrivers/tty/tty_low_latency.h"
#include "rcdrivers/sbus/packet_decoder.h"
#include "rcdrivers/sbus/sbus_spec.h"

SBUS::SBUS() noexcept
    : _fd(-1)
{}

SBUS::~SBUS() noexcept
{
    uninstall();
}

rcdrivers_err_t SBUS::install(const char path[], bool blocking, uint8_t timeout)
{
    _fd = rcdrivers_tty_install(path, blocking, timeout, SBUS_BAUD, true, true);
    return _fd < 0 ? RCDRIVERS_FAIL : RCDRIVERS_OK;
}

rcdrivers_err_t SBUS::uninstall()
{
    return _fd < 0 ? RCDRIVERS_OK : rcdrivers_tty_uninstall(_fd);
}

rcdrivers_err_t SBUS::setLowLatencyMode(bool enable)
{
    return _fd < 0 ? RCDRIVERS_FAIL : tty_set_low_latency(_fd, enable);
}

rcdrivers_err_t SBUS::onPacket(sbus_packet_cb cb)
{
    return _decoder.onPacket(cb);
}

rcdrivers_err_t SBUS::read()
{
    if (_fd < 0)
        return RCDRIVERS_FAIL;

    int nRead = rcdrivers_tty_read(_fd, _readBuf, READ_BUF_SIZE);

    // TODO OK if timeout, else error
    if (nRead <= 0)
        return RCDRIVERS_OK;

    bool hadDesync = false;
    _decoder.feed(_readBuf, nRead, &hadDesync);

    return hadDesync ? RCDRIVERS_ERR_DESYNC : RCDRIVERS_OK;
}

rcdrivers_err_t SBUS::write(const sbus_packet_t &packet)
{
    rcdrivers_err_t err = sbus_encode(_writeBuf, &packet);
    if (err)
        return err;
    return rcdrivers_tty_write(_fd, _writeBuf, SBUS_PACKET_SIZE);
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
