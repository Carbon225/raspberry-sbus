/*
 * How to write a class using member callback function.
 * Example requires linking with pthread.
 */

#include <iostream>
#include <chrono>
#include <functional>
#include <atomic>
#include <thread>
#include "SBUS.h"

using namespace std;
using chrono::steady_clock;
using chrono::milliseconds;
using chrono::nanoseconds;

class MySBUS
{
public:
    MySBUS()
    {
        _sbus.onPacket([this](const sbus_packet_t &packet){
            // Capturing lambda to bind function to 'this'.
            // Can also use std::bind or any std::function object
            onPacket(packet);
        });
    }

    sbus_err_t start(const char *tty)
    {
        // timeout set to 5 so that the read loop can check the _run flag every 500ms
        sbus_err_t err = _sbus.install(tty, true, 5);
        if (err != SBUS_OK)
        {
            cerr << "SBUS install error: " << err << endl;
            return err;
        }

        // set the flag to 1
        _run.test_and_set();

        _thread = thread([this]{
            // will run while flag is 1
            while(_run.test_and_set())
            {
                // will return after 500ms max
                sbus_err_t err = _sbus.read();
                if (err == SBUS_ERR_DESYNC)
                    cerr << "SBUS desync" << endl;
            }
            cout << "Thread terminating" << endl;
        });

        return SBUS_OK;
    }

    void stop()
    {
        if (_thread.joinable())
        {
            // set flag to 0 which will stop the while loop
            _run.clear();
            _thread.join();
        }
        _sbus.uninstall();
    }

private:
    steady_clock::time_point _lastPrint;

    // we have a proper callback as a member function
    void onPacket(const sbus_packet_t &packet)
    {
        auto now = steady_clock::now();
        if (now - _lastPrint > milliseconds(500))
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

            _lastPrint = now;
        }
    }

    SBUS _sbus;
    thread _thread;
    atomic_flag _run = ATOMIC_FLAG_INIT;
};

int main(int argc, char **argv)
{
    cout << "SBUS class example" << endl;

    string ttyPath;

    if (argc > 1)
        ttyPath = argv[1];
    else
    {
        cout << "Enter tty path: ";
        cin >> ttyPath;
    }

    MySBUS sbus;
    sbus_err_t err = sbus.start(ttyPath.c_str());
    if (err)
        return err;

    cout << "Press any key to stop" << endl;
    getchar();

    sbus.stop();

    return 0;
}
