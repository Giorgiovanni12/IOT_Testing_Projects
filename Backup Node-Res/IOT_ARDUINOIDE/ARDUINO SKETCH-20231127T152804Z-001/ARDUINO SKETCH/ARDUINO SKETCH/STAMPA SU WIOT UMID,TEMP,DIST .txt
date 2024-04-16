/*
   MQTT Exmple for SeeedStudio Wio Terminal
   Author: Salman Faris
   Date: 31/07/2020
   Last Updates: 02/08/2020
   MQTT Broker broker.mqtt-dashboard.com
   Subscribe Topic Name: WTIn
   Publish Topic Name: WTout
  - publishes "hello world" to the topic "WTout"
  - subscribes to the topic "WTin", printing out any messages
*/


#include "rpcWiFi.h"
#include "TFT_eSPI.h"
#include <PubSubClient.h>
#include <Ultrasonic.h>
#include <Arduino_JSON.h>
#include "DHT.h"


// Update these with values suitable for your network.
const char* ssid = "salottino";                 // WiFi Name
const char* password = "scherze771";            // WiFi Password
const char* mqtt_server = "broker.hivemq.com";  // MQTT Broker URL

TFT_eSPI tft;

WiFiClient wioClient;
PubSubClient client(wioClient);
long lastMsg = 0;
char msg[50];
int value = 0;


#define DHTTYPE DHT11      // DHT 11
#define DHTPIN BCM27       // what pin we're connected to（DHT10 and DHT20 don't need define it）
DHT dht(DHTPIN, DHTTYPE);  //   DHT11 DHT21 DHT22
Ultrasonic ult(BCM3);

void setup_wifi() {

  delay(10);

  tft.setTextSize(2);
  tft.setCursor((320 - tft.textWidth("Connecting to Wi-Fi..")) / 2, 120);
  tft.print("Connecting to Wi-Fi..");


  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  // Connecting WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  tft.fillScreen(TFT_BLACK);
  tft.setCursor((320 - tft.textWidth("Connected!")) / 2, 120);
  tft.print("Connected!");

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  // Display Local IP Address
}

void callback(char* topic, byte* payload, unsigned int length) {
  //tft.fillScreen(TFT_BLACK);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char buff_p[length];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    buff_p[i] = (char)payload[i];
  }
  Serial.println();
  buff_p[length] = '\0';
  String msg_p = String(buff_p);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor((320 - tft.textWidth("MQTT Message")) / 2, 90);
  tft.print("MQTT Message: ");
  tft.setCursor((320 - tft.textWidth(msg_p)) / 2, 120);
  tft.print(msg_p);  // Print receved payload
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "WioTerminal-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish( ult.MeasureInCentimeters());
      // ... and resubscribe
      client.subscribe("WPROVA");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);

  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  tft.begin();
  tft.setTextSize(2);
  tft.setRotation(3);
  Serial.begin(9600);

  Serial.println();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);  // Connect the MQTT Server
  client.setCallback(callback);
}

void loop() {
  JSONVar oggetto;
  float temp_hum_val[2] = { 0 };
  float distanza = ult.MeasureInCentimeters();
  String dist_string = String(distanza) + "CM";
  oggetto["distanza"] = dist_string;

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (!dht.readTempAndHumidity(temp_hum_val)) {


    float temp_valore = temp_hum_val[1];
    String temp_string = String(temp_valore) + "°C";
    oggetto["TEMPERATURE"] = temp_string;

    float umid_valore = temp_hum_val[0];
    String umid_str = String(umid_valore) + "%";
    oggetto["HUMIDITY"] = umid_str;

    tft.drawString(String(temp_string), 150, 150);
    tft.drawString(String(umid_str), 100, 100);
    tft.drawString(String(dist_string), 50, 50);
  }
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf(msg, 50, "Wio Terminal #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("WTout", msg);
  }
}