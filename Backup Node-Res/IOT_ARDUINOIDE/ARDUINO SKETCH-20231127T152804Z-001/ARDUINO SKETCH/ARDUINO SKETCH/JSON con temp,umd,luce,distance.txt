
#include "DHT.h"
#include <Arduino_JSON.h>
#include "Ultrasonic.h"

#define DHTTYPE DHT11  // DHT 11

#define DHTPIN BCM27       // what pin we're connected to（DHT10 and DHT20 don't need define it）
DHT dht(DHTPIN, DHTTYPE);  //   DHT11 DHT21 DHT22
Ultrasonic ultrasonic(BCM3);




#if defined(ARDUINO_ARCH_AVR)
#define debug Serial

#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
#define debug SerialUSB
#else
#define debug Serial
#endif

void setup() {

  debug.begin(115200);
  debug.println("DHTxx test!");
  Wire.begin();
  pinMode(WIO_LIGHT, INPUT);
  Serial.begin(9600);

  dht.begin();
}


void loop() {

  long RangeInInches;
  long RangeInCentimeters;


  float temp_hum_val[2] = { 0 };
  JSONVar oggetto;

  if (!dht.readTempAndHumidity(temp_hum_val)) {


    float temp_valore = temp_hum_val[1];
    String temp_string = String(temp_valore) + "°C";
    oggetto["TEMPOERATURE"] = temp_string;


    float umid_valore = temp_hum_val[0];
    String umid_str = String(umid_valore) + "%";
    oggetto["HUMIDITY"] = umid_str;

  }

  float luce =analogRead(WIO_LIGHT);
  String luce_string=String(luce) + "CL";
  oggetto["LIGHT "] = luce_string;

  float dist = ultrasonic.MeasureInCentimeters();
  String dist_string = String(dist) + "CM";
  oggetto["DISTANCE TO OBJECT:"] = dist_string;

  String jsonString = JSON.stringify(oggetto);
  Serial.println(jsonString);
  delay(10);
  delay(3000);
}

//https://www.youtube.com/watch?v=2ojWO1QNprw
