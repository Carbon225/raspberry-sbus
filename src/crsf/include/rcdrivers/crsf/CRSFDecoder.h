#ifndef RCDRIVERS_CRSF_DECODER_H
#define RCDRIVERS_CRSF_DECODER_H

#include <cstdint>
#include <functional>
#include "rcdrivers/errors.h"
#include "rcdrivers/crsf/crsf_spec.h"

typedef std::function<void(const crsf_packet_t&)> crsf_packet_cb;

class CRSFDecoder
{
public:
    CRSFDecoder();

    rcdrivers_err_t feed(const uint8_t buf[], int bufSize, bool *hadDesyncOut);

    rcdrivers_err_t onPacket(crsf_packet_cb cb);

    const crsf_packet_t& lastPacket() const;

    static rcdrivers_err_t decode(const uint8_t buf[], crsf_packet_t *packet);

    static rcdrivers_err_t encode(uint8_t buf[], const crsf_packet_t *packet);

private:
    enum class State
    {
        WAIT_FOR_HEADER,
        PACKET,
    } _state{State::WAIT_FOR_HEADER};

    size_t _parserConsumed{0};
    uint8_t _packetBuf[300];

    static constexpr size_t RINGBUF_CAP = 1024;
    static_assert(RINGBUF_CAP && ((RINGBUF_CAP & (RINGBUF_CAP - 1)) == 0),
                  "RINGBUF_CAP must be a power of two");
    static constexpr size_t RINGBUF_MASK = RINGBUF_CAP - 1;

    uint8_t _ringbuf[RINGBUF_CAP];
    size_t _ringbufHead{0};
    size_t _ringbufTail{0};

    size_t ringbufSize() const
    {
        return _ringbufHead - _ringbufTail;
    }

    size_t ringbufFree() const
    {
        return RINGBUF_CAP - ringbufSize();
    }

    void ringbufPush(uint8_t data)
    {
        _ringbuf[_ringbufHead & RINGBUF_MASK] = data;
        _ringbufHead++;
    }

    uint8_t ringbufPeek(size_t index) const
    {
        return _ringbuf[(_ringbufTail + index) & RINGBUF_MASK];
    }

    bool ringbufIsEmpty() const
    {
        return _ringbufHead == _ringbufTail;
    }

    crsf_packet_t _lastPacket{};
    crsf_packet_cb _packetCb{nullptr};

    bool packetReceivedWhole();
    rcdrivers_err_t verifyPacket();
    rcdrivers_err_t decodePacket();
    bool notifyCallback();
};

#endif // RCDRIVERS_CRSF_DECODER_H
