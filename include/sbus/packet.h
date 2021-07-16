#ifndef RPISBUS_PACKET_H
#define RPISBUS_PACKET_H


struct sbus_packet_t
{
    uint16_t channels[16];
    bool ch17, ch18;
    bool failsafe;
    bool frameLost;
};

typedef void (*sbus_packet_cb)(sbus_packet_t);


#endif
