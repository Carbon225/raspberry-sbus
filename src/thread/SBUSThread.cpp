#include "sbus/SBUSThread.h"

SBUSThread::SBUSThread()
{
    _sbus.onPacket([this](const sbus_packet_t &p){
        onPacket(p);
    });
}

void SBUSThread::onPacket(const sbus_packet_t &packet)
{

}

sbus_err_t SBUSThread::start(const char *tty)
{
    sbus_err_t err = _sbus.install(tty, true, 5);
    if (err != SBUS_OK)
        return err;

    _run.test_and_set();

    _thread = std::thread([this]{
        while(_run.test_and_set()) _sbus.read();
    });

    return SBUS_OK;
}

sbus_err_t SBUSThread::stop()
{
    if (_thread.joinable())
    {
        _run.clear();
        _thread.join();
    }
    return _sbus.uninstall();
}
