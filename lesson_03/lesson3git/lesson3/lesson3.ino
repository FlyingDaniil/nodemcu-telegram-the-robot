#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

#define DS18B21 12  //D6
 
const char* ssid = "x";
const char* password = "x";
const char BotToken[] = "x";

OneWire oneWire(DS18B21);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer1 = {0x28, 0xFF, 0x4E, 0x13, 0x50, 0x17, 0x04, 0x2B}; 
DeviceAddress Thermometer2 = {0x28, 0xFF, 0x1B, 0x14, 0x50, 0x17, 0x04, 0xEA}; 
WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

void setup() {
  sensors.begin();
  sensors.setResolution(Thermometer1, 10);
  sensors.setResolution(Thermometer1, 20);
  
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
//
  bot.begin(); 
}


void loop() {
  message m = bot.getUpdates(); 
  Serial.println(m.text);
  
  if (m.text.equals("temp1")){
    sensors.requestTemperatures();
    bot.sendMessage(m.chat_id, String(printTemperature(Thermometer1))); 
  } 
  else if (m.text.equals("temp2")){
    sensors.requestTemperatures();
    bot.sendMessage(m.chat_id, String(printTemperature(Thermometer2))); 
  }
}



float printTemperature(DeviceAddress deviceAddress) {
  return sensors.getTempC(deviceAddress);
}
