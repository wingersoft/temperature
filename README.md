# ESP32-C3 Temperature Monitoring System

This project is an **Arduino ESP32** application developed using the **PlatformIO** framework for temperature monitoring with DS18B20 sensor and MQTT integration. The system reads temperature from GPIO0, displays temperature in Celsius on serial monitor, and publishes data to MQTT broker for remote monitoring.

## Project Overview

**ESP32-C3 Temperature Monitoring System** with DS18B20 sensor and MQTT integration. Reads temperature from GPIO0, displays temperature in Celsius on serial monitor, and publishes data to MQTT broker for remote monitoring.

### Key Features
- **Temperature Monitoring**: Continuous DS18B20 sensor readings with 1-second sampling
- **Serial Output**: Real-time temperature display and debugging information
- **MQTT Integration**: WiFi connectivity and MQTT publishing for remote monitoring
- **Connection Management**: Automatic WiFi and MQTT reconnection handling
- **Status Reporting**: Device online/offline status publishing

## Hardware Setup

- ESP32-C3 DevKitM-1 (supermini)
- DS18B20 temperature sensor
- 4.7kΩ pull-up resistor between VCC and DATA line of the DS18B20 sensor
- Connect the DS18B20 sensor's DATA pin to GPIO0 of the ESP32-C3

## Features

### Temperature Monitoring
- **Real-time readings**: Temperature is sampled every second
- **Continuous operation**: Provides ongoing temperature monitoring

### Serial Output
The system provides real-time temperature readings:
- **Current temperature**: Shows each temperature reading as it's collected (e.g., "Current temperature: 23.45°C")


### MQTT Integration

#### Overview
Added comprehensive MQTT support for remote temperature monitoring and data publishing. The system connects to WiFi and publishes temperature data to an MQTT broker in real-time.

#### Features
- **WiFi Connectivity**: Automatic connection to configured WiFi network
- **MQTT Publishing**: Real-time publishing of temperature data to MQTT topics
- **Connection Management**: Automatic reconnection handling for both WiFi and MQTT
- **Status Reporting**: Online/offline status publishing to MQTT broker

#### MQTT Configuration
The following constants need to be configured in `src/config.h`:
- const char* ssid = "";
- const char* password = "";
- const char *mqtt_server = "";


#### MQTT Topics
- `sensor3/temp`: Publishes current temperature readings every second
- `esp32/status`: Publishes device online/offline status

#### Usage
1. Configure WiFi and MQTT settings in `src/config.h`
2. Ensure MQTT broker is running and accessible
3. Upload firmware to ESP32-C3 board
4. Monitor serial output for connection status
5. Subscribe to MQTT topics to receive temperature data

#### MQTT Message Format
Temperature data is published as floating-point values in Celsius:
- Current temperature: `25.67`
- Status messages: `online`, `offline`

## Software Dependencies

- PlatformIO
- OneWire library
- DallasTemperature library
- PubSubClient (for MQTT functionality)
- WiFi library (built-in ESP32 WiFi library)

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
  - Use `camelCase` for variables and functions
  - Use `PascalCase` for class names
  - Document public functions with comments
  - Avoid using `using namespace std;` in header files
  - Add to platformio.ini:
    ```
    build_flags =
        -D ARDUINO_USB_MODE=1
        -D ARDUINO_USB_CDC_ON_BOOT=1
    ```

## Project Structure

The project follows a standard PlatformIO layout:

- `/src`: Contains the main source code, including the `main.cpp` file
- `platformio.ini`: The main configuration file for PlatformIO
- `/doc`: Documentation files including DS18B20 datasheet
- `README.md`: This project documentation
- `agents.md`: Detailed project documentation and implementation notes
