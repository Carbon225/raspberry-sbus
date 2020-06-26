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

typedef void (*sbus_packet_cb)(sbus_packet_t);

class SBUS
{
public:
    SBUS();
    ~SBUS();

    sbus_err_t install(const char *path);
    uint16_t channel(int num) const;
    sbus_err_t onPacket(sbus_packet_cb cb);
    sbus_err_t read();
    sbus_err_t write(sbus_packet_t packet);

private:
    uint16_t _channels[16] = { 0 };
    uint8_t _packet[SBUS_PACKET_SIZE * 2] = { 0 };
    int _packetPos = 0;
    int _fd = -1;
    int _desync;
    sbus_packet_cb _packetCb = nullptr;
};


#endif //RPISBUS_SBUS_H
