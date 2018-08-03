//wifi, bot-token
#include "secrets.h"

//настройки
#define deltatime +1 //постройка времени (+1 час для Минска\Москвы)
#define waterMax 50 // (0/100) Порог сухости. Чем больше значение, тем меньше растение надо поливать 

//пины
#define RELAY 16 //D0
#define red   0 //D3
#define green 2 //D4
#define blue  14 //D5
#define DS18B21 12  //D6
#define WET  13 // Зеленый светодиод на пин D7
#define DRY  15 // Красный светодиод на пин D8
#define sense_Pin 0 // Датчик влажности почвы на пин A0
#define pirPin 10 //SD3


#include <OneWire.h>
#include <DallasTemperature.h>
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
#include <SFE_BMP180.h>

#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "ca.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)
unsigned long lastLcdUpdate = -30000;
unsigned long lastMillis;
String date;
String t;
String newMsg;
String msg;
String lastTime;
String lastDate;
String timeOn;
const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"} ;
unsigned long lastUpdate = 0;
byte value = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);
SFE_BMP180 pressure;
OneWire oneWire(DS18B21);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer1 = {0x28, 0xFF, 0x4E, 0x13, 0x50, 0x17, 0x04, 0x2B}; 
DeviceAddress Thermometer2 = {0x28, 0xFF, 0x1B, 0x14, 0x50, 0x17, 0x04, 0xEA}; 

void setup () {
  sensors.begin();
  sensors.setResolution(Thermometer1, 10);
  sensors.setResolution(Thermometer1, 20);
  Serial.begin(9600);
  wifiConnect();
  lcdSetUp();
  timeClient.begin();
  delay(1000);
  bot.begin();
  pressure.begin();
  getTime();
  timeOn = date+" "+t;
  pinMode(pirPin,  INPUT);
  pinMode(WET,    OUTPUT);
  pinMode(DRY,    OUTPUT);
  pinMode(red,    OUTPUT); 
  pinMode(green,  OUTPUT); 
  pinMode(blue,   OUTPUT); 
  pinMode(RELAY,  OUTPUT);
  digitalWrite(RELAY, LOW);
  Serial.println("Started");

  value = analogRead(sense_Pin) / 10;
}

void loop() {
  updateLcdTime();
  chat();
  updateValue();
  leds();
  pirCheck();
}

void chat() {
  message m = bot.getUpdates();
  lcd.setCursor(0, 1);
  if (m.text.startsWith("Setnote")) {
    msg = m.text;
    msg.replace("Setnote ", " ");
    msg.trim();
    lcd.print(msg);
    bot.sendMessage(m.chat_id, "Setted");
  }
  if (m.text.equals("Deletenote")) {
    lcd.print("                  ");
    bot.sendMessage(m.chat_id, "Deleted");
  }
  if (m.text.equals("Relayon")) {
    digitalWrite(RELAY, HIGH);
    bot.sendMessage(m.chat_id, "Relay is on");
  } 
  if (m.text.equals("Relayoff")) {
    digitalWrite(RELAY, LOW);
    bot.sendMessage(m.chat_id, "Relay is off");
  } 
  if (m.text.equals("Pressure")){   
    //то отправляем данные  
    bot.sendMessage(m.chat_id, String(round(getPressure()))+" Pa");  
  }
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
     
  }else if (m.text.equals("Off")){  
    
    digitalWrite(red,    LOW); 
    digitalWrite(green,  LOW); 
    digitalWrite(blue,   LOW); 
    bot.sendMessage(m.chat_id, "Strip off");  
     
  }
  if (m.text.equals("Temp1")){
    sensors.requestTemperatures();
    bot.sendMessage(m.chat_id, String(printTemperature(Thermometer1))); 
  } 
  else if (m.text.equals("Temp2")){
    sensors.requestTemperatures();
    bot.sendMessage(m.chat_id, String(printTemperature(Thermometer2))); 
  }

  if (m.text.equals("Plant")) {
    if (value < waterMax) {
      bot.sendMessage(m.chat_id, "Plant is ok");
    } else {
      bot.sendMessage(m.chat_id, "Plant is not ok");
    }
  }

  if (m.text.equals("Last")){   
    bot.sendMessage(m.chat_id, newMsg);  
  } 
  if (m.text.equals("Timeon")){   
    bot.sendMessage(m.chat_id, timeOn);  
  }
}   

void getTime() {
  date = ""; t = "";
  timeClient.update();
  unsigned long epochTime =  timeClient.getEpochTime();
  time_t local, utc;
  utc = epochTime;
  TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, +(deltatime * 60 + 60)}; //UTC - 5 hours - change this as needed
  TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, +(deltatime * 60)}; //UTC - 6 hours - change this as needed
  Timezone usEastern(usEDT, usEST);
  local = usEastern.toLocal(utc);
  date += months[month(local) - 1];
  date += " ";
  date += day(local);
  t += hour(local);
  t += ":";
  if (minute(local) < 10) // add a zero if minute is under 10
    t += "0";
  t += minute(local);
  t += " ";
  Serial.println(date);
  Serial.println(t);
}

void wifiConnect() {
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi at ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void updateLcdTime() {
  if (millis() - 60000 > lastLcdUpdate) {
    lcd.setCursor(0, 0);
    getTime();
    lcd.print(date + " " + t);
    lastLcdUpdate = millis();
  }
}

void lcdSetUp() {
  lcd.begin(16, 2);
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

//функция для получения давления
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

float printTemperature(DeviceAddress deviceAddress) {
  return sensors.getTempC(deviceAddress);
}

//функция отвечает за светодиодную индикацию
void leds() {
  if (value < waterMax) {
    digitalWrite(WET, HIGH);
    digitalWrite(DRY, LOW);
  } else {
    digitalWrite(DRY, HIGH);
    digitalWrite(WET, LOW);
  }
}

//функция обновляющая значение влажности раз в 0.5 секуды
void updateValue() {
  if (millis() - lastUpdate > 500) {
    value = analogRead(sense_Pin) / 10;
    lastUpdate = millis();
  }
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
