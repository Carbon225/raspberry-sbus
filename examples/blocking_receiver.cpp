#include <iostream>
#include <chrono>
#include "SBUS.h"

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;
using std::chrono::steady_clock;
using std::chrono::milliseconds;

static SBUS sbus;

static void printPacket(const sbus_packet_t &packet)
{
    static auto lastPrint = steady_clock::now();
    auto now = steady_clock::now();

    if (now - lastPrint > milliseconds(500))
    {
        for (int i = 0; i < 16; ++i)
            cout << "ch" << i + 1 << ": " << packet.channels[i] << "\t";

        cout << "ch17: " << (packet.ch17 ? "true" : "false") << "\t"
             << "ch18: " << (packet.ch18 ? "true" : "false");

        if (packet.frameLost)
            cout << "\tFrame lost";

        if (packet.failsafe)
            cout << "\tFailsafe active";

        cout << endl;

        lastPrint = now;
    }
}

int main(int argc, char **argv)
{
    cout << "SBUS blocking receiver example" << endl;

    string ttyPath;

    if (argc > 1)
        ttyPath = argv[argc - 1];
    else
    {
        cout << "Enter tty path: ";
        cin >> ttyPath;
    }

    sbus_err_t err = sbus.install(ttyPath.c_str(), true);  // true for blocking mode
    if (err != SBUS_OK)
    {
        cerr << "SBUS install error: " << err << endl;
        return err;
    }

    cout << "SBUS installed" << endl;

    // blocks until data is available
    while ((err = sbus.read()) != SBUS_FAIL)
    {
        // desync means a packet was misaligned and not received properly
        if (err == SBUS_ERR_DESYNC)
        {
            cerr << "SBUS desync" << endl;
        }

        // true if last read received a packet
        if (sbus.gotPacket())
        {
            // A read might actually find multiple packets.
            // However, since it all happened within a single read() call
            // we should only care about the most recent packet.
            // To get a packet just check if the last read()
            // got a packet and then get that packet with lastPacket()

            // this reference is only valid until the next read()!
            // do sbus_packet_t packet = sbus.lastPacket()
            // if you want to use the value later
            const sbus_packet_t &packet = sbus.lastPacket();
            printPacket(packet);
        }
    }

    cerr << "SBUS error: " << err << endl;

    return err;
}
