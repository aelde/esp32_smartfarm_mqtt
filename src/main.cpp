#include "main.h"
#include "net_mqtt.h"
// constants
#define TAG             "main"
#define BTN_PIN         0
#define WIFI_SSID       "DSEL24G"
#define WIFI_PASSWORD   "piglet1234"

// #define WIFI_SSID       "TGR17_2.4G"
// #define WIFI_PASSWORD   ""

// #define MQTT_EVT_TOPIC  "tgr2023/sailormoon/btn_evt"
// #define MQTT_CMD_TOPIC  "tgr2023/sailormoon/cmd"

// #define MQTT_EVT_TOPIC  "smartfarm/control"
#define MQTT_EVT_TOPIC  "6424020002/smartfarm/control"
// #define MQTT_CMD_TOPIC  "smartfarm/status"
#define MQTT_CMD_TOPIC  "6424020002/smartfarm/status"
// #define MQTT_CMD_TOPIC  "6424020002/smartfarm/control"


#define MQTT_DEV_ID     43

// static function prototypes
void print_memory(void);
static void mqtt_callback(char* topic, byte* payload, unsigned int length);

// static variables
static bool enable_flag = true;
// declare ArduinoJson objects for cmd_buf and evt_buf
StaticJsonDocument<128> cmd_buf;
StaticJsonDocument<128> evt_buf;

// State Declaration
float temp=0, humi=0, mois=0, ligh=0;
#define INI 0
int state=INI;
#define ONCH1 5
#define ONCH2 6
#define ONCH3 7
#define ONCH4 8
#define OFFCH1 9
#define OFFCH2 10
#define OFFCH3 11
#define OFFCH4 12
#define WAITING 13

DynamicJsonDocument doc(1024);


static char buf[128];

//relay setup ____________________________________ start
#define RELAY_CH_1  4
#define RELAY_CH_2  27
#define RELAY_CH_3  16
#define RELAY_CH_4  17


void relay_setup(){
    pinMode(RELAY_CH_1, OUTPUT);
    pinMode(RELAY_CH_2, OUTPUT);
    pinMode(RELAY_CH_3, OUTPUT);
    pinMode(RELAY_CH_4, OUTPUT);
    digitalWrite(RELAY_CH_1, HIGH);
    digitalWrite(RELAY_CH_2, HIGH);
    digitalWrite(RELAY_CH_3, HIGH);
    digitalWrite(RELAY_CH_4, HIGH);
    }

//dht setup ____________________________________ start
#define DHTPIN 26    // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
// #define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
//dht setup ____________________________________ end

// Setup hardware
void setup() {
  Serial.begin(115200);
  print_memory();

  //setup relay ____________________________
  relay_setup();

  // pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  // // connect to WiFi
  // net_mqtt_init(WIFI_SSID, WIFI_PASSWORD);
  net_mqtt_init(WIFI_SSID, WIFI_PASSWORD);  
  // // connect to MQTT broker

  net_mqtt_connect(MQTT_DEV_ID,MQTT_CMD_TOPIC, mqtt_callback);
  Serial.println("Setup mqtt done...");

  //setup dht ____________________________
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  //setup dht ____________________________
}

// void dht11_loop(){
//   delay(delayMS);
//   // Get temperature event and print its value.
//   sensors_event_t event;
//   dht.temperature().getEvent(&event);
//   if (isnan(event.temperature)) {
//     Serial.println(F("Error reading temperature!"));
//   }
//   else {
//     Serial.print(F("Temperature: "));
//     Serial.print(event.temperature);
//     Serial.println(F("°C"));
//   }
//   // Get humidity event and print its value.
//   dht.humidity().getEvent(&event);
//   if (isnan(event.relative_humidity)) {
//     Serial.println(F("Error reading humidity!"));
//   }
//   else {
//     Serial.print(F("Humidity: "));
//     Serial.print(event.relative_humidity);
//     Serial.println(F("%"));
//   }
// }

// Main loop
void loop() {
  static uint32_t prev_millis = 0;

//   switch (state)
//   {
//     case INI:{
//         Serial.println("Initializing");
//         digitalWrite(RELAY_CH_1,LOW);
//         digitalWrite(RELAY_CH_2,LOW);
//         digitalWrite(RELAY_CH_3,LOW);
//         digitalWrite(RELAY_CH_4,LOW);

//         state=WAITING;
//         break;  
//     }
//     case WAITING:{
//         Serial.println("Waiting...");
//         break;  
//     }
//     case ONCH1:{
//         Serial.println("CH1 On");
//         digitalWrite(RELAY_CH_1,HIGH);
//         state=WAITING;
//         break;  
//     }
//     case ONCH2:{
//         Serial.println("CH2 On");
//         digitalWrite(RELAY_CH_2,HIGH);
//         state=WAITING;
//         break;  
//     }
//     case ONCH3:{
//         Serial.println("CH3 On");
//         digitalWrite(RELAY_CH_3,HIGH);
//         state=WAITING;
//         break;  
//     }
//     case ONCH4:{
//         Serial.println("CH4 On");
//         digitalWrite(RELAY_CH_4,HIGH);
//         state=WAITING;
//         break;  
//     }
//     case OFFCH1:{
//         Serial.println("CH1 Off");
//         digitalWrite(RELAY_CH_1,LOW);
//         state=WAITING;
//         break;  
//     }
//     case OFFCH2:{
//         Serial.println("CH2 Off");
//         digitalWrite(RELAY_CH_2,LOW);
//         state=WAITING;
//         break;  
//     }
//     case OFFCH3:{
//         Serial.println("CH3 Off");
//         digitalWrite(RELAY_CH_3,LOW);
//         state=WAITING;
//         break;  
//     }
//     case OFFCH4:{
//         Serial.println("CH4 Off");
//         digitalWrite(RELAY_CH_4,LOW);
//         state=WAITING;
//         break;  
//     }
//   }

//   dht11_loop();

// Get temperature event and print its value.
    delay(delayMS);
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    }
    else {
    Serial.print(F("Temperature: "));
    temp=event.temperature;
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    }
    else {
    Serial.print(F("Humidity: "));
    humi=event.relative_humidity;
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    }


  evt_buf["id"] = "001";
  evt_buf["humi"] = humi;
  ESP_LOGI(TAG, "Total heap: %u", humi);
  evt_buf["temp"] = temp;
  ESP_LOGI(TAG, "Total heap: %u", temp);
//   evt_buf["temp"] = event.temperature;
  evt_buf["mois"] = "50";
  evt_buf["ligh"] = "50";
  // evt_buf["ch1"] = "off";
  // evt_buf["ch2"] = "off";
  // evt_buf["ch3"] = "off";
  // evt_buf["ch4"] = "";
  serializeJson(evt_buf,buf);
  net_mqtt_publish(MQTT_EVT_TOPIC,buf);
    
  net_mqtt_loop();
  delay(100);
  }

// Print memory information
void print_memory() {
  ESP_LOGI(TAG, "Total heap: %u", ESP.getHeapSize());
  ESP_LOGI(TAG, "Free heap: %u", ESP.getFreeHeap());
  ESP_LOGI(TAG, "Total PSRAM: %u", ESP.getPsramSize());
  ESP_LOGI(TAG, "Free PSRAM: %d", ESP.getFreePsram());
}


// callback function to handle MQTT message
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
// String msg;
//   String key = "cmd";
  ESP_LOGI(TAG, "Message arrived on topic %s", topic);
  ESP_LOGI(TAG, "Payload: %.*s", length, payload);
  // ESP_LOGI(TAG, "Payload: %s", payload);
  // msg = String((char *)payload).substring(2,length);
//   msg = String((char *)payload).substring(7,length-2);
  // msg = String((char *)payload).substring(0,length);
  // msg = msg.substring(msg.indexOf(key)+5,msg.indexOf(key)+11);  

//   ESP_LOGI(TAG, "Payload: %s", msg.c_str());

    //doc test
    deserializeJson(doc, payload);

    int ch1Value = doc["ch1"];
    Serial.print("doc: ");
    Serial.println(ch1Value);
    
    if (doc["ch1"]==1)   { state=ONCH1;  Serial.println(" CH1 On"); digitalWrite(RELAY_CH_1,LOW);}
    if (doc["ch1"]==0)  { state=OFFCH1; Serial.println(" CH1 Off"); digitalWrite(RELAY_CH_1,HIGH);}
    if (doc["ch2"]==1)   { state=ONCH2;  Serial.println(" CH2 On"); digitalWrite(RELAY_CH_2,LOW);}
    if (doc["ch2"]==0)  { state=OFFCH2; Serial.println(" CH2 Off"); digitalWrite(RELAY_CH_2,HIGH);}
    if (doc["ch3"]==1)   { state=ONCH3;  Serial.println(" CH3 On"); digitalWrite(RELAY_CH_3,LOW);}
    if (doc["ch3"]==0)  { state=OFFCH3; Serial.println(" CH3 Off"); digitalWrite(RELAY_CH_3,HIGH);}
    if (doc["ch4"]==1)   { state=ONCH4;  Serial.println(" CH4 On"); digitalWrite(RELAY_CH_4,LOW);}
    if (doc["ch4"]==0)  { state=OFFCH4; Serial.println(" CH4 Off"); digitalWrite(RELAY_CH_4,HIGH);}
    Serial.println("-----------------------------");
    // Serial.println("CH1: "+String(doc["ch1"]));

  // ESP_LOGI(TAG, "Payload: %s", length);
  // ESP_LOGI(TAG, "Payload: %.*s", length, );
  // Serial.println("Message arrived on topic"+ String(topic));
  // Serial.println("Payload: %.*s", length, payload);
  // extract data from payload
  char tmpbuf[128];
  memcpy(tmpbuf,payload,length);
  tmpbuf[length] = 0;
  // check if the message is for this device
  deserializeJson(cmd_buf,tmpbuf);
  // configure enable/disable status
 if(cmd_buf["ID"] == MQTT_DEV_ID){
    enable_flag = cmd_buf["enable"];
 }}