#include <Arduino.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

#define SOUND_PIN 32
#define VCC 3.3

#define WATER_LEVEL_PIN 34

DHT dht(DHTPIN, DHTTYPE);

struct TempHumidity {
  float humidity;
  float temperature;
  bool valid; // เช็คว่าค่าอ่านได้หรือไม่
};

TempHumidity getTempAndHumidity() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  TempHumidity result;
  if (isnan(h) || isnan(t)) {
    result.valid = false;
  } else {
    result.humidity = h;
    result.temperature = t;
    result.valid = true;
  }
  return result;
}

float getSoundVolt() {
  int rawValue = analogRead(SOUND_PIN); // อ่านค่า 0-4095
  float voltage = rawValue * VCC / 4095.0; // แปลงเป็นโวลต์
  return voltage;
}

int getWaterLevel() {
  int rawValue = analogRead(WATER_LEVEL_PIN); // อ่านค่า 0-4095
  return rawValue;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  TempHumidity th = getTempAndHumidity();
  if (th.valid) {
    Serial.print("Humidity: "); Serial.print(th.humidity);
    Serial.print(" %, Temp: "); Serial.print(th.temperature); Serial.println(" *C");
  } else {
    Serial.println("Failed to read from DHT sensor!\n");
  }

  float soundVolt = getSoundVolt();
  Serial.print("Sound Voltage: "); Serial.print(soundVolt);
  Serial.print("\n");

  int waterLevel = getWaterLevel();
  Serial.print("Water Level: "); Serial.print(waterLevel);
  Serial.print("\n");

  delay(2000); // อ่านทุก 2 วินาที
}