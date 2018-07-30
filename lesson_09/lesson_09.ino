#include "secrets.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>
#include <SFE_BMP180.h>
#include <Wire.h>

WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);
SFE_BMP180 pressure;

void setup() {
  Serial.begin(9600);
  wifiConnect();
  bot.begin(); 
  pressure.begin();
}

void loop() {
  message m = bot.getUpdates(); 
  
  if (m.text.equals("Pressure")){     
    bot.sendMessage(m.chat_id, String(round(getPressure()))+" Pa");  
  }
}

void wifiConnect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}

double getPressure() {
  char status;
  double T, P, p0, a;
  status = pressure.startTemperature();
  if (status != 0) {
    // ожидание замера температуры
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0) {
      status = pressure.startPressure(3);
      if (status != 0) {
        // ожидание замера давления
        delay(status);
        status = pressure.getPressure(P, T);
        if (status != 0) {
          return (P);
        }
      }
    }
  }
}
