//Настройки
const char* ssid = "aaa";
const char* password = "yyy";
const char BotToken[] = "zzz";

#define WET 16  // Зеленый светодиод на пин D0
#define DRY 2   // Красный светодиод на пин D4
#define sense_Pin 0 // Датчик влажности почвы на пин A0

#define waterMax 50 // (0/100) Порог сухости. Чем больше значение, тем меньше растение надо поливать 
//Настройки

//библиотеки
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

//рабочие переменные
unsigned long lastUpdate = 0;
byte value = 0;

//подключение бота
WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

//стартовая настройка
void setup() {

  Serial.begin(9600);
  wifiConnect();
  bot.begin();

  pinMode(WET, OUTPUT);
  pinMode(DRY, OUTPUT);
  delay(1000);

  value = analogRead(sense_Pin) / 10;
}

//основной цикл
void loop() {
  updateValue();
  leds();
  chat();
}

//функция для подключения к wifi
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

//функция для общения с пользователем
void chat() {
  message m = bot.getUpdates();

  if (m.text.equals("Plant")) {
    if (value < waterMax) {
      bot.sendMessage(m.chat_id, "Plant is ok");
    } else {
      bot.sendMessage(m.chat_id, "Plant is not ok");
    }
  }
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

