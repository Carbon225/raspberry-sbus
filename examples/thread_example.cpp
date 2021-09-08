#include <iostream>
#include <chrono>
#include <csignal>
#include "sbus/SBUSThread.h"

using namespace std;
using chrono::milliseconds;

static SBUSThread sbus;

static volatile sig_atomic_t run = 1;

void sigHandler(int)
{
    run = 0;
}

static void printPacket(const sbus_packet_t &packet)
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
}

int main(int argc, char **argv)
{
    cout << "SBUS thread example" << endl;

    string ttyPath;

    if (argc > 1)
        ttyPath = argv[1];
    else
    {
        cout << "Enter tty path: ";
        cin >> ttyPath;
    }

    sbus_err_t err = sbus.start(ttyPath.c_str());
    if (err != SBUS_OK)
    {
        cerr << "SBUS install error: " << err << endl;
        return err;
    }

    cout << "SBUS thread started" << endl;

    signal(SIGINT, sigHandler);

    while (run)
    {
        printPacket(sbus.getLastPacket());
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    cout << "Stopping" << endl;

    sbus.stop();

    return 0;
}
