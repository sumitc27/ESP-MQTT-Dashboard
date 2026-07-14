#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

using namespace std;

// --- Network & MQTT Settings ---
const char* ssid = "your-wifi-name";          // Replace with your WiFi Name
const char* password = "wifi-password";  // Replace with your WiFi Password

// --- EMQX Cloud Settings ---
const char* mqtt_server = "your-emqx-server.com"; // Replace with your EMQX Server Address
const int mqtt_port = 8883; // Secure MQTT Port
const char* mqtt_username = "username"; // Create this in EMQX Console -> Authentication
const char* mqtt_password = "password"; // Create this in EMQX Console -> Authentication
const char* mqtt_topic = "your/topic"; // Replace with your desired topic

// Use WiFiClientSecure for port 8883
WiFiClientSecure espClient;
PubSubClient client(espClient);

// --- Pin Definitions ---
const int VOLTAGE_SENSOR_1_PIN = 7; // VSens1
const int VOLTAGE_SENSOR_2_PIN = 6; // VSens2
const int HW122_1_VOUT_PIN = 5;     // AC 1
const int HW122_2_VOUT_PIN = 4;     // AC 2

unsigned long lastMsg = 0;
const long interval = 1000; // Publish every second

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Bypassing SSL Certificate Validation for easier testing
  // Note: For production, you should use espClient.setCACert(root_ca);
  espClient.setInsecure(); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting Secure MQTT connection...");
    String clientId = "ESP32-S3-Station-";
    clientId += String(random(0xffff), HEX);
    
    // Connect using Client ID, Username, and Password
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, state=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); 

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    // 1. Read and Calculate Battery Voltages
    float battery1Voltage = (analogRead(VOLTAGE_SENSOR_1_PIN) / 4095.0) * 3.3 * 5.0;
    float battery2Voltage = (analogRead(VOLTAGE_SENSOR_2_PIN) / 4095.0) * 3.3 * 5.0;

    // 2. Read AC Power Statuses
    String ac1Status = (analogRead(HW122_1_VOUT_PIN) > 2000) ? "ON" : "OFF";
    String ac2Status = (analogRead(HW122_2_VOUT_PIN) > 2000) ? "ON" : "OFF";

    // 3. Create Expanded JSON Payload
    String payload = "{";
    payload += "\"battery_1_voltage\": " + String(battery1Voltage, 2) + ", ";
    payload += "\"battery_2_voltage\": " + String(battery2Voltage, 2) + ", ";
    payload += "\"ac_1_status\": \"" + ac1Status + "\", ";
    payload += "\"ac_2_status\": \"" + ac2Status + "\"";
    payload += "}";

    // 4. Publish to EMQX
    Serial.print("Publishing: ");
    Serial.println(payload);
    client.publish(mqtt_topic, payload.c_str());
  }
}