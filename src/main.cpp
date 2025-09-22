// ESP32-C3 Temperature Monitoring System
//
// This program reads temperature data from a DS18B20 digital temperature sensor
// connected via OneWire protocol and publishes current temperature readings.
//
// Hardware Setup:
// - DS18B20 sensor connected to GPIO0 (OneWire data pin)
// - 4.7k pull-up resistor recommended between data and VCC
//
// Features:
// - Continuous temperature sampling every 10 second
// - Serial output for monitoring and debugging
// - MQTT publishing of current temperature

#include <Arduino.h> // Core Arduino framework functions
#include <OneWire.h> // Library for 1-Wire communication protocol
#include <DallasTemperature.h> // Library for DS18B20 temperature sensor
#include <PubSubClient.h> // Library for MQTT communication
#include <WiFi.h> // Library for WiFi connectivity

#include "config.h" // WiFi and MQTT credentials

// OneWire bus pin configuration
// The DS18B20 sensor's data pin is connected to GPIO1
#define ONE_WIRE_BUS 1

// OneWire instance for communication with DS18B20 sensor
// Handles the low-level OneWire protocol communication
OneWire oneWire(ONE_WIRE_BUS);

// DallasTemperature instance for high-level sensor operations
// Provides convenient methods for temperature reading and sensor management
DallasTemperature sensors(&oneWire);

// Time interval between temperature samples in milliseconds
// 10000ms = 10 second sampling rate
#define SAMPLE_INTERVAL 10000 // 10 second in milliseconds

// MQTT Configuration
#define MQTT_TOPIC_STATUS "esp32/status"
#define MQTT_TOPIC_TEMPERATURE "sensor3/temp"

// MQTT Broker settings - replace with your broker details
#define MQTT_PORT 1883

// Timestamp of the last temperature sample in milliseconds
// Used to maintain consistent sampling intervals
// Prevents multiple samples within the same interval
unsigned long lastSampleTime = 0;

// MQTT and WiFi connection management
// WiFiClient provides the underlying TCP connection for MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Connection status tracking
bool wifiConnected = false;
bool mqttConnected = false;
unsigned long lastReconnectAttempt = 0;
unsigned long lastMqttPublish = 0;

// MQTT reconnection interval in milliseconds
// Prevents excessive reconnection attempts
#define MQTT_RECONNECT_INTERVAL 5000

// MQTT callback function for incoming messages
// This function is called when a message is received on a subscribed topic
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
    // Convert payload to string for easier processing
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    // Log received message for debugging
    Serial.print("MQTT Message received [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(message);

    // Handle different topics if needed
    // Currently just logs the message, but could be extended for remote control
}

//
// Connect to WiFi network
// Returns true if connection is successful, retry otherwise
//
bool connectToWiFi()
{
    // Init WiFI
	WiFi.enableAP(false);
    WiFi.mode(WIFI_STA);
    // Reduce power for supermini antenna reflection
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    // WiFi connect
    WiFi.begin(ssid, password);
    // reduce power for supermini antenna problem
    // wait 10 S before reboot
    int trying = 10;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
        if (trying == 0)
            ESP.restart();
        else
            trying--;
    }
    return true;
}

//
// Connect to MQTT broker
// Returns true if connection is successful, false otherwise
//
bool connectToMQTT()
{
    // Set MQTT server details
    mqttClient.setServer(mqtt_server, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    Serial.print("Connecting to MQTT broker ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.print(MQTT_PORT);

    // Attempt to connect with client ID
    if (mqttClient.connect(mqtt_server))
    {
        Serial.println("\nMQTT connected!");

        // Publish online status
        mqttClient.publish(MQTT_TOPIC_STATUS, "online");

        // Subscribe to topics if needed
        // mqttClient.subscribe("your/control/topic");

        mqttConnected = true;
        return true;
    }
    else
    {
        Serial.println("\nMQTT connection failed!");
        Serial.print("Error code: ");
        Serial.println(mqttClient.state());
        mqttConnected = false;
        return false;
    }
}

//
// Publish temperature data to MQTT broker
// Publishes current temperature
//
void publishTemperatureData(float currentTemp)
{
    char payload[20]; // Buffer for payload strings

    // Publish current temperature
    dtostrf(currentTemp, 6, 1, payload);
    mqttClient.publish(MQTT_TOPIC_TEMPERATURE, payload);

    // Log published temperature
    Serial.print("Published to MQTT: ");
    Serial.print(payload);
    Serial.println(" C");
}

//
// Maintain MQTT connection and handle reconnections
//
void handleMQTTConnection()
{
    unsigned long currentTime = millis();

    // Check if WiFi is still connected
    if (WiFi.status() != WL_CONNECTED)
    {
        wifiConnected = false;
        mqttConnected = false;
        Serial.println("WiFi disconnected!");

        // Attempt to reconnect to WiFi
        if (connectToWiFi())
        {
            // WiFi reconnected, now try MQTT
            connectToMQTT();
        }
        return;
    }

    // Check if MQTT is connected
    if (!mqttClient.connected())
    {
        mqttConnected = false;

        // Check if enough time has passed since last reconnection attempt
        if (currentTime - lastReconnectAttempt > MQTT_RECONNECT_INTERVAL)
        {
            lastReconnectAttempt = currentTime;

            Serial.println("Attempting MQTT reconnection...");
            if (connectToMQTT())
            {
                Serial.println("MQTT reconnected successfully!");
            }
        }
    }
    else
    {
        // MQTT is connected, process any incoming messages
        mqttClient.loop();
    }
}

//
// Arduino setup function - runs once at startup
//
// Initializes all hardware components and prepares the system
// for temperature monitoring operations.
//
// Initialization sequence:
// 1. Configure serial communication for debugging/output
// 2. Initialize the DallasTemperature library
// 3. Verify sensor connection and readiness
//
void setup(void)
{
    // Initialize serial communication at 115200 baud
    // This allows for high-speed data transfer to serial monitor
    // 115200 is a common baud rate for ESP32 debugging
    Serial.begin(115200);

    // Print startup message to serial monitor
    // Helps verify that the program is running correctly
    // Useful for debugging initialization issues
    Serial.println("ESP32-C3 Temperature Monitoring System with MQTT");
    Serial.println("================================================");

    // Initialize the DallasTemperature library
    // This discovers connected DS18B20 sensors on the OneWire bus
    // Must be called before attempting to read temperatures
    sensors.begin();

    // Print sensor information for debugging
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" DS18B20 sensor(s)");

    // Initialize WiFi and MQTT connections
    Serial.println("\nInitializing network connections...");

    // Connect to WiFi
    if (connectToWiFi())
    {
        // WiFi connected successfully, now connect to MQTT
        connectToMQTT();
    }
    else
    {
        Serial.println("WiFi connection failed. MQTT will be disabled.");
        Serial.println("Please check your WiFi credentials in the code.");
    }

    Serial.println("Setup complete!");
    Serial.println("================================================");
}

//
// Main program loop - runs continuously after setup
//
// This function implements the core temperature monitoring logic:
// - Maintains precise 1-second sampling intervals
// - Collects temperature readings from DS18B20 sensor
// - Publishes current temperature to MQTT
// - Provides real-time feedback via serial output
//
// The loop uses non-blocking timing to ensure consistent sampling
// without interfering with other potential system operations.
//
void loop(void)
{
    // Get current time in milliseconds since system start
    // millis() provides a reliable timing reference
    // Note: millis() overflows after ~50 days, but this is acceptable for this application
    unsigned long currentTime = millis();

    // Handle MQTT connection maintenance
    // This ensures WiFi and MQTT connections remain active
    // Reconnects automatically if connection is lost
    handleMQTTConnection();

    // Check if it's time to take a new temperature sample
    // This implements a non-blocking delay mechanism
    // Ensures exactly 1 sample per second regardless of processing time
    if (currentTime - lastSampleTime >= SAMPLE_INTERVAL)
    {
        // Update timestamp for next interval calculation
        lastSampleTime = currentTime;

        // Request temperature conversion from DS18B20 sensor
        // This command initiates temperature measurement on the sensor
        // The sensor requires time to perform the conversion (typically 750ms)
        sensors.requestTemperatures();

        // Read temperature from first sensor (index 0)
        // getTempCByIndex(0) returns temperature in Celsius
        // Returns DEVICE_DISCONNECTED_C if sensor is not found
        // Note: No error checking is implemented in current version
        float currentTemp = sensors.getTempCByIndex(0);

        // Print current temperature information to serial monitor
        // Shows current temperature reading
        // Useful for real-time monitoring and debugging
        Serial.print("Current temperature: ");
        Serial.print(currentTemp);
        Serial.println(" C");

        // Publish current temperature to MQTT if connected
        // This allows external systems to receive real-time temperature data
        if (mqttConnected)
        {
            publishTemperatureData(currentTemp);
        }
    }
    delay(10);
    // End of main loop iteration
    // The loop will continue running indefinitely
    // Next iteration will check timing and potentially take new sample
}
