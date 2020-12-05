#include <ESP8266WiFi.h>
#include <MQTT.h>

#define AP_SSID     "TechHome"
#define AP_PASSWORD "12345678"  

#define EIOTCLOUD_USERNAME "highlysecure"
#define EIOTCLOUD_PASSWORD "N4xnpPTru43T8Lmk"

// создаем объект MQTT:
#define EIOT_CLOUD_ADDRESS "178.124.145.121"

#define DO_TOPIC        "/Sensor.Parameter1"

#define PIN_DO_1         D0  // 1-ый выходной цифровой контакт 
#define MODULE_ID_1     1


#define PIN_DO_2         D1  // 2-ой выходной цифровой контакт
#define MODULE_ID_2     2


#define PIN_DO_3         D2  // 3-ий выходной цифровой контакт
#define MODULE_ID_3     3


#define PIN_DO_4         D3  // 4-ый выходной цифровой контакт
#define MODULE_ID_4     4


MQTT myMqtt("", EIOT_CLOUD_ADDRESS, 1883);


void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);  
  WiFi.begin(AP_SSID, AP_PASSWORD);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");  //  "Подключение к "
  Serial.println(AP_SSID);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };

  Serial.println("WiFi connected");  //  "WiFi подключен"
  Serial.println("Connecting to MQTT server");  
             //  "Подключение к MQTT-серверу"

  // задаем ID клиента;
  // генерируем название клиента на основе MAC-адреса
  // и последних 8 бит счетчика микросекунд:
  String clientName;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);
  myMqtt.setClientId((char*) clientName.c_str());

  Serial.print("MQTT client id:");  //  "ID клиента MQTT"

  Serial.println(clientName);

  // настраиваем функции обратного вызова:
  myMqtt.onConnected(myConnectedCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);
  
  //////Serial.println("connect mqtt...");
                   //  "Подключаемся к MQTT..."
  myMqtt.setUserPwd(EIOTCLOUD_USERNAME, EIOTCLOUD_PASSWORD);  
  myMqtt.connect();

  delay(500);

  pinMode(PIN_DO_1, OUTPUT); 
  pinMode(PIN_DO_2, OUTPUT); 
  pinMode(PIN_DO_3, OUTPUT); 
  pinMode(PIN_DO_4, OUTPUT); 

  subscribe();
}

void loop() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


void subscribe()
{
    myMqtt.subscribe("/" + String(MODULE_ID_1) + DO_TOPIC); //DO 1
    myMqtt.subscribe("/" + String(MODULE_ID_2) + DO_TOPIC); //DO 2
    myMqtt.subscribe("/" + String(MODULE_ID_3) + DO_TOPIC); //DO 3
    myMqtt.subscribe("/" + String(MODULE_ID_4) + DO_TOPIC); //DO 4
}


void myConnectedCb() {
  Serial.println("connected to MQTT server");
             //  "Подключились к MQTT-серверу"
  subscribe();
}

void myDisconnectedCb() {
  Serial.println("disconnected. try to reconnect...");
             //  "Произошло отключение. Пробуем переподключиться..."
  delay(500);
  myMqtt.connect();
}

void myPublishedCb() {
  Serial.println("published.");  //  "Опубликовано."
}

void myDataCb(String& topic, String& data) {  
  if (topic == String("/"+String(MODULE_ID_1)+ DO_TOPIC))
  {
    if (data == String("1"))
      digitalWrite(PIN_DO_1, HIGH);     
    else
      digitalWrite(PIN_DO_1, LOW);

    Serial.print("Do 1:");
    Serial.println(data);
  }


  if (topic == String("/"+String(MODULE_ID_2)+ DO_TOPIC))
  {
    if (data == String("1"))
      digitalWrite(PIN_DO_2, HIGH);     
    else
      digitalWrite(PIN_DO_2, LOW);

    Serial.print("Do 2:");
    Serial.println(data);
  }


  if (topic == String("/"+String(MODULE_ID_3)+ DO_TOPIC))
  {
    if (data == String("1"))
      digitalWrite(PIN_DO_3, HIGH);     
    else
      digitalWrite(PIN_DO_3, LOW);

    Serial.print("Do 3:");
    Serial.println(data);
  }

  if (topic == String("/"+String(MODULE_ID_4)+ DO_TOPIC))
  {
    if (data == String("1"))
      digitalWrite(PIN_DO_4, HIGH);     
    else
      digitalWrite(PIN_DO_4, LOW);

    Serial.print("Do 4:");
    Serial.println(data);
  }
  
}
