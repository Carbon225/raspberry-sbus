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

static void onPacket(const sbus_packet_t &packet)
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
        ttyPath = argv[1];
    else
    {
        cout << "Enter tty path: ";
        cin >> ttyPath;
    }

    sbus.onPacket(onPacket);

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
    }

    cerr << "SBUS error: " << err << endl;

    return err;
}
