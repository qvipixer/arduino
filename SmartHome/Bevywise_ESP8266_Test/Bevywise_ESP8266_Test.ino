#include <ESP8266WiFi.h>
#include <PubSubClient.h>

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



String inString = "";    // string to hold input

const char* ssid = "TechHome";
//const char* ssid = "TechSpace";
const char* password = "0123456789";
//const char* password = "11111111";

const char* mqtt_server = "craft-projects.com";
const char* mqtt_client = "Sergg_ESP";

#define mqtt_user "highlysecure"
#define mqtt_password "N4xnpPTru43T8Lmk"


WiFiClient espClient;
PubSubClient client(espClient);

const int LED_PIN_0 = 2;
const int LED_PIN_1 = 16;

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
int sensorValue = 0;  // value read from the pot

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client, mqtt_user, mqtt_password))
    {
      Serial.println("connected");
      client.subscribe("ESP8266/LED");
      client.subscribe("ESP8266/A0");
      client.subscribe("Online");
      client.publish("ESP8266/LED", "on");
      client.publish("ESP8266/A0", "-1");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(1500);
    }
  }
}

void setup()
{
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

void setup_wifi()
{
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == 'o' && (char)payload[1] == 'n')
  {
    digitalWrite(LED_PIN_0, LOW);
    digitalWrite(LED_PIN_1, LOW);
  }
  if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f')
  {
    digitalWrite(LED_PIN_0, HIGH);
    digitalWrite(LED_PIN_1, HIGH);
  }
  if ((char)payload[0] == 's' && (char)payload[1] == 'y' && (char)payload[2] == 'n' && (char)payload[3] == 'c') {
    client.publish("Online", "online");
  }
}

void loop()
{
  if (!client.connected())  {
    reconnect();
  }
  client.loop();

  EVERY_MS(1000) {
    int sensorValue = analogRead(analogInPin);
    String msg = "";
    msg = msg + sensorValue;
    char message[58];
    msg.toCharArray(message, 58);
    Serial.println(message);
    client.publish("ESP8266/A0", message);
  }

}
