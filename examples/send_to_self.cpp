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
    cout << "SBUS send to self non-blocking example" << endl;

    string ttyPath;

    if (argc > 1)
        ttyPath = argv[1];
    else
    {
        cout << "Enter tty path: ";
        cin >> ttyPath;
    }

    sbus.onPacket(onPacket);

    sbus_err_t err = sbus.install(ttyPath.c_str(), false);  // false for non-blocking
    if (err != SBUS_OK)
    {
        cerr << "SBUS install error: " << err << endl;
        return err;
    }

    cout << "SBUS installed" << endl;

    // non-blocking mode, read() will check if any data is available and return immediately
    while ((err = sbus.read()) != SBUS_FAIL)
    {
        // desync means a packet was misaligned and not received properly
        if (err == SBUS_ERR_DESYNC)
        {
            cerr << "SBUS desync" << endl;
        }

        static auto lastWrite = steady_clock::now();
        auto now = steady_clock::now();

        /*
         * Receiving happens independently so we can do other things.
         * Here we send a packet every second.
         */
        if (now - lastWrite > milliseconds(1000))
        {
            lastWrite = now;

            sbus_packet_t packet = {
                    .ch17 = true,
                    .ch18 = false,
                    .failsafe = true,
                    .frameLost = false,
            };

            for (int i = 0; i < 16; i++)
            {
                packet.channels[i] = i + 1;
            }

            // make sure to limit sending frequency, SBUS is not that fast
            sbus.write(packet);
        }
    }

    cerr << "SBUS error: " << err << endl;

    return err;
}
