#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 100, 152);
IPAddress myDns(192, 168, 100, 1);

#include <PubSubClient.h>

EthernetClient Netw_client;
PubSubClient client(Netw_client);

const char* mqtt_server = "craft-projects.com";
const char* mqtt_client = "Sergg_MEGA";

#define mqtt_user "highlysecure"
#define mqtt_password "N4xnpPTru43T8Lmk"


//========= Temper ======================

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//=======================================

//=============== Relays=================

const int pin_relay_1 = 4;

//=======================================

//=========== Steper ====================

#include <Stepper.h>

const int stepsPerRevolution = 48;  // change this to fit the number of steps per revolution
int stepCount = 0;         // number of steps the motor has taken

const int in_1 = 5; // Driver motor in_1
const int in_2 = 6; // Driver motor in_2
const int in_3 = 7; // Driver motor in_3
const int in_4 = 8; // Driver motor in_4

Stepper myStepper(stepsPerRevolution, in_1, in_2, in_3, in_4);

//=======================================


//============ MAKROS ===================
//=========== WithOut delay =============
//=========== TIMER =====================
#define EVERY_MS(x) \
  static uint32_t tmr;\
  bool flag = millis() - tmr >= (x);\
  if (flag) tmr += (x);\
  if (flag)
//======================================

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

// === SEND INT ["NameTopic", int var]=====
void send_message_int(char* topic, int var) {
  String msg = "";
  msg = msg + var;
  char message[58];
  msg.toCharArray(message, 58);
  //  Serial.println(message);
  client.publish(topic, message);
}
//=============================================

//============ Callback =======================
/*
   Получение контроллером сообщений
*/
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == 's' && (char)payload[1] == 'y' && (char)payload[2] == 'n' && (char)payload[3] == 'c') {
    client.publish("Sergg_MEGA_2560/Online", "online");
  }
  if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {
    digitalWrite(pin_relay_1, HIGH);
  }
  if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {
    digitalWrite(pin_relay_1, LOW);
  }
  if ((char)payload[0] == '+') {

    myStepper.step(1);
    stepCount++;

    send_message_int("Sergg_MEGA_2560/Steper/0/stepCount", stepCount);
    client.publish("Sergg_MEGA_2560/Steper/0/Cmd", "/");
  }
  if ((char)payload[0] == '-') {

    myStepper.step(-1);
    stepCount--;

    send_message_int("Sergg_MEGA_2560/Steper/0/stepCount", stepCount);
    client.publish("Sergg_MEGA_2560/Steper/0/Cmd", "/");
  }
  if ((char)payload[0] == '/') {
    digitalWrite(in_1, LOW);
    digitalWrite(in_2, LOW);
    digitalWrite(in_3, LOW);
    digitalWrite(in_4, LOW);

    send_message_int("Sergg_MEGA_2560/Steper/0/stepCount", stepCount);
  }
  if ((char)payload[0] == '*') {
    digitalWrite(in_1, LOW);
    digitalWrite(in_2, LOW);
    digitalWrite(in_3, LOW);
    digitalWrite(in_4, LOW);
    stepCount = 0;
    send_message_int("Sergg_MEGA_2560/Steper/0/stepCount", stepCount);
  }

}
//==============================================

//==============================================


void setup() {
  myStepper.setSpeed(10);

  Serial.begin(115200);

  sensors.begin();

  Ethernet.begin(mac, ip, myDns);
  Serial.println(Ethernet.localIP());

  client.setServer(mqtt_server, 1883);
  client.connect(mqtt_client, mqtt_user, mqtt_password);
  client.setCallback(callback);

  client.subscribe("Sergg_MEGA_2560/Steper/0/Cmd");
  client.publish("Sergg_MEGA_2560/Steper/0/Cmd", "*");

  client.subscribe("Sergg_MEGA_2560/Relay/0");
  client.publish("Sergg_MEGA_2560/Relay/0", "off");

  client.subscribe("Sergg_MEGA_2560/Online");
  client.publish("Sergg_MEGA_2560/Online", "online");

  client.publish("Sergg_MEGA_2560/Steper/0/Temper_0", "-1");

  //================= Relay OUT ==================

  pinMode(pin_relay_1, OUTPUT);
  digitalWrite(pin_relay_1, LOW);

  //==============================================

  pinMode(in_1, OUTPUT);
  pinMode(in_2, OUTPUT);
  pinMode(in_3, OUTPUT);
  pinMode(in_4, OUTPUT);

  digitalWrite(in_1, LOW);
  digitalWrite(in_2, LOW);
  digitalWrite(in_3, LOW);
  digitalWrite(in_4, LOW);

}

void loop() {

  // client.loop();
  sensors.requestTemperatures();

  EVERY_MS(1000) {

    send_message_float("Sergg_MEGA_2560/Steper/0/Temper_0", sensors.getTempCByIndex(0));

    send_message_int("Sergg_MEGA_2560/A0", analogRead(A0));
    send_message_int("Sergg_MEGA_2560/A1", analogRead(A1));
    send_message_int("Sergg_MEGA_2560/A2", analogRead(A2));
    send_message_int("Sergg_MEGA_2560/A3", analogRead(A3));
    send_message_int("Sergg_MEGA_2560/A4", analogRead(A4));
    send_message_int("Sergg_MEGA_2560/A5", analogRead(A5));
    send_message_int("Sergg_MEGA_2560/A6", analogRead(A6));
    send_message_int("Sergg_MEGA_2560/A7", analogRead(A7));

    send_message_int("Sergg_MEGA_2560/in_1", digitalRead(in_1));
    send_message_int("Sergg_MEGA_2560/in_2", digitalRead(in_2));
    send_message_int("Sergg_MEGA_2560/in_3", digitalRead(in_3));
    send_message_int("Sergg_MEGA_2560/in_4", digitalRead(in_4));

  }
  client.loop();

}
