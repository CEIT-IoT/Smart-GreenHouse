#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN_1 5
#define DHTPIN_2 6

#define LDRPIN_1 A0
#define LDRPIN_2 A1

#define SOILPIN_1 A3
#define SOILPIN_2 A4

#define LEDPIN 12

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht_1(DHTPIN_1, DHTTYPE);
DHT dht_2(DHTPIN_2, DHTTYPE);

int humi_1, temp_1, light_1, soil_1, soil_2;
int humi_2, temp_2, light_2;



SoftwareSerial linkSerial(8, 9); // RX, TX

long publicInterval = 10000;      // ms
long lastPublic = millis();
int sensorReadCount = 0;

void setup_dht(){
  dht_1.begin();
  dht_2.begin();
}

void setup_analog_pin() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

void setup() {
  Serial.begin(115200);
  linkSerial.begin(9600);
  Serial.println("Starting...");
  setup_analog_pin();
  setup_dht();
}

void loop() {
  // Indoor Sensors
  
  humi_1 = humi_1 + dht_1.readHumidity();
  temp_1 = temp_1 + dht_1.readTemperature();
  light_1 = light_1 + analogRead(LDRPIN_1);
  soil_1 = soil_1 + analogRead(SOILPIN_1); 
  soil_2 = soil_2 + analogRead(SOILPIN_2);
  
  humi_2 = humi_2 + dht_2.readHumidity();
  temp_2 = temp_2 + dht_2.readTemperature();
  light_2 = light_2 + analogRead(LDRPIN_2); 

  sensorReadCount++;
  
  long now = millis();
  if (now - lastPublic > publicInterval) {
      humi_1 = round(humi_1/sensorReadCount);
      temp_1 = round(temp_1/sensorReadCount);
      light_1 = round(light_1/sensorReadCount);
      soil_1 = round(soil_1/sensorReadCount);
      soil_2 = round(soil_2/sensorReadCount);

      humi_2 = round(humi_2/sensorReadCount);
      temp_2 = round(temp_2/sensorReadCount);
      light_2 = round(light_2/sensorReadCount);

      // Cuurent sensor module provide 1023 as darkness and 0 as brightness  
      // enable below code to invert 1023-0 -> 0-1023
      light_1 = 1023-light_1;
      light_2 = 1023-light_2;
      

      StaticJsonDocument<200> data_1;
      StaticJsonDocument<200> data_2;
      
      data_1["ID"] = "indoor";
      data_1["humidity"] = (int)humi_1;
      data_1["temperature"] = (int)temp_1;
      data_1["light"] = (int)light_1;
      data_1["soil"][0] = (int)soil_1;
      data_1["soil"][1] = (int)soil_2;

      data_2["ID"] = "outdoor";
      data_2["humidity"] = (int)humi_2;
      data_2["temperature"] = (int)temp_2;
      data_2["light"] = (int)light_2;

      serializeJson(data_1, linkSerial);
      digitalWrite(LEDPIN, HIGH);
      delay(10);
      digitalWrite(LEDPIN, LOW);
      serializeJson(data_2, linkSerial);
      digitalWrite(LEDPIN, HIGH);
      delay(10);
      digitalWrite(LEDPIN, LOW);
      Serial.println("Sending...");
      
      Serial.print("Indoor Status: ");
      Serial.print((int)humi_1);
      Serial.print(", ");
      Serial.print((int)temp_1);
      Serial.print(", ");
      Serial.print((int)light_1);
      Serial.print(", ");
      Serial.print((int)soil_1);
      Serial.print(", ");
      Serial.print((int)soil_2);
      Serial.println();
      Serial.print("outdoor Status: ");
      Serial.print((int)humi_2);
      Serial.print(", ");
      Serial.print((int)temp_2);
      Serial.print(", ");
      Serial.print((int)light_2);
      Serial.println();
      
    sensorReadCount = 0;
    lastPublic = now;
  }

  delay(1000);
}
