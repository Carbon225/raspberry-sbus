# Raspberry Pi/Linux SBUS Driver
C++ SBUS library working on the Raspberry Pi and possibly any linux system with a serial port.
To use the built-in UART on the RPi 3 make sure to use the PL011 (ttyAMA0) as the mini UART does not support parity.
It is also possible to use a simple USB-Serial converter (expect latency with FTDI adapters, fix coming soon).

## Features:
- Non-blocking and blocking modes
- Send & Receive
- All channels from 1 - 16
- Channels 17 and 18 (whatever they are for)
- Failsafe and frame lost bits
- Automatic recovery from hardware failures like broken wiring

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
### Receive
- Define packet callback `void packetCallback(sbus_packet_t packet) {/* handle packet */}`
- Set packet callback with `sbus.onPacket(packetCallback)`
- Call `sbus.read()` as often as possible to process buffered data from the serial port (non-blocking) or at least once per packet (blocking mode).
In blocking mode `read` will block and wait for data to arrive while non-blocking mode only checks if any data is available and returns immediately.
### Send
- Create `sbus_packet_t myPacket` object and populate its fields
- `sbus.write(myPacket)` to send an SBUS packet

Look at examples folder for more.
