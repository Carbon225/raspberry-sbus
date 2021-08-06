#include <iostream>
#include <chrono>
#include <thread>

#include "SBUS.h"

using namespace std;
using namespace chrono_literals;

static SBUS sbus;

int main(int argc, char **argv)
{
    cout << "SBUS simple send example" << endl;

    string ttyPath;

    if (argc > 1)
    {
        ttyPath = argv[1];
    }
    else
    {
        cout << "Enter tty path: ";
        cin >> ttyPath;
    }

    sbus_err_t err = sbus.install(ttyPath.c_str(), true);
    if (err != SBUS_OK)
    {
        cerr << "SBUS install error: " << err << endl;
        return err;
    }

    cout << "SBUS installed" << endl;

    for (;;)
    {
        auto now = chrono::steady_clock::now();

        sbus_packet_t packet = {
                .ch17 = true,
                .ch18 = false,
                .failsafe = true,
                .frameLost = false,
        };

        for (int i = 0; i < 16; i++)
        {
            // channel value will be channel number
            packet.channels[i] = i + 1;
        }

        err = sbus.write(packet);
        if (err != SBUS_OK)
        {
            cerr << "SBUS write error: " << err << endl;
            return err;
        }

        // make sure to limit sending frequency
        // My transmitter does 100Hz
        this_thread::sleep_until(now + 10ms);
    }

    return 0;
}
