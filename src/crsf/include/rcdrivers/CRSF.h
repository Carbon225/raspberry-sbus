#ifndef RCDRIVERS_CRSF_H
#define RCDRIVERS_CRSF_H

#include <cstdint>
#include "rcdrivers/errors.h"
#include "rcdrivers/crsf/CRSFDecoder.h"

class CRSF
{
public:
    CRSF() noexcept;

    virtual ~CRSF() noexcept;

    /// Configure a tty for CRSF.
    /// \param path tty path e.g. "/dev/ttyUSB0"
    /// \param blocking If true, read() will block, else it will return immediately
    /// \param timeout Timeout in deciseconds (10 is 1 second) for read() (only if blocking=true)
    /// \param baudrate Optional custom baudrate
    /// \return Error code or RCDRIVERS_OK
    rcdrivers_err_t install(const char path[], bool blocking, uint8_t timeout = 0, int baudrate = CRSF_BAUD);

    /// Close the opened tty.
    /// \return Error code or RCDRIVERS_OK (closing a closed tty also gives RCDRIVERS_OK)
    rcdrivers_err_t uninstall();

    /// Enable "low latency mode" which fixes performance on FTDI USB adapters.
    /// Called after install().
    /// \param enable True to enable and false to disable low latency mode
    /// \return Error code or RCDRIVERS_OK
    rcdrivers_err_t setLowLatencyMode(bool enable);

    /// Set function to be called when a packet is received.
    /// \param cb Function object with signature void (crsf_packet_t)
    /// \return Error code or RCDRIVERS_OK
    rcdrivers_err_t onPacket(crsf_packet_cb cb);

    /// Call to process buffered data.
    /// Called after install().
    /// Has to be called frequently to receive packets.
    /// \return RCDRIVERS_ERR_DESYNC signaling a bad packet (not fatal), other error code or RCDRIVERS_OK
    rcdrivers_err_t read();

    /// Send a packet.
    /// Called after install().
    /// \param packet The packet to send
    /// \return Error code or RCDRIVERS_OK
    rcdrivers_err_t write(const crsf_packet_t &packet);

private:
    static constexpr int READ_BUF_SIZE = CRSF_MAX_PACKET_SIZE * 10;

    int _fd;
    CRSFDecoder _decoder;
    uint8_t _readBuf[READ_BUF_SIZE];
    uint8_t _writeBuf[CRSF_MAX_PACKET_SIZE];
};


#endif //RCDRIVERS_SBUS_H
