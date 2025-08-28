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
    uint8_t _packetBuf[256];

    template <size_t CAPACITY>
    class RingBuffer
    {
        static_assert(CAPACITY && ((CAPACITY & (CAPACITY - 1)) == 0),
                      "CAPACITY must be a power of two");
        static constexpr size_t MASK = CAPACITY - 1;
    public:
        size_t size() const
        {
            return _head - _tail;
        }

        size_t free() const
        {
            return CAPACITY - size();
        }

        void push(uint8_t data)
        {
            _buf[_head & MASK] = data;
            _head++;
        }

        uint8_t peek(size_t index) const
        {
            return _buf[(_tail + index) & MASK];
        }

        void discard(size_t count)
        {
            _tail += count;
        }

        bool isEmpty() const
        {
            return _head == _tail;
        }

    private:
        uint8_t _buf[CAPACITY];
        size_t _head{0};
        size_t _tail{0};
    };

    RingBuffer<1024> _recvBuf;

    crsf_packet_t _lastPacket{};
    crsf_packet_cb _packetCb{nullptr};

    bool packetReceivedWhole();
    rcdrivers_err_t verifyPacket();
    rcdrivers_err_t decodePacket();
    bool notifyCallback();
};

#endif // RCDRIVERS_CRSF_DECODER_H
