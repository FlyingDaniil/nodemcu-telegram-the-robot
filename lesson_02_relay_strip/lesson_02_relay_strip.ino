#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

#define STRIP 5 //D1
#define RELAY 4 //D2
 
#include "secrets.h"

WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

void setup() {
  
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  bot.begin(); 
    
  pinMode(STRIP, OUTPUT); 
  pinMode(RELAY, OUTPUT); 
}

void loop() {
  message m = bot.getUpdates(); 
  
  if (m.text.equals("Relayon")){  
    digitalWrite(RELAY, HIGH);   
    bot.sendMessage(m.chat_id, "The relay is now ON");  
    
  }else if (m.text.equals("Relayoff")){  
    digitalWrite(RELAY, LOW);   
    bot.sendMessage(m.chat_id, "The relay is now OFF"); 
     
  }else if (m.text.equals("Stripon")){  
    digitalWrite(STRIP, HIGH);   
    bot.sendMessage(m.chat_id, "The strip is now ON"); 
     
  }else if (m.text.equals("Stripoff")){  
    digitalWrite(STRIP, LOW);   
    bot.sendMessage(m.chat_id, "The strip is now OFF"); 
     
  }
}

