# Raspberry Pi/Linux SBUS Driver
C++ SBUS library working on the Raspberry Pi and possibly any linux system with a serial port.

To use the built-in UART on the raspberry make sure to use the PL011 (e.g. ttyAMA0) as the mini UART does not support parity.
By default the first PL011 is connected to bluetooth so you will have to either disable it and set the PL011 as the primary UART or enable additional UARTs (only on RPi 4).
Refer to https://www.raspberrypi.org/documentation/configuration/uart.md.

It is also possible to use a simple USB-Serial converter.
For FTDI adapters use `setLowLatencyMode(true)`.

Also, don't forget to use an inverter to invert the SBUS signal!
Something like this works well. I use 10k resistors.
https://electronicspost.com/explain-the-logic-not-gate-or-inverter-and-its-operation-with-truth-table/

## Features:
- Non-blocking and blocking modes
- Send & Receive
- All channels from 1 - 16
- Channels 17 and 18 (whatever they are for)
- Failsafe and frame lost bits
- Automatic recovery from hardware failures like broken wiring
- FTDI low latency support

SBUS protocol specification and original decoding function: https://github.com/bolderflight/SBUS

## Installation
- Create a CMake project
- Clone this repo somewhere into your project
- Add `add_subdirectory(path/to/raspberry-sbus)` to your CMakeLists.txt
- Link the library to your targets with `target_link_libraries(your_target PUBLIC libsbus)`.

Example CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.16)
project(my-sbus-project)

add_subdirectory(raspberry-sbus) # relative path to the cloned repo

add_executable(main main.cpp)
target_link_libraries(main PUBLIC libsbus)
```

## Blocking vs. Non-blocking
In blocking mode the `read` function blocks until some data is available.
This mode is best used when your code contains a main loop that does not need to process anything when there are no packets.
You can also create a separate thread for reading.

In non-blocking mode `read` processes only available bytes (or nothing if none are available) and returns immediately.
You have to call `read` as often as possible to make sure you don't skip any bytes.
The most common use case is when your main loop does other things and only processes SBUS packets when one arrives.

## Usage:
### Setup
- `#include <SBUS.h>`
- Create `SBUS sbus` object
- `sbus.install("/path/to/tty", blocking_mode)` to init the serial port
- `sbus.setLowLatencyMode(true)` if you have an FTDI adapter
### Receive
- Define packet callback `void packetCallback(sbus_packet_t packet) {/* handle packet */}`
- Set packet callback with `sbus.onPacket(packetCallback)`
- Call `sbus.read()` as often as possible to process buffered data from the serial port (non-blocking) or at least once per packet (blocking mode).
In blocking mode `read` will block and wait for data to arrive while non-blocking mode only checks if any data is available and returns immediately.
### Send
- Create `sbus_packet_t myPacket` object and populate its fields
- `sbus.write(myPacket)` to send an SBUS packet

Look at examples folder for more.

## Low latency mode
FTDI adapters have weird buffering that makes packets send in batches and not right after calling `write()`.
Enabling low latency mode fixes this by doing some magic even I don't understand.
Credit goes to https://github.com/projectgus/hairless-midiserial/blob/add59f04c3b75044f3033f70d5523685b6b9dd0a/src/PortLatency_linux.cpp.
