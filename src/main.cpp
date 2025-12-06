#define BLYNK_TEMPLATE_ID "TMPL6H-cMWIcR"
#define BLYNK_TEMPLATE_NAME "Proj"
#define BLYNK_AUTH_TOKEN "g-bdAArnro7kshKZ7LR4WB6nrya8iH9I"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "DHT.h"
#include <HTTPClient.h>

String GAS_URL = "https://script.google.com/macros/s/AKfycbxOUED034r8Vyhbjur597YmROFteTDIbY1LF1FFCQjea1lLRFVQY6xT50GLGUejPhG5aA/exec"; 

// ------- PIN CLARIFICATION -------
#define DHTPIN 4
#define DHTTYPE DHT11

#define SOUND_PIN 32
#define WATER_LEVEL_PIN 34
#define VCC 3.3

// ------- WIFI -------
char ssid[] = "T";
char pass[] = "homeless";
const int LOW_WATER_THRESHOLD = 500; 

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

double soundValue;
int WaterValue;
double HumidValue;
double TempValue;


//DHT11
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

//Sound
void sendSoundLevel()
{
  soundValue = analogRead(SOUND_PIN);
  float voltage = soundValue * VCC / 4095.0;

  Serial.print("Sound Voltage: ");
  Serial.println(voltage);

  Blynk.virtualWrite(V4, voltage);
}

//Water Level
void sendWaterLevel()
{
  WaterValue = analogRead(WATER_LEVEL_PIN);
  Blynk.virtualWrite(V1, WaterValue); 

  if (WaterValue < LOW_WATER_THRESHOLD) {
    Blynk.logEvent("water_alert", "Water level too low: " + String(WaterValue)); 
  }

  Serial.print("Water Level: ");
  Serial.println(WaterValue);
}

//send data from sensor to google sheets
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
      Serial.print("Google Sheet Response: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Google Sheet HTTP Error: ");
      Serial.println(httpCode);
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