#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <math.h>

// WiFi Configuration
const char* ssid = "YourWifiSSID";
const char* password = "YourWifiPassword";

// MQTT Broker IP
const char* mqtt_server = "LinuxMachineIPAddress";

// Thermistor connected to A0
const int sensorPin = A0;

// WiFi + MQTT Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Connect to WiFi
void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to MQTT Broker
void reconnect() {
  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266_Temp_Sensor")) {

      Serial.println("connected");

    } else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

void loop() {

  // Reconnect MQTT if disconnected
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Read analog value from thermistor
  int rawADC = analogRead(sensorPin);

  // Prevent divide-by-zero
  if (rawADC == 0) {
    rawADC = 1;
  }

  // Steinhart-Hart Thermistor Calculation
  double tempVal = log(((1023.0 / rawADC) - 1.0) * 10000.0);

  double tempK = 1.0 / (
      0.001129148 +
      (0.000234125 +
      (0.0000000876741 * tempVal * tempVal)) * tempVal
  );

  // Convert Kelvin to Celsius/Fahrenheit
  float tempC = tempK - 273.15;
  float tempF = (tempC * 9.0 / 5.0) + 32.0;

  // Buffers for MQTT payloads
  char payloadC[10];
  char payloadF[10];

  // Convert float values to strings
  dtostrf(tempC, 4, 1, payloadC);
  dtostrf(tempF, 4, 1, payloadF);

  // Publish to MQTT topics
  client.publish("home/sensor/tempC", payloadC);
  client.publish("home/sensor/tempF", payloadF);

  // Serial Monitor Output
  Serial.print("Temperature C: ");
  Serial.println(payloadC);

  Serial.print("Temperature F: ");
  Serial.println(payloadF);

  Serial.println("----------------------");

  // Send every 5 seconds
  delay(5000);
}