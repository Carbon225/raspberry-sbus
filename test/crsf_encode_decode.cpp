#include <iostream>
#include <cstring>
#include "rcdrivers/crsf/CRSFDecoder.h"

using namespace std;

int main()
{
    crsf_packet_t packet1;
    memset(&packet1, 0, sizeof(packet1));
    uint8_t buf[CRSF_MAX_PACKET_SIZE];

    packet1.frametype = CRSF_FRAMETYPE_RC_CHANNELS_PACKED;

    for (int i = 0; i < CRSF_NUM_RC_CHANNELS; ++i)
    {
        packet1.payload.rc_channels_packed.channels[i] = i + 1;
    }

    rcdrivers_err_t err;
    err = CRSFDecoder::encode(buf, &packet1);
    cout << "encode: " << err << endl;
    if (err != RCDRIVERS_OK) return -1;

    crsf_packet_t packet2;
    memset(&packet2, 0, sizeof(packet2));
    err = CRSFDecoder::decode(buf, &packet2);
    cout << "decode: " << err << endl;
    if (err != RCDRIVERS_OK) return -1;

    if (memcmp(&packet1, &packet2, sizeof(packet1)) != 0)
    {
        cerr << "Packets don't match" << endl;
        
        cerr << "packet1.frametype: " << packet1.frametype << endl;
        cerr << "packet2.frametype: " << packet2.frametype << endl;

        cerr << "packet1.payload.rc_channels_packed.channels: ";
        for (int i = 0; i < CRSF_NUM_RC_CHANNELS; ++i)
            cerr << (int)packet1.payload.rc_channels_packed.channels[i] << " ";
        cerr << endl;

        cerr << "packet2.payload.rc_channels_packed.channels: ";
        for (int i = 0; i < CRSF_NUM_RC_CHANNELS; ++i)
            cerr << (int)packet2.payload.rc_channels_packed.channels[i] << " ";
        cerr << endl;

        return -1;
    }

    return 0;
}
