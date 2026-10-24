#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Proximity.h"

namespace OledAnimations {
  void drawSinSenal(Adafruit_SSD1306& display, uint8_t animFrame, int16_t cx, int16_t cy);
  void drawFrio(Adafruit_SSD1306& display, uint8_t animFrame, int16_t cx, int16_t cy);
  void drawTibio(Adafruit_SSD1306& display, uint8_t animFrame, int16_t cx, int16_t cy);
  void drawCaliente(Adafruit_SSD1306& display, uint8_t animFrame);
  void drawEncontrado(Adafruit_SSD1306& display, uint8_t animFrame);

  void printCentered(Adafruit_SSD1306& display, const char* text, uint8_t size, int16_t y);
}
