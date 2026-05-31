#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>

// WiFi Credentials
const char* ssid = "YourWifiSSID";
const char* password = "YourWifiPassword";

// MQTT Broker IP
const char* mqtt_server = "LinuxMachineIPAddress";

// Thermistor
const int sensorPin = A0;

// WS2812B
#define LED_PIN 2
#define NUM_LEDS 8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("WiFi Connected");
}

void reconnect() {

  while (!client.connected()) {

    if (client.connect("ESP8266_Temp_Sensor")) {

      Serial.println("MQTT Connected");

    } else {

      delay(5000);
    }
  }
}

// Set all LEDs to one color
void setColor(uint8_t r, uint8_t g, uint8_t b) {

  for (int i = 0; i < NUM_LEDS; i++) {

    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  strip.show();
}

void setup() {

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);

  strip.begin();
  strip.show();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Read Thermistor
  int rawADC = analogRead(sensorPin);

  if (rawADC == 0) {
    rawADC = 1;
  }

  double tempVal = log(((1023.0 / rawADC) - 1.0) * 10000.0);

  double tempK = 1.0 / (
      0.001129148 +
      (0.000234125 +
      (0.0000000876741 * tempVal * tempVal)) * tempVal
  );

  float tempC = tempK - 273.15;

  // MQTT Payload
  char payloadC[10];

  dtostrf(tempC, 4, 1, payloadC);

  client.publish("home/sensor/tempC", payloadC);

  // Temperature LED Logic
  if (tempC < 10) {

    // Green
    setColor(0, 255, 0);

  } else if (tempC >= 10 && tempC < 12) {

    // Yellow
    setColor(255, 255, 0);

  } else {

    // Red
    setColor(255, 0, 0);
  }

  Serial.print("Temperature: ");
  Serial.println(tempC);

  delay(5000);
}