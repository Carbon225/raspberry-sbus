#ifndef RPISBUS_SBUS_THREAD
#define RPISBUS_SBUS_THREAD

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "SBUS.h"

class SBUSThread
{
public:
    SBUSThread();

    sbus_err_t start(const char *tty);

    sbus_err_t stop();

    sbus_packet_t getLastPacket();

    sbus_packet_t getNextPacket();

private:
    void onPacket(const sbus_packet_t &packet);

    SBUS _sbus;
    std::atomic_flag _run = ATOMIC_FLAG_INIT;
    std::thread _thread;

    sbus_packet_t _lastPacket;
    std::mutex _lastPacketMtx;
    std::mutex _gotPacketMtx;
    std::condition_variable _gotPacket;
};

#endif // RPISBUS_SBUS_THREAD
