/*
 * Use SBUSThread to automatically manage
 * a receiver thread in the background.
 * The example uses one thread to print packets every 500ms
 * and another to measure packet frequency.
 * SBUSThread is thread-safe, so you can call start() once
 * and then get packets from wherever you want.
 */

#include <iostream>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <csignal>

#include "sbus/SBUSThread.h"

using namespace std;
using namespace chrono;

static SBUSThread sbus;
static mutex consoleMtx;  // locks printing to console
static condition_variable shouldStop;  // blocks main until SIGINT
static atomic_flag run = ATOMIC_FLAG_INIT;  // helper threads run until run.clear()

void sigHandler(int)
{
    // stop helper threads and signal main to stop sbus
    run.clear();
    shouldStop.notify_all();
}

static void printPacket(const sbus_packet_t &packet)
{
    unique_lock<mutex> lck(consoleMtx);

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

static void printPacketsTask()
{
    while (run.test_and_set())
    {
        printPacket(sbus.getLastPacket());
        this_thread::sleep_for(milliseconds(500));
    }
    // have to clear because we set before while testing
    // otherwise other threads would keep running
    run.clear();
}

static void printPacketFrequencyTask()
{
    steady_clock::time_point lastPrint;
    steady_clock::time_point lastPacket;
    double freq = 0.;

    while (run.test_and_set())
    {
        // waits for next packet
        sbus.getNextPacket();

        auto now = steady_clock::now();
        auto dt = now - lastPacket;
        double freqNow =
                static_cast<double>(duration_cast<decltype(dt)>(seconds(1)).count())
                / static_cast<double>(dt.count());

        // IIR filter
        freq = 0.7 * freq + 0.3 * freqNow;

        if (now - lastPrint > milliseconds(500))
        {
            unique_lock<mutex> lck(consoleMtx);
            cout << "Packet frequency: " << freq << " Hz" << endl;
            lastPrint = now;
        }

        lastPacket = now;
    }
    // have to clear because we set before while testing
    // otherwise other threads would keep running
    run.clear();
}

int main(int argc, char **argv)
{
    cout << "SBUS thread example" << endl;

    signal(SIGINT, sigHandler);
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

    // prepare run flag
    run.test_and_set();

    thread printerThread(printPacketsTask);
    thread printFrequencyThread(printPacketFrequencyTask);

    // wait for SIGINT
    mutex stopMtx;
    unique_lock<mutex> stopLck(stopMtx);
    shouldStop.wait(stopLck);

    cout << "Stopping" << endl;

    // stopping unblocks threads waiting for getNextPacket()
    sbus.stop();

    if (printerThread.joinable())
        printerThread.join();

    if (printFrequencyThread.joinable())
        printFrequencyThread.join();

    return 0;
}
