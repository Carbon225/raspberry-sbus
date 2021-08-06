#include "sbus/DecoderFSM.h"
#include "sbus/packet_decoder.h"

DecoderFSM::DecoderFSM()
        : _state(State::WAIT_FOR_HEADER)
        , _packetPos(0)
        , _lastPacket({0})
        , _packetCb(nullptr)
{
    _lastPacket.failsafe = true;
    _lastPacket.frameLost = true;
}

sbus_err_t DecoderFSM::feed(const uint8_t buf[], int bufSize, bool *hadDesyncOut)
{
    bool hadDesync = false;

    int headerByte = -1;

    for (int i = 0; i < bufSize; i++)
    {
        switch (_state)
        {
            case State::WAIT_FOR_HEADER:
            case State::HEADER_SKIP:
                if (buf[i] == SBUS_HEADER)
                {
                    if (_state == State::HEADER_SKIP)
                    {
                        // skip this header
                        _state = State::WAIT_FOR_HEADER;
                        break;
                    }

                    // remember this as the last header
                    headerByte = i;

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
                        hadDesync = false;  // clear desync if last packet was ok
                        notifyCallback();

                        // receive next packet
                        _state = State::WAIT_FOR_HEADER;
                    }
                    // packet error but we found other possible headers
                    else if (headerByte >= 0)
                    {
                        // retry scanning after last header
                        i = headerByte;
                        _state = State::WAIT_FOR_HEADER;
                    }
                    else // out of headers to scan
                    {
                        hadDesync = true;
                        /*
                         * SBUS header is '15' and packet end is '0'.
                         * In case the packet looks like this:
                         * 15 .. 15 .. 00 15 .. 15 .. 00
                         * |------------| |------------|
                         * We could have locked on like this:
                         * 15 .. 15 .. 00 15 .. 15 .. 00
                         *       |------------| |------------|
                         * In this situation we would loop forever.
                         * So if a desync happens it is safer to skip the next header
                         * which makes sure we are always moving inside each packet
                         * and not stuck a on single match.
                         *
                         *
                         * Actual example (observed when my transmitter was turned off):
                         *
                         * First match:
                         * 15 124 224 3 31 248 192 7 62 240 129 15 124 12 0 15 224 3 31 44 194 199 10 86 128
                         * ^Found header                          Real end^ ^Real header                 ^End mismatch
                         *                                      ^
                         *                                      |
                         *                          (this becomes new header)
                         * Next match:
                         * 15 124 12 0 15 224 3 31 44 194 199 10 86 128 15 124 224 3 31 248 192 7 62 240 129
                         *   Real end^ ^Real header
                         * You can see the decoder grabbed the next '15'.
                         * In the next step it would grab the next(er) '15' which would end up being the actual header
                         * and decoding would succeed.
                         */
                        _state = State::HEADER_SKIP;
                    }

                    _packetPos = 0;
                }
                break;
        }
    }

    if (hadDesyncOut)
        *hadDesyncOut = hadDesync;

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
    return sbus_decode(_packetBuf, &_lastPacket);
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
    return SBUS_OK;
}
