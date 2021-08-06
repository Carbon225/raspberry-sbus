#include <iostream>
#include <cstring>
#include "sbus/packet_decoder.h"

using namespace std;

int main()
{
    sbus_packet_t packet1;
    uint8_t buf[SBUS_PACKET_SIZE];

    packet1.frameLost = true;
    packet1.failsafe = false;
    packet1.ch17 = true;
    packet1.ch18 = false;

    for (int i = 0; i < SBUS_NUM_CHANNELS; ++i)
        packet1.channels[i] = i + 1;

    sbus_encode(buf, &packet1);

    sbus_packet_t packet2;
    sbus_decode(buf, &packet2);

    if (memcmp(&packet1, &packet2, sizeof(packet1)) != 0)
    {
        cerr << "Packets don't match" << endl;
        return -1;
    }

    return 0;
}
