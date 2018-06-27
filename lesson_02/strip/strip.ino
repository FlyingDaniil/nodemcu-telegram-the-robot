#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

#define stripPin 14 //D5

const char* ssid = "xxx";
const char* password = "yyy";
const char BotToken[] = "zzz";

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
    
  pinMode(stripPin, OUTPUT); 

}

void loop() {

  message m = bot.getUpdates(); 
  
  if (m.text.equals("stripon")){  
    digitalWrite(stripPin, HIGH);   
    bot.sendMessage(m.chat_id, "The Led 1 is now ON");  
    
  }else if (m.text.equals("stripoff")){  
    digitalWrite(stripPin, LOW);   
    bot.sendMessage(m.chat_id, "The Led 1 is now OFF");  
  }
    
}

