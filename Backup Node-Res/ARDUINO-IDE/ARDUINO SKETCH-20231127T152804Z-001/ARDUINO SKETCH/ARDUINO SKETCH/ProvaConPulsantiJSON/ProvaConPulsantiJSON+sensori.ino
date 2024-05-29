#include "TFT_eSPI.h"
#include <Ultrasonic.h>
#include <Arduino_JSON.h>
#include "DHT.h"

#define DHTTYPE DHT11  // DHT 11

TFT_eSPI tft;
#define DHTPIN BCM27  // what pin we're connected to（DHT10 and DHT20 don't need define it）

DHT dht(DHTPIN, DHTTYPE);  //   DHT11 DHT21 DHT22

Ultrasonic ult(BCM3);

void setup() {
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
}

void loop() {

  JSONVar oggetto;
  oggetto["id"] = "01";
  float distanza = ult.MeasureInCentimeters();
  String dist_string = String(distanza) + "CM";
  oggetto["distanza"] = dist_string;


  float temp_hum_val[2] = { 0 };

  tft.fillScreen(0x0);
  tft.setTextColor(0xFFFF);
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

  if (ult.MeasureInCentimeters() < 20) {

    tft.fillScreen(0xF800);
    tft.drawString(String(dist_string), 50, 50);
  }

  if (digitalRead(WIO_KEY_A) == LOW) {
    oggetto["PULSANTE A"] = "TRUE <3";
  } else {
    oggetto["PULSANTE A"] = "FALSE <3";
  }

  if (digitalRead(WIO_KEY_B) == LOW) {
    oggetto["PULSANTE B"] = "TRUE <3";
  } else {
    oggetto["PULSANTE B"] = "FALSE <3";
  }


  if (digitalRead(WIO_KEY_C) == LOW) {
    oggetto["PULSANTE C"] = true;
  } else {
    oggetto["PULSANTE C"] = "false";
  }





  String jsonString = JSON.stringify(oggetto);
  Serial.println(jsonString);
  delay(3000);
}