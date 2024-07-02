#ifndef RPISBUS_DECODER_FSM_H
#define RPISBUS_DECODER_FSM_H

#include <cstdint>
#include "rcdrivers/errors.h"
#include "rcdrivers/sbus/sbus_packet.h"

typedef void (*sbus_packet_cb)(const sbus_packet_t&);

class DecoderFSM
{
public:
    DecoderFSM();

    rcdriver_err_t feed(const uint8_t buf[], int bufSize, bool *hadDesyncOut);

    rcdriver_err_t onPacket(sbus_packet_cb cb);

    const sbus_packet_t& lastPacket() const;

private:
    enum class State
    {
        WAIT_FOR_HEADER,
        PACKET,
        HEADER_SKIP,
    } _state;

    uint8_t _packetBuf[SBUS_PACKET_SIZE];
    int _packetPos;

    sbus_packet_t _lastPacket;
    sbus_packet_cb _packetCb;

    rcdriver_err_t verifyPacket();
    rcdriver_err_t decodePacket();
    bool notifyCallback();
};

#endif
