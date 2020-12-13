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

//=========== Steper=====================

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

const int in_1 = 8; // Driver motor in_1
const int in_2 = 9; // Driver motor in_2
const int in_3 = 10; // Driver motor in_3
const int in_4 = 11; // Driver motor in_4

Stepper myStepper(stepsPerRevolution, in_1, in_2, in_3, in_4);

//=======================================


//============ MAKROS ===================
//=========== WithOut delay==============
//=========== TIMER =====================
#define EVERY_MS(x) \
  static uint32_t tmr;\
  bool flag = millis() - tmr >= (x);\
  if (flag) tmr += (x);\
  if (flag)
//======================================

//======================================

void setup() {

  Serial.begin(115200);
  // put your setup code here, to run once:
  sensors.begin();
  Ethernet.begin(mac, ip, myDns);
  Serial.println(Ethernet.localIP());
  client.setServer(mqtt_server, 1883);
  client.connect(mqtt_client, mqtt_user, mqtt_password);

  client.subscribe("Sergg_MEGA_2560/LED");
  client.subscribe("Sergg_MEGA_2560/A0");
  client.subscribe("Sergg_MEGA_2560/Online");
  client.publish("Sergg_MEGA_2560/Online", "on");
  client.publish("Sergg_MEGA_2560", "-1");

  pinMode(in_1, OUTPUT);
  pinMode(in_2, OUTPUT);
  pinMode(in_3, OUTPUT);
  pinMode(in_4, OUTPUT);

  digitalWrite(in_1, HIGH);
  digitalWrite(in_2, LOW);
  digitalWrite(in_3, LOW);
  digitalWrite(in_4, HIGH);

}

void loop() {
  sensors.requestTemperatures();

  EVERY_MS(1000) {
    float tempC = sensors.getTempCByIndex(0);
    String msg = "";
    msg = msg + tempC;
    char message[58];
    msg.toCharArray(message, 58);
    Serial.println(message);
    client.publish("Sergg_MEGA_2560/A0", message);

    digitalWrite(in_1, LOW);
    digitalWrite(in_2, LOW);
    digitalWrite(in_3, LOW);
    digitalWrite(in_4, LOW);
  }

  digitalWrite(in_1, LOW);
  digitalWrite(in_2, LOW);
  digitalWrite(in_3, LOW);
  digitalWrite(in_4, LOW);

}
