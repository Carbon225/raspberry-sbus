#include <iostream>
#include <chrono>
#include <thread>

#include "SBUS.h"

using namespace std;
using namespace chrono_literals;

static SBUS sbus;

static void onPacket(const sbus_packet_t &packet)
{
    static auto lastPrint = chrono::steady_clock::now();
    auto now = chrono::steady_clock::now();

    if (now - lastPrint > 500ms)
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

static void sending_task()
{
    sbus_err_t err;
    int counter = 0;
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
            packet.channels[i] = i + counter;
        counter++;

        err = sbus.write(packet);
        if (err != SBUS_OK)
            break;

        this_thread::sleep_until(now + 10ms);
    }
    cerr << "SBUS write error: " << err << endl;
}

static void receiving_task()
{
    sbus_err_t err;
    while ((err = sbus.read()) != SBUS_FAIL)
        if (err == SBUS_ERR_DESYNC)
            cerr << "SBUS desync" << endl;
    cerr << "SBUS read error: " << err << endl;
}

int main(int argc, char **argv)
{
    cout << "SBUS threads example" << endl;

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

    // You should really know what you're doing if using threads.
    // I don't think a mutex is necessary in this case (send from one, receive from other)
    // but you definitely need one if sending from multiple threads.
    thread receiving_thread(receiving_task);
    thread sending_thread(sending_task);

    cout << "Threads running" << endl;

    receiving_thread.join();
    sending_thread.join();

    return 0;
}
