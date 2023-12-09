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

#define MQTT_DEV_ID     43

// static function prototypes
void print_memory(void);
static void mqtt_callback(char* topic, byte* payload, unsigned int length);

// static variables
static bool enable_flag = true;
// declare ArduinoJson objects for cmd_buf and evt_buf
StaticJsonDocument<128> cmd_buf;
StaticJsonDocument<128> evt_buf;

static char buf[128];

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
  // pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  // // connect to WiFi
  // net_mqtt_init(WIFI_SSID, WIFI_PASSWORD);
  net_mqtt_init(WIFI_SSID, WIFI_PASSWORD);  
  // // connect to MQTT broker
  // net_mqtt_connect(MQTT_DEV_ID,MQTT_CMD_TOPIC, mqtt_callback);
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

// Main loop
void loop() {
  static uint32_t prev_millis = 0;

  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
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
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }

    evt_buf["id"] = "001";
    evt_buf["humi"] = event.relative_humidity;
    evt_buf["temp"] = event.temperature;
    evt_buf["mois"] = "50";
    evt_buf["ligh"] = "50";
    evt_buf["ch1"] = "off";
    evt_buf["ch2"] = "off";
    evt_buf["ch3"] = "off";
    evt_buf["ch4"] = "";
    serializeJson(evt_buf,buf);
    net_mqtt_publish(MQTT_EVT_TOPIC,buf);
  // if (enable_flag) {
    // check button status
    // if(digitalRead(BTN_PIN)==LOW){
    //   ESP_LOGI(TAG, "Total");
    // // check button bounce
    //     if(millis()-prev_millis > 1000){
    //     // publish button event
    //         prev_millis = millis();
    //         // evt_buf["timestamp"] = millis();
    //         evt_buf["id"] = "001";
    //         evt_buf["humi"] = "100";
    //         evt_buf["temp"] = "100";
    //         evt_buf["mois"] = "100";
    //         evt_buf["ligh"] = "100";
    //         evt_buf["ch1"] = "on";
    //         evt_buf["ch2"] = "on";
    //         evt_buf["ch3"] = "on";
    //         evt_buf["ch4"] = "on";
    //         serializeJson(evt_buf,buf);
    //         net_mqtt_publish(MQTT_EVT_TOPIC,buf);

    //         // {"id":"001","humi":1,"temp":1,"mois":1,"ligh":1,"ch1":"on","ch2":"on","ch3":"on","ch4":"on"}
    //     }
    // }
  
  net_mqtt_loop();
  delay(100);
  }

 // if (enable_flag) {
  //   // check button status
  //   if(digitalRead(BTN_PIN)==LOW){
  //   // check button bounce
  //     if(millis()-prev_millis > 1000){
  //       // publish button event
  //       prev_millis = millis();
  //       evt_buf["ID"] = MQTT_DEV_ID;
  //       evt_buf["timestamp"] = millis();
  //       evt_buf["pressed"] = true;
  //       serializeJson(evt_buf,buf);
  //       net_mqtt_publish(MQTT_EVT_TOPIC,buf);
  //       }
  //   }
  // }

  // loop MQTT interval


// Print memory information
void print_memory() {
  ESP_LOGI(TAG, "Total heap: %u", ESP.getHeapSize());
  ESP_LOGI(TAG, "Free heap: %u", ESP.getFreeHeap());
  ESP_LOGI(TAG, "Total PSRAM: %u", ESP.getPsramSize());
  ESP_LOGI(TAG, "Free PSRAM: %d", ESP.getFreePsram());
}



// callback function to handle MQTT message
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  String key = "cmd";
  ESP_LOGI(TAG, "Message arrived on topic %s", topic);
  ESP_LOGI(TAG, "Payload: %.*s", length, payload);
  // ESP_LOGI(TAG, "Payload: %s", payload);
  // msg = String((char *)payload).substring(2,length);
  msg = String((char *)payload).substring(7,length-2);
  // msg = String((char *)payload).substring(0,length);
  // msg = msg.substring(msg.indexOf(key)+5,msg.indexOf(key)+11);



  // ESP_LOGI(TAG, "Payload: %s", msg.c_str());
  // ESP_LOGI(TAG, "Payload: %s", msg);
  // if (msg == "on")
  // {
  //   ESP_LOGI(TAG, "EAAAAAA");
  // }
  // else if (msg != "on")
  // {
  //   ESP_LOGI(TAG, "");
  // }
  

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