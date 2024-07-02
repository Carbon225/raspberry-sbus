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
        HEADER_SKIP,
    } _state;

    uint8_t _packetBuf[CRSF_MAX_PACKET_SIZE];
    int _packetPos;

    crsf_packet_t _lastPacket;
    crsf_packet_cb _packetCb;

    bool packetReceivedWhole();
    rcdrivers_err_t verifyPacket();
    rcdrivers_err_t decodePacket();
    bool notifyCallback();
};

#endif // RCDRIVERS_CRSF_DECODER_H
