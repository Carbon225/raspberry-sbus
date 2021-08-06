# Raspberry Pi/Linux SBUS Driver
C++ SBUS library working on the Raspberry Pi and possibly any linux system with a serial port.

You can use the built-in UART on the pi or a USB-Serial adapter.
For FTDI adapters use `setLowLatencyMode(true)`.

Also, don't forget to use an inverter to invert the SBUS signal!
Something like [this](https://electronicspost.com/explain-the-logic-not-gate-or-inverter-and-its-operation-with-truth-table/) works well. I use 10k resistors.

Most receivers use 5V so be careful when plugging directly into the GPIOs on the pi and use a level converter.

## Features:
- Non-blocking and blocking modes
- Send & Receive
- All channels from 1 - 16
- Binary channels 17 and 18
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
### Raspberry Pi UART setup
#### To use built-in UART on Raspberry Pi 3/4
- `sudo systemctl disable hciuart` - disable bluetooth as we will steal its UART
- add `dtoverlay=disable-bt` to `/boot/config.txt`
- reboot and use `/dev/ttyAMA0`
#### To use additional UARTs only on Raspberry Pi 4
- add `dtoverlay=uartX` to `/boot/config.txt` where X is 2, 3, 4 or 5 to enable another UART
- reboot and use `/dev/ttyAMAX` where X is the UART you chose

Look at https://www.raspberrypi.org/documentation/configuration/uart.md for more info.

### The Code
#### Setup
- `#include <SBUS.h>`
- Create `SBUS sbus` object
- `sbus.install("/path/to/tty", blocking_mode)` to init the serial port
- `sbus.setLowLatencyMode(true)` if you have an FTDI adapter
#### Receive
- Define packet callback `void packetCallback(const sbus_packet_t &packet) {/* handle packet */}`
- Set packet callback with `sbus.onPacket(packetCallback)`
- Call `sbus.read()` as often as possible to process buffered data from the serial port (non-blocking) or at least once per packet (blocking mode).
In blocking mode `read` will block and wait for data to arrive while non-blocking mode only checks if any data is available and returns immediately.
#### Send
- Create `sbus_packet_t myPacket` object and populate its fields
- `sbus.write(myPacket)` to send an SBUS packet

Look at [examples](https://github.com/Carbon225/raspberry-sbus/tree/master/examples) folder for more.

## Low latency mode
FTDI adapters have weird buffering that makes packets send in batches and not right after calling `write()`.
Enabling low latency mode fixes this by doing some magic even I don't understand.
Credit goes to [https://github.com/projectgus/hairless-midiserial](https://github.com/projectgus/hairless-midiserial/blob/add59f04c3b75044f3033f70d5523685b6b9dd0a/src/PortLatency_linux.cpp).

Note: only supported on linux.
