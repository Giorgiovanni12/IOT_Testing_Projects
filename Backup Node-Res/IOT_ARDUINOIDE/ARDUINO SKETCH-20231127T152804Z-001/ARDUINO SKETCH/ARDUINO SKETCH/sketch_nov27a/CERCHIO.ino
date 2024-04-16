#include "TFT_eSPI.h"

TFT_eSPI tft;
float x;
float y;
float r;

void setup() {
  tft.begin();
  pinMode(WIO_KEY_A, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  x = 20;
  y = 20;
  r = 20;
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
}



void loop() {


  tft.fillScreen(0xFFFF);

  if (digitalRead(WIO_5S_PRESS) == LOW) {
    r = r * 2;
  }


  if (digitalRead(WIO_5S_UP) == LOW) {
    x = x + -15;
  }
  if (digitalRead(WIO_5S_DOWN) == LOW) {
    x = x + 15;
  }
  if (digitalRead(WIO_5S_LEFT) == LOW) {
    y = y + 15;
  }
  if (digitalRead(WIO_5S_RIGHT) == LOW) {
    y = y - 15;
  }

  if (digitalRead(WIO_KEY_A) == LOW) {
     r = r / 2;
  }
  tft.drawCircle(x, y, r, 0xF800);
  tft.fillCircle(x, y, r, 0xF800);
}