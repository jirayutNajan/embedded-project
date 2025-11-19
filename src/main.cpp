#include <Arduino.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  // อ่าน humidity
  float h = dht.readHumidity();
  // อ่าน temperature
  float t = dht.readTemperature(); // °C
  // อ่าน temperature แบบ Fahrenheit
  // float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
  }

  delay(2000); // อ่านทุก 2 วินาที
}
