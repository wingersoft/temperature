# AGENTS.md

## Project Overview

This project is an **Arduino ESP32** application developed using the **PlatformIO** framework.

**ESP32-C3 Temperature Monitoring System** with DS18B20 sensor and MQTT integration. Reads temperature from GPIO0, displays temperature in Celsius on serial monitor, and publishes data to MQTT broker for remote monitoring.

### Key Features
- **Temperature Monitoring**: Continuous DS18B20 sensor readings with 1-second sampling
- **Serial Output**: Real-time temperature display and debugging information
- **MQTT Integration**: WiFi connectivity and MQTT publishing for remote monitoring
- **Connection Management**: Automatic WiFi and MQTT reconnection handling
- **Status Reporting**: Device online/offline status publishing

The project structure follows the standard PlatformIO conventions.

## Build and Test Commands

### Environment Setup

The required dependencies for this project are managed by PlatformIO. The agent should assume that the PlatformIO Core and the project's dependencies are already installed.

### Build and Upload

To build and upload the firmware to the ESP32 board, use the PlatformIO CLI.

- **Build:** `pio run`
- **Upload:** `pio run -t upload`

### Clean Build

To clean the build environment, use the following command:

- **Clean:** `pio run -t clean`

### Monitor Serial Output

To monitor the serial output from the ESP32 board, use the following command:

- **Monitor:** `pio device monitor`

## Code Style and Conventions

- **Language:** C++

- **Framework:** Arduino

- **Platform:** PlatformIO

- **Coding Practices:**

  - Use `camelCase` for variables and functions.

  - Use `PascalCase` for class names.

  - Document public functions with comments.

  - Avoid using `using namespace std;` in header files.

  - add to platformio.ini

    build_flags =
        -D ARDUINO_USB_MODE=1
        -D ARDUINO_USB_CDC_ON_BOOT=1

## Project Structure Hints

The project follows a standard PlatformIO layout:

- `/src`: Contains the main source code, including the `main.cpp` file.
- `platformio.ini`: The main configuration file for PlatformIO.

## Project task code, documentation and git repo

- [x] create `platformio.ini`
- [x] add `board = esp32-c3-devkitm-1`
- [x] add lib-depends
- [x] create `/src` directory and add `main.cpp` with project code
- [x] Add `<Arduino.h>` to `main.cpp`
- [x] execute `clang-format -i --style=Microsoft src/main.cpp`
- [x] create a github style README.md describing the project
- [x] create a .gitignore with .pio/ and .vscode/
- [x] create a git repo with `git init`
- [x] add the files to repo with `git add .`
- [x] update `agents.md`
- [x] git commit with `git commit -m "<message>"`


## MQTT Integration

### Overview
Added comprehensive MQTT support for remote temperature monitoring and data publishing. The system connects to WiFi and publishes temperature data to an MQTT broker in real-time.

### Features
- **WiFi Connectivity**: Automatic connection to configured WiFi network
- **MQTT Publishing**: Real-time publishing of temperature data to MQTT topics
- **Connection Management**: Automatic reconnection handling for both WiFi and MQTT
- **Status Reporting**: Online/offline status publishing to MQTT broker

### MQTT Configuration
The following constants need to be configured in `src/config.h`:



### MQTT Topics
- `esp32/temperature/current`: Publishes current temperature readings every second
- `esp32/status`: Publishes device online/offline status

### Dependencies
- **PubSubClient**: Arduino MQTT client library for ESP32
- **WiFi**: Built-in ESP32 WiFi library for network connectivity

### Technical Implementation
- **Connection Management**: Non-blocking WiFi and MQTT connection handling
- **Reconnection Logic**: Automatic reconnection with configurable intervals
- **Error Handling**: Graceful handling of connection failures
- **Memory Efficient**: Minimal memory footprint for embedded system
- **Real-time Publishing**: Immediate publishing of temperature data when MQTT is connected

### Usage
1. Configure WiFi and MQTT settings in `src/config.h`
2. Ensure MQTT broker is running and accessible
3. Upload firmware to ESP32-C3 board
4. Monitor serial output for connection status
5. Subscribe to MQTT topics to receive temperature data

### MQTT Message Format
Temperature data is published as floating-point values in Celsius:
- Current temperature: `25.67`
- Status messages: `online`, `offline`

