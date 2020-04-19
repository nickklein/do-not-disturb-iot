#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>

StaticJsonDocument<200> doc;
String requestUrl = "";
String bearer = "";
String payload;

#define PIN        14 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 12 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

String fetch(String url) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("Authorization", bearer);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();
    if (httpCode == 200) {
      payload = http.getString();
      http.end();
      return payload;
    }
    return "";
}

void setLightColor(int one, int two, int three) {
  pixels.begin();
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(one, two, three));
  }
}

void setLight(String state) {
  if (state == "on") {
     setLightColor(148, 0, 211);
     pixels.show();
  } else {
    setLightColor(0, 0, 0);
    pixels.show(); 
  }
}

void setup() {
    Serial.begin(115200);
    WiFiManager wifiManager;
    wifiManager.autoConnect("DND");
    setLightColor(0, 0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");

    payload = fetch(requestUrl);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    
    int value = doc["value"];
    Serial.println(value);
    if (value == 1) {
      setLight("on");
    } else {
      setLight("off");
    }
  }
  delay(5000);
}
