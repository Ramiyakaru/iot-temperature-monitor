#include <math.h>

// ESP8266 ADC resolution is 10-bit (0 - 1023)
const int sensorPin = A0; 

void setup() {
  Serial.begin(115200); // Higher baud rate is standard for ESP8266
  while (!Serial) { ; } // Wait for serial port to connect
  Serial.println("ESP8266 Temperature Sensor Initialized");
}

void loop() {
  int rawADC = analogRead(sensorPin);
  
  // Prevent division by zero if reading is 0
  if (rawADC == 0) rawADC = 1; 

  // Keyestudio formula adjusted for 10-bit ADC 
  // Calculates the resistance of the thermistor
  double tempVal = log(((1023.0 / rawADC) - 1.0) * 10000.0); 
  
  // Steinhart-Hart equation for temperature conversion to Kelvin
  double tempK = 1.0 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempVal * tempVal)) * tempVal);
  
  // Convert Kelvin to Celsius
  float tempC = tempK - 273.15;            
  
  // Convert Celsius to Fahrenheit
  float tempF = (tempC * 9.0 / 5.0) + 32.0; 

  // Print results to the Serial Monitor
  Serial.print(" | Temp: ");
  Serial.print(tempC, 1); // 1 decimal place
  Serial.print(" °C  /  ");
  Serial.print(tempF, 1);
  Serial.println(" °F");

  delay(2000); // Wait 2 seconds before the next reading
}