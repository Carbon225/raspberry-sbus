#include "rcdrivers/CRSF.h"

#include "rcdrivers/tty/tty.h"
#include "rcdrivers/tty/tty_low_latency.h"

CRSF::CRSF() noexcept
    : _fd(-1)
{}

CRSF::~CRSF() noexcept
{
    uninstall();
}

rcdrivers_err_t CRSF::install(const char path[], bool blocking, uint8_t timeout)
{
    _fd = rcdrivers_tty_install(path, blocking, timeout);
    return _fd < 0 ? RCDRIVERS_FAIL : RCDRIVERS_OK;
}

rcdrivers_err_t CRSF::uninstall()
{
    return _fd < 0 ? RCDRIVERS_OK : rcdrivers_tty_uninstall(_fd);
}

rcdrivers_err_t CRSF::setLowLatencyMode(bool enable)
{
    return _fd < 0 ? RCDRIVERS_FAIL : tty_set_low_latency(_fd, enable);
}

rcdrivers_err_t CRSF::onPacket(crsf_packet_cb cb)
{
    return _decoder.onPacket(cb);
}

rcdrivers_err_t CRSF::read()
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

rcdrivers_err_t CRSF::write(const crsf_packet_t &packet)
{
    rcdrivers_err_t err = CRSFDecoder::encode(_writeBuf, &packet);
    if (err)
        return err;
    return rcdrivers_tty_write(_fd, _writeBuf, _writeBuf[CRSF_PACKET_LEN_BYTE] + 2);
}
