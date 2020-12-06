#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>


const char* ssid = "Rostelecom_Inactivated"; //Название сети
const char* password = "29974978";           //пароль
const char* mqtt_server = "192.168.1.10";    //ip брокера mqtt

#define RELAY_1 5                            //Выход на реле или светодиод
#define BUTTON_1 4                           //Кнопка
#define relays_topic "ESP04/relay_1"         //публикуемый топик
Ultrasonic ultrasonic(13, 12);               //пины дальнометра 12,echo 13,trig

WiFiClient espClient;                        //инициализация WiFi клиента
PubSubClient client(espClient);              //инициализация MQTT клиента
long last_mls = millis();                    //функция времени в милисекундах
long last_mls2 = millis();
long last_mls3 = millis();
boolean Status = true;                      //объявляем статус реле в 0
boolean btnPress = false;                    //объявляем что кнопка не нажата 0
boolean lastbtnStat = false;                 //временная переменная для хранения статуса
int regim = 1;                               //режим по умолчанию при включении
int lastdist_cm = 40;                        //расстояние до объекта

void setup()                                 //Выполняется при запуске 1 раз
{
  pinMode(RELAY_1, OUTPUT);                  //Выход на реле или светодиод
  pinMode(BUTTON_1, INPUT);                  //Кнопка
  digitalWrite(RELAY_1, Status);             //устанавливаем на выходе статус реле
  Serial.begin(115200);                      //инициализация монитора порта
  client.setServer(mqtt_server, 1883);       //подключаемся к MQTT
  client.setCallback(callback);              //функция получения топиков с брокера
  delay(100);                                //ждем 100 милисекунд
  WiFi.begin(ssid, password);                //подключаемся к WiFi
  delay(2000);                               //ждем 2 секунд
  client.connect("ESP8266Client");           //конектимся с брокером как клиент
  client.subscribe(relays_topic);            //подписываемся на топик
}


void ultra_son()                             //функция чтения с датчика расстояния
{
  float dist_cm = ultrasonic.Ranging(CM);
  Serial.print(ultrasonic.Ranging(CM));
  Serial.println("cm");
  if (dist_cm < lastdist_cm)                 //если растояние меньше указанного
  { //то включем свет
    Status = false;
    digitalWrite(RELAY_1, Status);
  } else                                     //если нет то выключаем
  {
    Status = true;
    digitalWrite(RELAY_1, Status);
  }
}

void callback(char* topic, byte* payload, unsigned int length) //читаем топики
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1')
  {
    Status = true;
    regim = 1;
    digitalWrite(RELAY_1, Status);
  }
  else if ((char)payload[0] == '2')
  {
    Status = false;
    regim = 2;
    digitalWrite(RELAY_1, Status);
  } else {
    regim = 3;
  }
}

void button()                                //функция нажатия кнопки и переключение режимов
{
  Status = digitalRead(RELAY_1);
  btnPress = digitalRead(BUTTON_1);
  if (btnPress == HIGH && lastbtnStat == false)
  {
    delay(30);
    regim++;
    lastbtnStat = true;
    if (regim > 3)
    {
      regim = 1;
    }
  }
  if (btnPress == LOW && lastbtnStat == true)
  {
    lastbtnStat = false;
  }
  if (regim == 1)                            //первый режим
  {
    Status = true;
    digitalWrite(RELAY_1, Status);
  }
  if (regim == 2)                            //второй режим
  {
    Status = false;
    digitalWrite(RELAY_1, Status);
  }
  if (regim == 3)                            //третий режим (авторежим)
  {
    if (millis() - last_mls2 > 500)          //функция ultra_son раз 500 милисекунд
    {
      last_mls2 = millis();
      ultra_son();
    }
  }
}

void reconnect_server()                      //функция проверки подключения
{

  if (WiFi.status() != WL_CONNECTED)         //если нет подключения с сети
  {
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.println("WiFi connect...");       //выводим в монитор порта что пытаемся подключиться
  } else                                     //если есть подключение
  {
    Serial.println("");
    Serial.println("WiFi connected");        //выводим в монитор порта что подключились
    Serial.println("IP address: ");          //выводим в монитор порта наш IP
    Serial.println(WiFi.localIP());          //выводим в монитор порта наш IP
    Serial.println(WiFi.RSSI());             //выводим в монитор порта уровень сети

  }

  if (!client.connected() && WiFi.status() == WL_CONNECTED) //если к сети подключились но к MQTT нет
  {
    if (client.connect("ESP8266Client"))     //если с брокером уже конектились
    {
      Serial.println("Mosquitto connect...");//выводим в монитор порта что пытаемся подключиться
      client.subscribe(relays_topic);        //подписываемся на топик
    } else                                   //если не получилось
    {
      Serial.print("failed connect Mosquitto, rc="); //пишем ошибку
      Serial.print(client.state());
      Serial.println("");
    }
  }
}

void MQTT()                                   //публикуем топик с состоянием режима
{
  if (millis() - last_mls3 > 2000)            //периодичность публикации топика
  {
    last_mls3 = millis();
    client.publish(relays_topic, String(regim).c_str());
  }
}

void loop()                                  //основной цикл, выполняется посстоянно
{
  client.loop();

  if (millis() - last_mls > 5000)               //периодичность проверки подключения
  {
    last_mls = millis();
    reconnect_server();
  }

  MQTT();
  button();                                //функция кнопки и публикации на брокер состояния
}
