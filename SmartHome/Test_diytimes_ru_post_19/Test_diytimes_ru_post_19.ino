#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "TechHome"; // Имя роутера
const char *pass = "12345678"; // Пароль роутера

const char *mqtt_server = "178.124.145.121"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "highlysecure"; // Логи для подключения к серверу MQTT
const char *mqtt_pass = "N4xnpPTru43T8Lmk"; // Пароль для подключения к серверу MQTT

const int led = 5; // диод на плате

#define BUFFER_SIZE 100

int tm = 300;
float temp = 0;

WiFiClient wclient; 
PubSubClient client(wclient, mqtt_server, mqtt_port);

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();

  pinMode(led, OUTPUT);
}

// Функция отправки показаний
void refreshData() {
  if (tm == 0) {
    time = millis();
    client.publish("test/1", time);
    tm = 3000; // пауза меду отправками 3 секунды
  }
  tm--;
  
  delay(1); 
}

// Функция получения данных от сервера
void callback(const MQTT::Publish& pub)
{
  String payload = pub.payload_string();
  String topic = pub.topic();
  
  Serial.print(pub.topic()); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.println(payload); // выводим в сериал порт значение полученных данных

  // проверяем из нужного ли нам топика пришли данные 
  if(topic == "test/2")
  {
     Serial.println("test/2 OK"); // выводим в сериал порт подтверждение, что мы получили топик test/2
  }
}

void loop() {
  // подключаемся к wi-fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);
    
    if (WiFi.waitForConnectResult() != WL_CONNECTED) return;
    Serial.println("WiFi connected");
  }
  
  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.print("Connecting to MQTT server ");
      Serial.print(mqtt_server);
      Serial.println("...");
      if (client.connect(MQTT::Connect("arduinoClient2").set_auth(mqtt_user, mqtt_pass))) {
        Serial.println("Connected to MQTT server ");
        client.set_callback(callback);
        // подписываемся под топики
        client.subscribe("test/1");
        client.subscribe("test/2");
      } else {
        Serial.println("Could not connect to MQTT server"); 
      }
    }
    
    if (client.connected()){
      client.loop();
      refreshData();
    }
  
  }
}
