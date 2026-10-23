#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Feedback.h"
#include "GameState.h"

// OLED SSD1306 128x64 (0,96") por SPI: muestra el RSSI filtrado en grande
// y el estado de proximidad arriba. Reemplaza al buzzer.
class OledFeedback : public Feedback {
public:
  explicit OledFeedback(const GameState& game);

  void begin() override;
  void update(Proximity proximity, unsigned long now) override;

private:
  void draw(Proximity proximity, bool hasSignal, int rssi);
  void printCentered(const char* text, uint8_t size, int16_t y);

  const GameState& game_;
  Adafruit_SSD1306 display_;
  bool ready_ = false;

  // Para redibujar sólo cuando cambia algo
  bool drawnOnce_ = false;
  Proximity lastProximity_ = Proximity::SinSenal;
  bool lastHasSignal_ = false;
  int lastRssi_ = 0;
  unsigned long lastDrawMs_ = 0;
};
