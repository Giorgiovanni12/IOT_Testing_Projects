#define BROKER_IP "broker.hivemq.com"
#define DEV_NAME "mqttdevice"
#define MQTT_USER "mqtt_user"
#define MQTT_PW "mqtt_password"
#define MQTT_SUB "/mirko"
#define MQTT_PUB "/mirko"

const char ssid[] = "Enrico";
const char pass[] = "CIAOCIAO";

#include "TFT_eSPI.h"
TFT_eSPI tft;
#include <MQTT.h>

#ifdef WIO_TERMINAL
#include "rpcWiFi.h"
#elif ARDUINO_SAMD_MKRWIFI1010
#include <WiFiNINA.h>
#elif ARDUINO_SAMD_MKR1000
#include <WiFi101.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#else
#error unknown board
#endif

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

#include <Arduino_JSON.h>

// Delaration of Sensors

#include "LIS3DHTR.h"

LIS3DHTR<TwoWire> lis;

int light = WIO_LIGHT;
int mic = WIO_MIC;
int key_a = WIO_KEY_A;
int key_b = WIO_KEY_B;


void connect() {
  Serial.print("checking wifi...");
  tft.setTextSize(2);
  tft.setCursor((320 - tft.textWidth("Connecting to Wi-Fi..")) / 2, 120);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  tft.setCursor((320 - tft.textWidth("Connected!")) / 2, 120);

  Serial.print("\nconnecting...");
  tft.setTextSize(2);
  tft.setCursor((320 - tft.textWidth("Connecting to MQTT..")) / 2, 120);
  while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  tft.setTextSize(2);
  tft.setCursor((320 - tft.textWidth("Connected!")) / 2, 120);
  client.subscribe(MQTT_SUB);  //SUBSCRIBE TO TOPIC /hello
}
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  

  if (topic == MQTT_SUB) {


  JSONVar myObject = JSON.parse(payload);

  if (myObject.hasOwnProperty("btnb")) {
     // Serial.print("myObject[\"count\"] = ");
  
      Serial.println((int) myObject["btnb"]);
      if((int) myObject["btnb"] == 1)
      {tft.fillScreen(0xf800);}else
      {
        tft.fillScreen(0x000);
      }
      
    }
  }
}


void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  tft.begin();
  tft.setTextSize(4);
  tft.setRotation(3);

  // TODO ADD WIO_LIGHT / DECLARE LIGHT INPUT


  client.begin(BROKER_IP, 1883, net);
  client.onMessage(messageReceived);
  connect();

  pinMode(light, INPUT);
  pinMode(mic, INPUT);
  pinMode(key_a, INPUT_PULLUP);
  pinMode(key_b, INPUT_PULLUP);
  lis.begin(Wire1);
  lis.setOutputDataRate(LIS3DHTR_DATARATE_100HZ);
  lis.setFullScaleRange(LIS3DHTR_RANGE_4G);
}

void loop() {

  client.loop();
  if (!client.connected()) {
    connect();
  }

  // create the JSON object
  JSONVar myObject;

  // read the light, pass it to light_amount variable, assign it to "luce" key parameter
  int light_amount = analogRead(light);
  myObject["luce"] = (int)light_amount;

  int noise = analogRead(WIO_MIC);
  myObject["suono"] = (int)noise;

  int btn_a = digitalRead(WIO_KEY_A) == LOW;
  myObject["btn-a"] = (int)btn_a;

  int btn_b = digitalRead(WIO_KEY_B) == LOW;
  myObject["btnb"] = (int)btn_b;

  int Z = lis.getAccelerationZ();
  myObject["z-acc"] = (int)Z;



  // JSON.stringify(myVar) can be used to convert the json var to a String
  String jsonString = JSON.stringify(myObject);

  // publish a message roughly every 5 seconds.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    //Serial.println(myObject);

    client.publish(MQTT_PUB, jsonString);  // Publish to the topic
  }
}
