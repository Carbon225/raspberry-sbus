# Raspberry Pi/Linux CRSF/SBUS Driver

C++ CRSF/SBUS library working on the Raspberry Pi and possibly any linux system with a serial port.
Supports both sending and receiving. Can be used for telemetry or RC control.

You can use the built-in UART on the Pi or a USB-Serial adapter.
For FTDI adapters use `setLowLatencyMode(true)`.

Also, don't forget to use an inverter to invert the SBUS signal!
Something like [this](https://electronicspost.com/explain-the-logic-not-gate-or-inverter-and-its-operation-with-truth-table/) works well. I use 10k resistors.

CRSF does not need inversion.

## Features:

- CRSF and SBUS support
- Non-blocking and blocking modes
- Send & Receive
- Full CRSF support:
  - Telemetry and RC packets
  - Send and receive custom packets
- Full SBUS support:
  - All channels from 1 - 16
  - Binary channels 17 and 18
  - Failsafe and frame lost bits
- Automatic recovery from hardware failures like broken wiring
- FTDI low latency support

SBUS protocol specification and original decoding function: https://github.com/bolderflight/SBUS

## Getting started

- Create an empty project folder
- `git clone https://github.com/Carbon225/raspberry-sbus` **inside** your project folder
- Create a `CMakeLists.txt` inside project folder
```cmake
cmake_minimum_required(VERSION 3.9)
project(my-rcdrivers-project)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# This is what you cloned previously.
# After this line an rcdrivers target is available for linking
add_subdirectory(raspberry-sbus)

# Create a new executable target with a single source file main.cpp
# and link it with the library
add_executable(main main.cpp)
target_link_libraries(main PUBLIC rcdrivers)
```
- Create `main.cpp` and paste code from one of the [examples](examples)
- Your project should look like this:
```
- project_folder/
  - CMakeLists.txt
  - raspberry-sbus/
  - main.cpp
```
- Open your project in a CMake compatible IDE (CLion, VS, ...)
- or build manually:
  - `cmake -B build -S .` <- don't forget the dot
  - `cmake --build build`
  - run with `./build/main`

## Usage:
### Raspberry Pi UART setup
#### To use built-in UART on Raspberry Pi 3/4
- `sudo systemctl disable hciuart` - disable bluetooth as we will steal its UART
- add `dtoverlay=disable-bt` to `/boot/config.txt`
- reboot and use `/dev/ttyAMA0`
#### To use additional UARTs only on Raspberry Pi 4/5
- add `dtoverlay=uartX` to `/boot/config.txt` where X is 2, 3, 4 or 5 to enable another UART
- reboot and use `/dev/ttyAMAY` where Y will be assigned sequentially for the new UART

Look at https://www.raspberrypi.org/documentation/configuration/uart.md for more info.

### The Code

Replace CRSF with SBUS in the code to use the other protocol.

#### Setup
- `#include <rcdrivers/CRSF.h>`
- Create a `CRSF crsf` object
- `crsf.install("/path/to/tty", blocking_mode)` to init the serial port
- `crsf.setLowLatencyMode(true)` if you have an FTDI adapter
#### Receive
- Define packet callback `void packetCallback(const crsf_packet_t &packet) {/* handle packet */}`
- Set packet callback with `crsf.onPacket(packetCallback)`
- Call `crsf.read()` as often as possible to process buffered data from the serial port (non-blocking) or at least once per packet (blocking mode).
In blocking mode `read` will block and wait for data to arrive while non-blocking mode only checks if any data is available and returns immediately.
#### Send
- Create a `crsf_packet_t myPacket` object and populate its fields
- `crsf.write(myPacket)` to send a packet

Look at [examples](examples) folder for more.

## Blocking vs. Non-blocking
In blocking mode the `read` function blocks until some data is available.
This mode is best used when your code contains a main loop that does not need to process anything when there are no packets.
You can also create a separate thread for reading.

In non-blocking mode `read` processes only available bytes (or nothing if none are available) and returns immediately.
You have to call `read` as often as possible to make sure you don't skip any bytes.
The most common use case is when your main loop does other things and only processes SBUS packets when one arrives.

## Low latency mode
FTDI adapters have weird buffering that makes packets send in batches and not right after calling `write()`.
Enabling low latency mode fixes this by doing some magic even I don't understand.
Credit goes to [https://github.com/projectgus/hairless-midiserial](https://github.com/projectgus/hairless-midiserial/blob/add59f04c3b75044f3033f70d5523685b6b9dd0a/src/PortLatency_linux.cpp).
