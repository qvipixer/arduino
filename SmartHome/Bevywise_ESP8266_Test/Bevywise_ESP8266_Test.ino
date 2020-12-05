#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "TechHome";
const char* password = "12345678";
const char* mqtt_server = "178.124.145.121";

#define mqtt_user "highlysecure"
#define mqtt_password "N4xnpPTru43T8Lmk"


WiFiClient espClient;
PubSubClient client(espClient);
int RECV_PIN = 5;

void reconnect() 
{
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    if(client.connect("ESP8266Client",mqtt_user, mqtt_password)) 
    {
      Serial.println("connected");
      client.subscribe("ESP8266/LED");
      client.publish("ESP8266/LED","on");      
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
  pinMode(RECV_PIN,OUTPUT);
  Serial.begin(115200);
  digitalWrite(RECV_PIN,HIGH);
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
  if((char)payload[0] == 'o' && (char)payload[1] == 'n')
  {
    digitalWrite(RECV_PIN,LOW);
    }
  }
  
void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
}
