# Raspberry Pi/Linux SBUS Driver
C++ SBUS library working on the Raspberry Pi and possibly any linux system with a serial port. To use the built-in UART on the Pi make sure to use the PL011 (ttyAMA0) as the mini UART does not support parity.

## Features:
- Non-blocking
- Send & Receive
- All channels from 1 - 16
- Channels 17 and 18 (whatever they are for)
- Failsafe and frame lost bits

SBUS protocol specification and original decoding function: https://github.com/bolderflight/SBUS

## Usage:
- Create <code>SBUS</code> object
- <code>SBUS::install(const char *ttyPath)</code> to init the serial port
- Set <code>void sbusCallback(sbus_packet_t)</code> callback with <code>SBUS::onPacket(sbus_packet_cb)</code>
- Call <code>SBUS::read()</code> as often as possible to poll the serial port (non-blocking)
- <code>SBUS::write(sbus_packet_t)</code> to send an SBUS packet
