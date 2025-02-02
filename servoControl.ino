
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <Servo.h>

const char* ssid = "*****";
const char* password = "*****";
String serverUrl = "https://goose-cat.com/Home";
StaticJsonDocument<200> jsonDoc;
DeserializationError jsonError;
uint8_t LED4 = 4;
uint8_t servo1port = 5;
uint8_t servo2port = 16;
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);
  //LED
  pinMode(LED4, OUTPUT);
  //伺服馬達
  servo1.attach(servo1port);
  servo2.attach(servo2port);
  //連線
  connectToWIFI();
}

void loop() {
  String str = connectToHTTPS();
  jsonError = deserializeJson(jsonDoc, str);
  if (!jsonError){
    bool port4 = jsonDoc["port4"].as<bool>();
    bool port5 = jsonDoc["port5"].as<bool>();
    int servo1SpeedL = jsonDoc["servo1SpeedL"].as<int>();
    int servo1SpeedR = jsonDoc["servo1SpeedR"].as<int>();
    Serial.println(port5);
    if(port4){
      LED_HIGH(0);
    }else{
      LED_LOW(0);
    }
    if(port5){
      servoGO(servo1SpeedL, servo1SpeedR);
    }else{
      servoGO(0, 0);
    }
  }
  delay(500);
}

//WIFI連接
void connectToWIFI(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
}

//HTTPS連接
String connectToHTTPS(){
  //wait for WiFi connection
  String resultStr = "";
  if ((WiFi.status() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;
    if (https.begin(*client, serverUrl + "/GetArduinoA1Data?UserId=goose")) {
      int httpCode = https.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          resultStr = https.getString();
          //Serial.println(resultStr);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  return resultStr;
}

//LED燈開啟
void LED_HIGH(int delayTime){
  //Serial.println("LED_HIGH");
  digitalWrite(LED4, HIGH);
  delay(delayTime);
}

//LED燈關閉
void LED_LOW(int delayTime){
  //Serial.println("LED_LOW");
  digitalWrite(LED4, LOW);
  delay(delayTime);
}

//伺服馬達控制
void servoGO(int numL, int numR){
  if(numL == 0){
    servo1.detach();
  }else{
    servo1.attach(servo1port);
    servo1.write(numL);
  }
  //====
  if(numR == 0){
    servo2.detach();
  }else{
    servo2.attach(servo2port);
    servo2.write(numR);
  }
}