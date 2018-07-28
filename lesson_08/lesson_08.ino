//настройки пинов
#define red   5 //D1
#define green 4 //D2
#define blue  0 //D3
//имя сети, пароль, токен бота лежат в этом файле (должен находиться в одной папке)
#include "secrets.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

void setup() {
  
  Serial.begin(9600);
  wifiConnect();
  bot.begin(); 

  //конфигурируем пины как цифровые выходы
  pinMode(red,   OUTPUT); 
  pinMode(green, OUTPUT); 
  pinMode(blue,  OUTPUT); 
}

void loop() {
  //получаем сообщение
  message m = bot.getUpdates(); 

  //включаем нужный цвет
  if (m.text.equals("Red")){  
    
    digitalWrite(red,    HIGH); 
    digitalWrite(green,  LOW); 
    digitalWrite(blue,   LOW); 
    bot.sendMessage(m.chat_id, "Red strip");  
    
  }else if (m.text.equals("Green")){  
    
    digitalWrite(red,    LOW); 
    digitalWrite(green,  HIGH); 
    digitalWrite(blue,   LOW); 
    bot.sendMessage(m.chat_id, "Green strip");  
     
  }else if (m.text.equals("Blue")){  
    
    digitalWrite(red,    LOW); 
    digitalWrite(green,  LOW); 
    digitalWrite(blue,   HIGH); 
    bot.sendMessage(m.chat_id, "Blue strip");  
     
  }else if (m.text.equals("Purple")){  
    
    digitalWrite(red,    HIGH); 
    digitalWrite(green,  LOW); 
    digitalWrite(blue,   HIGH); 
    bot.sendMessage(m.chat_id, "Purple strip");  
     
  }else if (m.text.equals("Off")){  
    
    digitalWrite(red,    LOW); 
    digitalWrite(green,  LOW); 
    digitalWrite(blue,   LOW); 
    bot.sendMessage(m.chat_id, "Strip off");  
     
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

