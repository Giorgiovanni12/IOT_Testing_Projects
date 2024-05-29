// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include <Arduino_JSON.h>
#include "Ultrasonic.h"

#define DHTTYPE DHT11  // DHT 11
#define DHTTYPE DHT22  // DHT 22  (AM2302)

#define DHTPIN BCM27       // what pin we're connected to（DHT10 and DHT20 don't need define it）
DHT dht(DHTPIN, DHTTYPE);  //   DHT11 DHT21 DHT22
//DHT dht(DHTTYPE);         //   DHT10 DHT20 don't need to define Pin
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
  /*if using WIO link,must pull up the power pin.*/
  // pinMode(PIN_GROVE_POWER, OUTPUT);
  // digitalWrite(PIN_GROVE_POWER, 1);

  dht.begin();
}


void loop() {

  long RangeInInches;
  long RangeInCentimeters;

  Serial.println("The distance to obstacles in front is: ");
  RangeInInches = ultrasonic.MeasureInInches();
  Serial.print(RangeInInches);  //0~157 inches
  Serial.println(" inch");
  delay(250);

  RangeInCentimeters = ultrasonic.MeasureInCentimeters();  // two measurements should keep an interval
  Serial.print(RangeInCentimeters);                        //0~400cm
  Serial.println(" cm");
  delay(250);

  float temp_hum_val[2] = { 0 };
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  JSONVar oggetto;

  if (!dht.readTempAndHumidity(temp_hum_val)) {
    oggetto["Temperatura"] = (temp_hum_val[1]);
    oggetto["Umidità"] = (temp_hum_val[0]);

    //   debug.print("Humidity: ");
    //   debug.print(temp_hum_val[0]);
    //   debug.print(" %\t");
    //   debug.print("Temperature: ");
    //   debug.print(temp_hum_val[1]);
    //   debug.println(" *C");
    // } else {
    //   debug.println("Failed to get temprature and humidity value.");
  }
  oggetto["light"] = analogRead(WIO_LIGHT);
  oggetto["Distance:"] = ultrasonic.MeasureInCentimeters();

  String jsonString = JSON.stringify(oggetto);
  Serial.println(jsonString);
  delay(10);
  delay(1500);
}


