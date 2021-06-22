#ifndef RPISBUS_SBUS_H
#define RPISBUS_SBUS_H

#include <cstdint>
#include "sbus_driver.h"

struct sbus_packet_t
{
    uint16_t *channels;
    bool ch17, ch18;
    bool failsafe;
    bool frameLost;
};

struct sbus_telemetry_t
{
    uint8_t type;
    uint16_t data;
};

typedef void (*sbus_packet_cb)(sbus_packet_t);
typedef void (*sbus_telemetry_cb)(sbus_telemetry_t);

class SBUS
{
public:
    SBUS();
    ~SBUS();

    sbus_err_t install(const char *path, bool blocking, uint8_t timeout = 0);
    uint16_t channel(int num) const;
    sbus_err_t onPacket(sbus_packet_cb cb);
    sbus_err_t onTelemetry(sbus_telemetry_cb cb);
    sbus_err_t read();
    sbus_err_t write(sbus_packet_t packet);

private:
    enum class State
    {
        WAIT_FOR_HEADER,
        PACKET,
        TELEMETRY_TYPE,
        TELEMETRY_DATA,
    } _state = State::WAIT_FOR_HEADER;

    uint8_t _telemetryType;
    uint8_t _telemetryData[SBUS_TELEMETRY_BYTES];
    int _telemetryPos = 0;
    int _receivedTelemetrySlots = 0;

    uint8_t _packet[SBUS_PACKET_SIZE] = { 0 };
    int _packetPos = 0;
    int _nextRead;
    int _fd = -1;

    uint16_t _channels[16] = { 0 };
    sbus_packet_cb _packetCb = nullptr;
    sbus_telemetry_cb _telemetryCb = nullptr;

    static bool verifyPacket(const uint8_t packet[]);
    static bool isTelemetrySlot(uint8_t byte);
    void decodePacket();
    void handleTelemetry();
};


#endif //RPISBUS_SBUS_H
