#define BLYNK_TEMPLATE_ID "TMPL6H-cMWIcR"
#define BLYNK_TEMPLATE_NAME "Proj"
#define BLYNK_AUTH_TOKEN "g-bdAArnro7kshKZ7LR4WB6nrya8iH9I"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "DHT.h"
#include <HTTPClient.h>

String GAS_URL = "https://script.google.com/macros/s/AKfycbxOUED034r8Vyhbjur597YmROFteTDIbY1LF1FFCQjea1lLRFVQY6xT50GLGUejPhG5aA/exec"; 

// ------- PIN DEFINITIONS -------
#define DHTPIN 4
#define DHTTYPE DHT11

#define SOUND_PIN 32
#define WATER_LEVEL_PIN 34
#define VCC 3.3

// ------- WIFI -------
char ssid[] = "T";
char pass[] = "homeless";

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

double soundValue;
int WaterValue;
double HumidValue;
double TempValue;


// =====================================================
// Send DHT11 temperature & humidity
// =====================================================
void sendTempHumidity()
{
  HumidValue = dht.readHumidity();
  TempValue = dht.readTemperature();

  if (isnan(HumidValue) || isnan(TempValue)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(HumidValue);
  Serial.print(" %, Temp: ");
  Serial.println(TempValue);

  Blynk.virtualWrite(V2, HumidValue);  // Humidity
  Blynk.virtualWrite(V3, TempValue);  // Temperature
}

// =====================================================
// Send sound voltage level
// =====================================================
void sendSoundLevel()
{
  soundValue = analogRead(SOUND_PIN);
  float voltage = soundValue * VCC / 4095.0;

  Serial.print("Sound Voltage: ");
  Serial.println(voltage);

  Blynk.virtualWrite(V4, voltage);
}

// =====================================================
// Send water level (RAW ADC)
// =====================================================
void sendWaterLevel()
{
  WaterValue = analogRead(WATER_LEVEL_PIN);

  Serial.print("Water Level: ");
  Serial.println(WaterValue);

  Blynk.virtualWrite(V1, WaterValue);  // already created
}

void sendToGoogleSheet() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    float temp = TempValue;
    float humid = HumidValue;
    int water = WaterValue;

    String url = GAS_URL + "?temp=" + String(temp) + "&humid=" + String(humid)+"&water=" + String(water);

    http.begin(url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println(http.getString());
    }

    http.end();
  }
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
  timer.setInterval(60000L, sendToGoogleSheet);
}

void loop()
{
  Blynk.run();
  timer.run();
}
