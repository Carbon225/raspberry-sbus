#ifndef RPISBUS_SBUS_THREAD
#define RPISBUS_SBUS_THREAD

#include <atomic>
#include <thread>
#include "SBUS.h"

class SBUSThread
{
public:
    SBUSThread();

    sbus_err_t start(const char *tty);

    sbus_err_t stop();

private:
    void onPacket(const sbus_packet_t &packet);

    SBUS _sbus;
    std::atomic_flag _run = ATOMIC_FLAG_INIT;
    std::thread _thread;
};

#endif // RPISBUS_SBUS_THREAD
