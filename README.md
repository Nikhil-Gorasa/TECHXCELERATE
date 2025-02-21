# Piezoelectric Sensor with GPS Monitoring

## Overview
This project utilizes two piezoelectric sensors to measure vibrations and detect anomalies based on frequency thresholds. Additionally, a GPS module is integrated to track the location of the device. The system provides real-time data output via the serial monitor and uses LEDs to indicate normal operation or detected anomalies.

## Hardware Requirements
- Arduino Board (e.g., Arduino Uno)
- Two Piezoelectric Sensors (connected to A0 and A1)
- GPS Module (connected via SoftwareSerial to pins 8 and 9)
- Two LEDs (one for normal operation, one for alerts)
- Resistors and jumper wires as necessary

## Pin Configuration
| Component         | Pin       |
|------------------|----------|
| Piezo Sensor 1   | A0       |
| Piezo Sensor 2   | A1       |
| LED (Alert)      | 13       |
| LED (Normal)     | 11       |
| GPS TX          | 8        |
| GPS RX          | 9        |

## Features
- Reads and analyzes vibration data from two piezoelectric sensors.
- Calculates frequency and amplitude of vibrations.
- Determines anomalies based on predefined frequency thresholds.
- Displays formatted data on the serial monitor.
- Flickers an LED to indicate an anomaly.
- Retrieves GPS coordinates to track the device location.

## Functionality
1. *Sensor Data Collection*:
   - Reads analog values from the piezo sensors.
   - Measures peak amplitudes and cycle counts to determine frequency.
   
2. *Anomaly Detection*:
   - Compares average frequency against defined thresholds (20Hz - 60Hz).
   - If the frequency is outside this range, an anomaly is detected, and the alert LED flickers.
   - If within the range, the normal operation LED remains ON.

3. *GPS Tracking*:
   - Retrieves latitude and longitude from the GPS module.
   - Displays GPS coordinates on the serial monitor if a fix is available.

## Installation & Usage
1. Connect all hardware components as per the pin configuration.
2. Upload the provided code to the Arduino board.
3. Open the serial monitor (9600 baud) to view real-time data.
4. Observe LED indicators:
   - *Normal LED ON (No anomaly detected)*
   - *Alert LED Flickering (Anomaly detected)*

## Code Explanation
- setup(): Initializes serial communication, sensor pins, and LEDs.
- loop(): Reads sensor data, computes amplitude and frequency, detects anomalies, and updates GPS data.
- updateGPS(): Reads data from the GPS module and extracts valid coordinates.
- flickerLED(): Flickers the alert LED when an anomaly is detected.

## Dependencies
Ensure the following libraries are installed:
cpp
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

Install these via the Arduino Library Manager if not already available.

## Future Enhancements
- Store GPS and sensor data for historical analysis.
- Send data to the cloud for remote monitoring.
- Implement additional sensors for enhanced anomaly detection.

## License
This project is open-source and can be modified and distributed freely.

## Author
[TEAM IOTA]
