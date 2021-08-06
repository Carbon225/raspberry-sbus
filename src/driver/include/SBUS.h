#ifndef RPISBUS_SBUS_H
#define RPISBUS_SBUS_H

#include <cstdint>
#include "sbus/sbus_error.h"
#include "sbus/DecoderFSM.h"

class SBUS
{
public:
    SBUS() noexcept;

    virtual ~SBUS() noexcept;

    /// Configure a tty for SBUS.
    /// \param path tty path e.g. "/dev/ttyUSB0"
    /// \param blocking If true, read() will block, else it will return immediately
    /// \param timeout Timeout in deciseconds (10 is 1 second) for read() (only if blocking=true)
    /// \return Error code or SBUS_OK
    sbus_err_t install(const char path[], bool blocking, uint8_t timeout = 0);

    /// Close the opened tty.
    /// \return Error code or SBUS_OK (closing a closed tty also gives SBUS_OK)
    sbus_err_t uninstall();

    /// Enable "low latency mode" which fixes performance on FTDI USB adapters.
    /// Called after install().
    /// \param enable True to enable and false to disable low latency mode
    /// \return Error code or SBUS_OK
    sbus_err_t setLowLatencyMode(bool enable);

    /// Set function to be called when a packet is received.
    /// \param cb Pointer to a function with signature void (sbus_packet_t)
    /// \return Error code or SBUS_OK
    sbus_err_t onPacket(sbus_packet_cb cb);

    /// Call to process buffered data.
    /// Called after install().
    /// Has to be called frequently to receive packets.
    /// \return SBUS_ERR_DESYNC signaling a bad packet (not fatal), other error code or SBUS_OK
    sbus_err_t read();

    /// Send a packet.
    /// Called after install().
    /// \param packet The packet to send
    /// \return Error code or SBUS_OK
    sbus_err_t write(const sbus_packet_t &packet);

    /// Get last known value of a channel.
    /// \param num Channel number 0 to 15
    /// \return Value of the channel or 0 if given channel number was invalid
    uint16_t channel(int num) const;

    /// Get last received packet.
    /// \return Reference to last received packet
    const sbus_packet_t& lastPacket() const;

private:
    static constexpr int READ_BUF_SIZE = SBUS_PACKET_SIZE * 10;

    int _fd;
    DecoderFSM _decoder;
    uint8_t _readBuf[READ_BUF_SIZE];
    uint8_t _writeBuf[SBUS_PACKET_SIZE];
};


#endif //RPISBUS_SBUS_H
