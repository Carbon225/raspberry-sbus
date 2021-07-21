#ifndef RPISBUS_DECODER_FSM_H
#define RPISBUS_DECODER_FSM_H

#include <cstdint>
#include "sbus/packet.h"
#include "sbus_driver.h"

class DecoderFSM
{
public:
    DecoderFSM();

    sbus_err_t feed(const uint8_t buf[], int n, bool *hadDesync);

    sbus_err_t onPacket(sbus_packet_cb cb);

    const sbus_packet_t& lastPacket() const;

private:
    enum class State
    {
        WAIT_FOR_HEADER,
        PACKET,
    } _state;

    uint8_t _packetBuf[SBUS_PACKET_SIZE];
    int _packetPos;

    sbus_packet_t _lastPacket;
    sbus_packet_cb _packetCb;

    sbus_err_t verifyPacket();
    sbus_err_t decodePacket();
    bool notifyCallback();
};

#endif
