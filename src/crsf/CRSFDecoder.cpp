#include "rcdrivers/crsf/CRSFDecoder.h"

static rcdrivers_err_t verifyCrsfCrc8(const uint8_t packet[])
{
    return RCDRIVERS_FAIL;
}

CRSFDecoder::CRSFDecoder()
        : _state(State::WAIT_FOR_HEADER)
        , _packetPos(0)
        , _packetCb(nullptr)
{}

rcdrivers_err_t CRSFDecoder::feed(const uint8_t buf[], int bufSize, bool *hadDesyncOut)
{
    return RCDRIVERS_FAIL;
}

rcdrivers_err_t CRSFDecoder::verifyPacket()
{
    if ((_packetBuf[0] == CRSF_SYNC_BYTE || _packetBuf[0] == CRSF_SYNC_BYTE_EDGETX) &&
        verifyCrsfCrc8(_packetBuf) == RCDRIVERS_OK)
        return RCDRIVERS_OK;
    else
        return RCDRIVERS_FAIL;
}

rcdrivers_err_t CRSFDecoder::decodePacket()
{
    return decode(_packetBuf, &_lastPacket);
}

bool CRSFDecoder::notifyCallback()
{
    if (_packetCb)
        _packetCb(_lastPacket);
    return static_cast<bool>(_packetCb);
}

const crsf_packet_t& CRSFDecoder::lastPacket() const
{
    return _lastPacket;
}

rcdrivers_err_t CRSFDecoder::onPacket(crsf_packet_cb cb)
{
    _packetCb = cb;
    return RCDRIVERS_OK;
}

rcdrivers_err_t CRSFDecoder::decode(const uint8_t buf[], crsf_packet_t *packet)
{
    return RCDRIVERS_FAIL;
}

rcdrivers_err_t CRSFDecoder::encode(uint8_t buf[], const crsf_packet_t *packet)
{
    return RCDRIVERS_FAIL;
}
