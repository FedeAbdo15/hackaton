#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Feedback.h"
#include "GameState.h"

// OLED SSD1306 128x64 (0,96") por SPI: muestra el valor arriba en chico
// y una animación de temperatura en grande en el centro.
class OledFeedback : public Feedback {
public:
  explicit OledFeedback(const GameState& game);

  void begin() override;
  void update(Proximity proximity, unsigned long now) override;

private:
  void draw(Proximity proximity, bool hasSignal, int rssi);
  void drawAnimation(Proximity proximity, bool hasSignal);

  const GameState& game_;
  Adafruit_SSD1306 display_;
  bool ready_ = false;

  uint8_t animFrame_ = 0;
  unsigned long lastDrawMs_ = 0;
};
