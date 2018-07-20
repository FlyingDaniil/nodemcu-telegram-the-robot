//настройки
#define btn1 5 // пин кнопки
#define id "437751818" //id чата
const char* ssid = "ssid"; // имя сети
const char* password = "password"; // пароль сети
const char BotToken[] = "token:token"; //токен бота

//библиотеки
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

//соединение и бот
WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

//первый запуск
void setup() {
  
  Serial.begin(9600);
  
  wifiConnect();
  bot.begin(); 
    
  pinMode(btn1, INPUT); 
}

//основной цикл
void loop() { 
  
  if (digitalRead(btn1)==HIGH){ //если кнопка нажата
    bot.sendMessage(id, "Button pressed"); //отправить сообщение
  }
  
}

//функция для подключения к wifi
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

