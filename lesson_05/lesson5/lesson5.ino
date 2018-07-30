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
#include <LiquidCrystal_I2C.h>

//PINS
#define work 10 //SD3

#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "ca.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)

const char* ssid = "xxx";
const char* password = "yyy";
const char BotToken[] = "zzz";
unsigned long lastLcdUpdate = -30000;
String date;
String t;
String newMsg;
String msg;
const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"} ;

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

void setup (){
  pinMode(work, OUTPUT);
  digitalWrite(work, LOW);
  Serial.begin(9600); 
  
  wifiConnect();
  lcdSetUp();
  timeClient.begin();
  
  delay(1000);
  
  bot.begin(); 

  Serial.println("Good");
  digitalWrite(work, HIGH);
}

void loop(){
  updateLcdTime();
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

void chat(){
  message m = bot.getUpdates();
  lcd.setCursor(0,1);
  if (m.text.startsWith("setNote")){  
    msg = m.text;
    msg.replace("setNote ", " ");
    msg.trim();
    lcd.print(msg); 
    bot.sendMessage(m.chat_id, "setted");  
  } else if (m.text.equals("deleteNote")){  
    lcd.print("                  "); 
    bot.sendMessage(m.chat_id, "deleted");  
  }
}

void updateLcdTime(){
  if (millis() - 60000 > lastLcdUpdate){
    lcd.setCursor(0,0);
    getTime();
    lcd.print(date+" "+t);
    lastLcdUpdate = millis();
  }
}

void lcdSetUp(){
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("WIFI CONNECTED");
  lcd.setCursor(0, 1);      
  lcd.print(WiFi.localIP());
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Developed by");
  lcd.setCursor(0, 1);      
  lcd.print("t.me/arduino_io");
  delay(2000);
  lcd.clear();
}



