#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

#define LED 0 //D3
 
const char* ssid = "xxx";
const char* password = "xxx";
const char BotToken[] = "xxx";

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
    
  pinMode(LED, OUTPUT); 
}

void loop() {
  message m = bot.getUpdates(); 
  
  if (m.text.equals("on")){  
    digitalWrite(LED, HIGH);   
    bot.sendMessage(m.chat_id, "The Led 1 is now ON");  
    
  }else if (m.text.equals("off")){  
    digitalWrite(LED, LOW);   
    bot.sendMessage(m.chat_id, "The Led 1 is now OFF"); 
     
  }
}
