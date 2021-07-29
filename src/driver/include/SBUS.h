#ifndef RPISBUS_SBUS_H
#define RPISBUS_SBUS_H

#include <cstdint>
#include "sbus/sbus_defs.h"
#include "sbus/DecoderFSM.h"

class SBUS
{
public:
    SBUS() noexcept;

    virtual ~SBUS() noexcept;

    sbus_err_t install(const char path[], bool blocking, uint8_t timeout = 0);

    sbus_err_t uninstall();

    sbus_err_t setLowLatencyMode(bool enable);

    sbus_err_t onPacket(sbus_packet_cb cb);

    sbus_err_t read();

    sbus_err_t write(sbus_packet_t packet);

    uint16_t channel(int num) const;

    const sbus_packet_t& lastPacket() const;

private:
    int _fd;
    DecoderFSM _decoder;
};


#endif //RPISBUS_SBUS_H