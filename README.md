# ESP Dashboard for ESP-32 Sensor Monitoring

This repository contains a simple end-to-end monitoring setup for an ESP-32 based sensor dashboard.

The ESP-32 reads multiple inputs, packages the readings as JSON, and publishes them to an EMQX MQTT topic. A browser-based dashboard subscribes to the same topic over MQTT WebSockets and displays the live values.

## Project Overview

The system is split into two parts:

- `mqtt-wifi.ino` runs on the ESP-32 and publishes sensor data to EMQX.
- `dashboard-index.html` is the web dashboard that subscribes to the MQTT topic and shows the latest readings.

## Data Flow

1. The ESP-32 connects to Wi-Fi.
2. It reads voltage and AC status inputs.
3. It builds a JSON payload.
4. The payload is published to the EMQX topic `monitoring_station/power_data`.
5. The dashboard subscribes to that topic and updates the UI in real time.

## Published Payload

The ESP-32 publishes JSON in this format:

```json
{
  "battery_1_voltage": 12.34,
  "battery_2_voltage": 11.98,
  "ac_1_status": "ON",
  "ac_2_status": "OFF"
}
```

## Repository Files

- `mqtt-wifi.ino` - ESP-32 firmware that reads sensors and publishes data.
- `dashboard-index.html` - Browser dashboard for live monitoring.
- `index.html` - Alternate HTML entry point, if you want to use a different dashboard page.

## Requirements

- ESP-32 board supported by the Arduino IDE.
- Wi-Fi network access.
- EMQX broker account or server access.
- A modern browser with MQTT over WebSockets support.

## ESP-32 Setup

Open `mqtt-wifi.ino` in the Arduino IDE and update these values before uploading:

- Wi-Fi SSID and password
- EMQX broker host
- MQTT username and password
- MQTT topic, if you want to use a different one

The sketch currently uses these defaults:

- Broker: `j18eff7a.ala.asia-southeast1.emqxsl.com`
- MQTT port: `8883`
- Topic: `monitoring_station/power_data`

## Dashboard Setup

Open `dashboard-index.html` in a browser or serve it from a local web server.

The dashboard connects to EMQX using WebSockets and listens on the same topic as the ESP-32 publisher. If you change the broker address, username, password, or topic in the sketch, update the same values in the dashboard script as well.

## Notes

- The sketch currently uses `espClient.setInsecure()` for easier testing with TLS.
- For production use, configure proper certificate validation.
- The sensor pin assignments in the sketch may need to be adjusted for your hardware.

## Customization

You can extend this project by:

- Adding more sensor fields to the JSON payload.
- Showing additional cards or charts in the dashboard.
- Changing the publish interval.
- Replacing the EMQX topic with your own topic hierarchy.
