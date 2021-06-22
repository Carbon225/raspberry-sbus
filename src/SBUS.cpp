#include "SBUS.h"
#include <cstdio>
#include <termios.h>

#define SBUS_DESYNC_END 2
#define SBUS_DESYNC_HDR 1
#define READ_BUF_SIZE (SBUS_PACKET_SIZE * 2)

SBUS::SBUS()
    : _nextRead(READ_BUF_SIZE)
{}

SBUS::~SBUS()
{
    sbus_uninstall(&_fd);
}

sbus_err_t SBUS::install(const char *path, bool blocking, uint8_t timeout)
{
    return sbus_install(&_fd, path, blocking, timeout);
}

uint16_t SBUS::channel(int num) const
{
    if (num >= 0 & num < 16)
        return _channels[num];
    else
        return 0;
}

sbus_err_t SBUS::onPacket(sbus_packet_cb cb)
{
    _packetCb = cb;
    return SBUS_OK;
}

sbus_err_t SBUS::onTelemetry(sbus_telemetry_cb cb)
{
    _telemetryCb = cb;
    return SBUS_OK;
}

sbus_err_t SBUS::read()
{
    uint8_t readBuf[READ_BUF_SIZE] = {0};
    int nRead = sbus_read(&_fd, readBuf, _nextRead);

    if (nRead <= 0)
        return SBUS_OK;

    bool hadDesync = false;

    for (int i = 0; i < nRead; i++)
    {
        switch (_state)
        {
            case State::WAIT_FOR_HEADER:
                if (readBuf[i] == SBUS_HEADER)
                {
                    _packet[0] = SBUS_HEADER;
                    _packetPos = 1;
                    _state = State::PACKET;
                }
                break;

            case State::PACKET:
                _packet[_packetPos] = readBuf[i];
                _packetPos++;
                if (_packetPos >= SBUS_PACKET_SIZE)
                {
                    if (verifyPacket(_packet))
                    {
                        decodePacket();

                        if (isTelemetrySlot(readBuf[i]))
                        {
                            // expect telemetry data
                            _state = State::TELEMETRY_TYPE;
                            _receivedTelemetrySlots = 0;
                        }
                        else
                        {
                            _state = State::WAIT_FOR_HEADER;
                        }
                    }
                    else
                    {
                        hadDesync = true;
                        _state = State::WAIT_FOR_HEADER;
                    }
                }
                break;

            // waiting for telemetry type for time slot
            case State::TELEMETRY_TYPE:
                switch (readBuf[i])
                {
                    case SBUS_HEADER:
                        // no telemetry received
                        _packet[0] = SBUS_HEADER;
                        _packetPos = 1;
                        _state = State::PACKET;
                        break;

                    default:
                        // assume what we got was a sensor type
                        _telemetryType = readBuf[i];
                        _telemetryPos = 0;
                        _state = State::TELEMETRY_DATA;
                        // begin waiting for data
                        break;
                }
                break;

            case State::TELEMETRY_DATA:
                _telemetryData[_telemetryPos] = readBuf[i];
                _telemetryPos++;
                if (_telemetryPos >= SBUS_TELEMETRY_BYTES)
                {
                    _receivedTelemetrySlots++;

                    // make sure we do not get stuck reading incorrect telemetry
                    if (_receivedTelemetrySlots >= 9)
                    {
                        // looks like we desynchronized
                        hadDesync = true;
                        _state = State::WAIT_FOR_HEADER;
                    }
                    else
                    {
                        handleTelemetry();

                        // try reading more sensor slots
                        _state = State::TELEMETRY_TYPE;
                    }
                }
                break;
        }
    }

    if (_state == State::PACKET)
    {
        _nextRead = SBUS_PACKET_SIZE - _packetPos;
    }
    else
    {
        _nextRead = READ_BUF_SIZE;
    }

    if (hadDesync)
        tcflush(_fd, TCIFLUSH);

    return hadDesync ? SBUS_ERR_DESYNC : SBUS_OK;
}

sbus_err_t SBUS::write(sbus_packet_t packet)
{
    uint8_t opt = 0;
    opt |= packet.ch17 * SBUS_OPT_C17;
    opt |= packet.ch18 * SBUS_OPT_C18;
    opt |= packet.failsafe * SBUS_OPT_FS;
    opt |= packet.frameLost * SBUS_OPT_FL;
    sbus_write(&_fd, packet.channels, opt);
    return SBUS_OK;
}

bool SBUS::verifyPacket(const uint8_t packet[])
{
    const uint8_t first = packet[0];
    const uint8_t last = packet[SBUS_PACKET_SIZE - 1];

    return (first == SBUS_HEADER &&
            (last == SBUS_END || isTelemetrySlot(last)));
}

bool SBUS::isTelemetrySlot(uint8_t byte)
{
    switch (byte)
    {
        case SBUS_TELEMETRY_SLOTS_0_7:
        case SBUS_TELEMETRY_SLOTS_8_15:
        case SBUS_TELEMETRY_SLOTS_16_23:
        case SBUS_TELEMETRY_SLOTS_24_31:
            return true;

        default:
            return false;
    }
}

void SBUS::decodePacket()
{
    uint8_t opt = 0;
    sbus_err_t ret = sbus_decode(_packet, _channels, &opt);
    if (ret == SBUS_OK && _packetCb != nullptr)
    {
        sbus_packet_t packet = {
                _channels,
                (bool) (opt & SBUS_OPT_C17),
                (bool) (opt & SBUS_OPT_C18),
                (bool) (opt & SBUS_OPT_FS),
                (bool) (opt & SBUS_OPT_FL)
        };
        _packetCb(packet);
    }
}

void SBUS::handleTelemetry()
{
    if (_telemetryCb)
    {
        sbus_telemetry_t data;
        data.type = _telemetryType;

        // might be wrong endian
        data.data = _telemetryData[1];
        data.data <<= 8;
        data.data |= _telemetryData[0];

        _telemetryCb(data);
    }
}
