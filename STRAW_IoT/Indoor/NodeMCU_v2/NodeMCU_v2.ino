#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#define WIFIPIN D8
#define DATAPIN D7

const char* ssid = "Smart IoT";
const char* password = "iot@2020";

const char* mqtt_server = "192.168.9.76";
const int mqtt_port = 1883;
const char* mqtt_user = "strawberry";
const char* mqtt_password = "123456";


WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

SoftwareSerial linkSerial(D2, D1); // RX, TX

// WiFi ----------------------------------------------------------
void wifi_connect() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    LEDblink(WIFIPIN, 50);
    delay(500);
    Serial.print(".");
  }
  digitalWrite(WIFIPIN,HIGH);
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);
}

void wifi_reconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFIPIN, LOW);
    digitalWrite(DATAPIN, LOW);
    Serial.println("WiFi Status [code]: ");
    Serial.println(WiFi.status());
    WiFi.disconnect();
    wifi_connect();
  }
}

// MQTT ----------------------------------------------------------
void setup_mqtt() {
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  if (!mqtt.connected()) {
    digitalWrite(DATAPIN, LOW);
    while (!mqtt.connected()) {
      LEDblink(DATAPIN, 10);
      Serial.print("Connecting to MQTT...");
      // Create a random client ID
      String clientId = "ESP8266-1";
   
      // Attempt to connect
      if (mqtt.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
        Serial.println("connected");
        digitalWrite(DATAPIN, LOW);
        
        // Once connected, publish an announcement...
        mqtt.publish("IoT/device", (char*)clientId.c_str());
        LEDblink(DATAPIN, 50);
        delay(50);
        mqtt.subscribe("@straw/control/reboot");
      } else {
        Serial.print("failed, rc=");
        Serial.print(mqtt.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] msg:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  char CMD = (char)payload[0];
  if (CMD == '1') {
    digitalWrite(D0, HIGH);
  } else {
    digitalWrite(D0, LOW);
  }
  LEDblink(DATAPIN, 50);
}

void setup_system_status() {
  pinMode(WIFIPIN, OUTPUT);
  pinMode(DATAPIN, OUTPUT);
  
  digitalWrite(DATAPIN, HIGH);
  digitalWrite(WIFIPIN, HIGH);
  delay(50);
  digitalWrite(DATAPIN, LOW);
  digitalWrite(WIFIPIN, LOW);
  
}

void LEDblink(int PIN, int flash_rate){
  digitalWrite(PIN, HIGH);
  delay(flash_rate);
  digitalWrite(PIN, LOW);
  delay(flash_rate);
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.println("Starting...");
  linkSerial.begin(9600);
  setup_system_status();
  wifi_connect();
  setup_mqtt();
}

void loop() {
  wifi_reconnect();
  mqtt_reconnect();
  mqtt.loop();

  if (linkSerial.available()){
//    String jsonMsg;
    DynamicJsonDocument data(300);
    DeserializationError err = deserializeJson(data, linkSerial);
    if (err != DeserializationError::Ok){
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());

      // Flush all bytes in the "link" serial port buffer
      while (linkSerial.available() > 0)
        linkSerial.read();
    } else {
      digitalWrite(DATAPIN, LOW);
      
      if (data.size()>0) {
        String zone = data["ID"];
        String topic = "@straw/"+zone;
        String topic_pub;
        
        String H = data["humidity"];
        String T = data["temperature"];
        String L = data["light"];

        Serial.println();
        Serial.println(zone);
        Serial.println("Publishing...");
        topic_pub = topic + "/humidity";
        if (mqtt.publish(topic_pub.c_str(), H.c_str())) {
          Serial.print("Humidity: ");
          Serial.println(H);
          LEDblink(DATAPIN, 5);
        }
        
        topic_pub = topic + "/temperature";
        if (mqtt.publish(topic_pub.c_str(), T.c_str())) {
          Serial.print("Temperatur: ");
          Serial.println(T);
          LEDblink(DATAPIN, 5);
        }
        
        topic_pub = topic + "/light";
        if (mqtt.publish(topic_pub.c_str(), L.c_str())) {
          Serial.print("Light: ");
          Serial.println(L);
          LEDblink(DATAPIN, 5);
        }
        
        if (zone=="indoor") {

          String S1 = data["soil"][0];
          String S2 = data["soil"][1];
          
          topic_pub = topic + "/soil/1";
          if (mqtt.publish(topic_pub.c_str(), S1.c_str())){
            Serial.print("Soil Moisture 1: ");
            Serial.println(S1);
            LEDblink(DATAPIN, 5);
          }
          
          topic_pub = topic + "/soil/2";
          if (mqtt.publish(topic_pub.c_str(), S2.c_str())) {
            Serial.print("Soil Moisture 2: ");
            Serial.println(S2);
            LEDblink(DATAPIN, 5);
          }
        }
      }
    }
  }
}
