// Светодиод подлкючен к 5 пину
// Датчик температуры ds18b20 к 2 пину

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char *ssid = "TechHome"; // Имя вайфай точки доступа
const char *pass = "0123456789"; // Пароль от точки доступа

const char *mqtt_server = "178.124.145.121"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "highlysecure"; // Логи от сервер
const char *mqtt_pass = "N4xnpPTru43T8Lmk"; // Пароль от сервера

#define BUFFER_SIZE 100

bool LedState = false;
int tm = 300;
float temp = 0;

// Функция получения данных от сервера

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print(pub.topic()); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print(pub.payload_string()); // выводим в сериал порт значение полученных данных

  String payload = pub.payload_string();

  if (String(pub.topic()) == "test/led") // проверяем из нужного ли нам топика пришли данные
  {
    int stled = payload.toInt(); // преобразуем полученные данные в тип integer
    digitalWrite(5, stled); // включаем или выключаем светодиод в зависимоти от полученных значений данных
  }
}

WiFiClient wclient;
PubSubClient client(wclient, mqtt_server, mqtt_port);

void setup() {

  sensors.begin();
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  pinMode(5, OUTPUT);
}

void loop() {
  // подключаемся к wi-fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server");
      if (client.connect(MQTT::Connect("arduinoClient2")
                         .set_auth(mqtt_user, mqtt_pass))) {
        Serial.println("Connected to MQTT server");
        client.set_callback(callback);
        client.subscribe("test/led"); // подписывааемся по топик с данными для светодиода
      } else {
        Serial.println("Could not connect to MQTT server");
      }
    }

    if (client.connected()) {
      client.loop();
      TempSend();
    }

  }
} // конец основного цикла

// Функция отправки показаний с термодатчика
void TempSend() {
  if (tm == 0)
  {
    sensors.requestTemperatures(); // от датчика получаем значение температуры
    float temp = sensors.getTempCByIndex(0);
    client.publish("test/temp", String(temp)); // отправляем в топик для термодатчика значение температуры
    Serial.println(temp);
    tm = 300; // пауза меду отправками значений температуры коло 3 секунд
  }
  tm--;
  delay(10);
}
