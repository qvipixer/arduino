#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//============ LCD ===================
/*
  // ESP8266 with 16x2 i2c LCD
  // Compatible with the Arduino IDE 1.6.6
  // Library https://github.com/agnunez/ESP8266-I2C-LCD1602
  // Original Library https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
  // Modified for ESP8266 with GPIO0-SDA GPIO2-SCL and LCD1206 display
  // edit library and change Wire.begin() by Wire.begin(sda,scl) or other GPIO's used for I2C
  // and access from lcd.begin(sda,scl)
*/

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Check I2C address of LCD, normally 0x27 or 0x3F
//=============================================
#include "DHTesp.h"
#define DHTpin 14    //D5 of NodeMCU is GPIO14
DHTesp dht;

String inString = "";    // string to hold input

const char* ssid = "TechHome";
//const char* ssid = "TechSpace";
const char* password = "0123456789";
//const char* password = "craft-projects.com";

const char* mqtt_server = "craft-projects.com";
const char* mqtt_client = "Sergg/TechSpace/Plc/ESP8266";

#define mqtt_user "highlysecure"
#define mqtt_password "N4xnpPTru43T8Lmk"


WiFiClient espClient;
PubSubClient client(espClient);

const int LED_PIN_0 = 2;
const int LED_PIN_1 = 16;

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
int sensorValue = 0;  // value read from the pot

//============ MAKROS ===================
//=========== WithOut delay =============
//=========== TIMER =====================
#define EVERY_MS(x) \
  static uint32_t tmr;\
  bool flag = millis() - tmr >= (x);\
  if (flag) tmr += (x);\
  if (flag)
//=============================================

// === SEND FLOAT ["NameTopic", float var]=====
void send_message_float(char* topic, float var) {
  String msg = "";
  msg = msg + var;
  char message[58];
  msg.toCharArray(message, 58);
  //  Serial.println(message);
  client.publish(topic, message);
}
//=============================================

// === SEND INT ["NameTopic", int var]========
void send_message_int(char* topic, int var) {
  String msg = "";
  msg = msg + var;
  char message[58];
  msg.toCharArray(message, 58);
  //  Serial.println(message);
  client.publish(topic, message);
}
//=============================================

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client, mqtt_user, mqtt_password)) {
      Serial.println("connected");

      client.subscribe("Sergg/TechSpace/Plc/ESP8266/LCD");

      client.subscribe("Sergg/TechSpace/Plc/ESP8266/LED");
      client.subscribe("Sergg/TechSpace/Plc/ESP8266/Status");
      client.publish("Sergg/TechSpace/Plc/ESP8266/LED", "on");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(1500);
    }
  }
}

void setup() {
  // ======= LCD ==============
  lcd.begin(4, 5);     // In ESP8266-01, SDA=4 GPIO, SCL=5 GPIO
  lcd.backlight();

  lcd.home();                // At column=0, row=0
  lcd.print("ESP8266");
  lcd.setCursor(0, 1);
  lcd.print("LiquidCrystalI2C");

  //=============================================

  dht.setup(DHTpin, DHTesp::DHT11); //for DHT11 Connect DHT sensor to GPIO

  pinMode(LED_PIN_0, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  Serial.begin(115200);
  digitalWrite(LED_PIN_0, HIGH);
  digitalWrite(LED_PIN_1, HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\nMessage arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {
    digitalWrite(LED_PIN_0, LOW);
    digitalWrite(LED_PIN_1, LOW);
  }
  if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {
    digitalWrite(LED_PIN_0, HIGH);
    digitalWrite(LED_PIN_1, HIGH);
  }
  if ((char)payload[0] == 's' && (char)payload[1] == 'y' && (char)payload[2] == 'n' && (char)payload[3] == 'c') {
    client.publish("Sergg/TechSpace/Plc/ESP8266/Status", "online");
  }
  if ((char)payload[0] == 'm' && (char)payload[1] == 's' && (char)payload[2] == 'g') {
    lcd.clear();
    lcd.home();                // At column=0, row=0

    for (int i = 3; i < 19; i++) {
      lcd.setCursor(i-3, 0);
      lcd.print((char)payload[i]);
      Serial.print((char)payload[i]);
    }
    for (int i = 19; i < 35; i++) {
      lcd.setCursor(i - 19, 1);
      lcd.print((char)payload[i]);
      Serial.print((char)payload[i]);
    }
  }


  /*
    if (topic == "Sergg/TechSpace/Plc/MEGA_2560/Relay/2") {
      lcd.clear();
      lcd.home();                // At column=0, row=0
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP());
      Serial.println("444");
    }
  */
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  EVERY_MS(1000) {

    send_message_float("Sergg/TechSpace/Plc/ESP8266/Temper_0", dht.getTemperature());
    send_message_float("Sergg/TechSpace/Plc/ESP8266/Humidity_0", dht.getHumidity());

    send_message_int("Sergg/TechSpace/Plc/ESP8266/A0_Flower", analogRead(analogInPin));
  }

  client.loop();


}
