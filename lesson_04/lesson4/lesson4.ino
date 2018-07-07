#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WifiUDP.h>
#include <WiFiClientSecure.h>
#include <String.h>
#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <TelegramBot.h>

#define pirPin 15 //D8
#define work 10 //SD3
#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "ca.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)

const char* ssid = "qqq";
const char* password = "sss";
const char BotToken[] = "xxx";
unsigned long lastMillis;
String date;
String t;
String lastTime;
String lastDate;
String newMsg;
String timeOn;
const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"} ;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

void setup (){
  pinMode(work, OUTPUT);
  digitalWrite(work, LOW);
  
  Serial.begin(9600); 
  
  wifiConnect();
  
  timeClient.begin();   // Start the NTP UDP client
  delay(1000);
  bot.begin(); 

  getTime();
  timeOn = date+" "+t;
  
  pinMode(pirPin, INPUT);
  Serial.println("Good");
  digitalWrite(work, HIGH);
}

void loop(){
  
  pirCheck();
  chat();
  
}
void getTime(){
  date = "";t = "";
  timeClient.update();
  unsigned long epochTime =  timeClient.getEpochTime();
  time_t local, utc;
  utc = epochTime;
  TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, +120};  //UTC - 5 hours - change this as needed
  TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, +60};   //UTC - 6 hours - change this as needed
  Timezone usEastern(usEDT, usEST);
  local = usEastern.toLocal(utc);
  date += months[month(local)-1];
  date += " ";
  date += day(local);
  t += hour(local);
  t += ":";
  if(minute(local) < 10)  // add a zero if minute is under 10
    t += "0";
  t += minute(local);
  t += " ";
  Serial.println(date);
  Serial.println(t);
}

void wifiConnect(){
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi at ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void pirCheck(){
    if (digitalRead(pirPin) == HIGH && millis() - lastMillis > 10000){
    lastMillis = millis();
    getTime();
    lastDate = date;
    lastTime = t;
    Serial.println(date);
    Serial.println(t);
    newMsg = "" + date + " "+t;
  }
}

void chat(){
  message m = bot.getUpdates();
  if (m.text.equals("last")){   
    bot.sendMessage(m.chat_id, newMsg);  
  } else if (m.text.equals("timeon")){   
    bot.sendMessage(m.chat_id, timeOn);  
  }
}

