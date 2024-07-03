#include <iostream>
#include <chrono>
#include "rcdrivers/CRSF.h"

// uncomment if you have an FTDI adapter to enable low latency mode (might work on other adapters as well)
//#define FTDI_ADAPTER

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;
using std::chrono::steady_clock;
using std::chrono::milliseconds;

static CRSF crsf_radio;
static CRSF crsf_fc;

static void onPacket(const crsf_packet_t &packet)
{
    static auto lastPrint = steady_clock::now();
    auto now = steady_clock::now();

    // retransmit received packet
    crsf_fc.write(packet);

    if ((packet.frametype == CRSF_FRAMETYPE_RC_CHANNELS_PACKED) && (now - lastPrint > milliseconds(500)))
    {
        for (int i = 0; i < CRSF_NUM_RC_CHANNELS; ++i)
            cout << "ch" << i + 1 << ": " << packet.payload.rc_channels_packed.channels[i] << "\t";

        cout << endl;

        lastPrint = now;
    }
}

int main(int argc, char **argv)
{
    cout << "CRSF blocking passthrough example" << endl;

    string radioTtyPath;
    string fcTtyPath;

    if (argc == 3)
    {
        radioTtyPath = argv[1];
        fcTtyPath = argv[2];
    }
    else
    {
        cerr << "Usage: " << argv[0] << " <radio tty path> <flight controller tty path>" << endl;
        return -1;
    }

    crsf_radio.onPacket(onPacket);

    rcdrivers_err_t err = crsf_radio.install(radioTtyPath.c_str(), true);  // true for blocking mode
    if (err != RCDRIVERS_OK)
    {
        cerr << "CRSF radio install error: " << err << endl;
        return err;
    }

    err = crsf_fc.install(fcTtyPath.c_str(), true);
    if (err != RCDRIVERS_OK)
    {
        cerr << "CRSF FC install error: " << err << endl;
        return err;
    }

#ifdef FTDI_ADAPTER
    // enable only if you have weird packet timings (mostly on FTDI chips)
    err = crsf_fc.setLowLatencyMode(true);
    if (err != RCDRIVERS_OK)
    {
        cerr << "CRSF set low latency error: " << err << endl;
        return err;
    }
    cout << "Low latency mode enabled" << endl;
#endif

    cout << "CRSF installed" << endl;

    // blocks until data is available
    while ((err = crsf_radio.read()) != RCDRIVERS_FAIL)
    {
        // desync means a packet was misaligned and not received properly
        if (err == RCDRIVERS_ERR_DESYNC)
        {
            cerr << "CRSF desync" << endl;
        }
    }

    cerr << "CRSF error: " << err << endl;

    return err;
}
