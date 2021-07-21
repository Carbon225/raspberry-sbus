#include "sbus/DecoderFSM.h"

DecoderFSM::DecoderFSM()
        : _state(State::WAIT_FOR_HEADER)
        , _packetPos(0)
        , _lastPacket({0})
        , _packetCb(nullptr)
{

}

sbus_err_t DecoderFSM::feed(const uint8_t buf[], int n, bool *hadDesync)
{
    if (hadDesync)
        *hadDesync = false;

    for (int i = 0; i < n; i++)
    {
        switch (_state)
        {
            case State::WAIT_FOR_HEADER:
                if (buf[i] == SBUS_HEADER)
                {
                    _packetBuf[0] = SBUS_HEADER;
                    _packetPos = 1;
                    _state = State::PACKET;
                }
                break;

            case State::PACKET:
                _packetBuf[_packetPos] = buf[i];
                _packetPos++;
                if (_packetPos >= SBUS_PACKET_SIZE)
                {
                    if (verifyPacket() == SBUS_OK &&
                        decodePacket() == SBUS_OK)
                    {
                        if (hadDesync)
                            *hadDesync = false;
                        notifyCallback();
                    }

                    else if (hadDesync)
                        *hadDesync = true;

                    _state = State::WAIT_FOR_HEADER;
                    _packetPos = 0;
                }
                break;
        }
    }

    return SBUS_OK;
}

sbus_err_t DecoderFSM::verifyPacket()
{
    if (_packetBuf[0] == SBUS_HEADER &&
        _packetBuf[SBUS_PACKET_SIZE - 1] == SBUS_END)
        return SBUS_OK;
    else
        return SBUS_FAIL;
}

sbus_err_t DecoderFSM::decodePacket()
{
    uint8_t opt = 0;
    sbus_err_t ret = sbus_decode(_packetBuf, _lastPacket.channels, &opt);

    if (ret)
        return ret;

    _lastPacket.ch17 = (bool) (opt & SBUS_OPT_C17);
    _lastPacket.ch18 = (bool) (opt & SBUS_OPT_C18);
    _lastPacket.failsafe = (bool) (opt & SBUS_OPT_FS);
    _lastPacket.frameLost = (bool) (opt & SBUS_OPT_FL);

    return SBUS_OK;
}

bool DecoderFSM::notifyCallback()
{
    if (_packetCb)
        _packetCb(_lastPacket);
    return _packetCb;
}

const sbus_packet_t& DecoderFSM::lastPacket() const
{
    return _lastPacket;
}

sbus_err_t DecoderFSM::onPacket(sbus_packet_cb cb)
{
    _packetCb = cb;
    return 0;
}
