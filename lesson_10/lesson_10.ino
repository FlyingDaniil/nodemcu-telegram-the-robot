//пины и настройки
#define waterSensor 0
#define led 5
#define led2 4
#define minWater 100 //минимальный уровень воды для включения светодиода от 0 до 1023

//файл с настройками сети и токеном бота
#include "secrets.h"

//подключение библиотек
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

//объявление бота
WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

void setup() {
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  wifiConnect();
  Serial.begin(9600);
}

void loop() {
  //получаем значение уровня
  int x = analogRead(waterSensor);

  //управляем светодиодом
  if (x > minWater) {
    digitalWrite(led, HIGH);
    digitalWrite(led2, LOW);
  } else {
    digitalWrite(led, LOW);
    digitalWrite(led2, HIGH);
  }

  //получение сообщения
  message m = bot.getUpdates();

  //если запрос уровня воды
  if (m.text.equals("Water")) {
    //то отправляем данные
    bot.sendMessage(m.chat_id, String(x) + " /1023");
  }
}

//процедура для подключения к wifi
void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}
