#define BLYNK_TEMPLATE_ID "TMPL6H-cMWIcR"
#define BLYNK_TEMPLATE_NAME "Proj"
#define BLYNK_AUTH_TOKEN "g-bdAArnro7kshKZ7LR4WB6nrya8iH9I"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "DHT.h"

// ------- PIN DEFINITIONS -------
#define DHTPIN 4
#define DHTTYPE DHT11

#define SOUND_PIN 32
#define WATER_LEVEL_PIN 34
#define VCC 3.3

// ------- WIFI -------
char ssid[] = "wifi_username";
char pass[] = "wifi_password";

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// =====================================================
// Send DHT11 temperature & humidity
// =====================================================
void sendTempHumidity()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %, Temp: ");
  Serial.println(t);

  Blynk.virtualWrite(V2, h);  // Humidity
  Blynk.virtualWrite(V3, t);  // Temperature
}

// =====================================================
// Send sound voltage level
// =====================================================
void sendSoundLevel()
{
  int rawValue = analogRead(SOUND_PIN);
  float voltage = rawValue * VCC / 4095.0;

  Serial.print("Sound Voltage: ");
  Serial.println(voltage);

  Blynk.virtualWrite(V4, voltage);
}

// =====================================================
// Send water level (RAW ADC)
// =====================================================
void sendWaterLevel()
{
  int rawValue = analogRead(WATER_LEVEL_PIN);

  Serial.print("Water Level: ");
  Serial.println(rawValue);

  Blynk.virtualWrite(V1, rawValue);  // already created
}

void setup()
{
  Serial.begin(115200);
  dht.begin();

  pinMode(SOUND_PIN, INPUT);
  pinMode(WATER_LEVEL_PIN, INPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Timers (every 2 seconds)
  timer.setInterval(2000L, sendWaterLevel);
  timer.setInterval(2500L, sendTempHumidity);
  timer.setInterval(3000L, sendSoundLevel);
}

void loop()
{
  Blynk.run();
  timer.run();
}
