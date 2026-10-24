#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Feedback.h"
#include "GameState.h"

// OLED SSD1306 128x64 (0,96") por SPI:
//  - WaitingStart: pantalla de inicio esperando BOTÓN N2
//  - NotReady:     mensaje "Tesoro no preparado"
//  - Starting:     mensaje "Juego comenzado" por 5 segundos
//  - Playing:      distancia arriba en chico + animaciones en grande (Pingüino, Tibia, This is fine)
//  - Victory:      animación de cofre del tesoro + tiempo tardado
class OledFeedback : public Feedback {
public:
  explicit OledFeedback(const GameState& game);

  void begin() override;
  void update(Proximity proximity, unsigned long now) override;

private:
  void drawWaitingStart();
  void drawNotReady();
  void drawStarting(unsigned long now);
  void drawPlaying(Proximity proximity);
  void drawVictory();
  void drawAnimation(Proximity proximity);

  const GameState& game_;
  Adafruit_SSD1306 display_;
  bool          ready_      = false;
  uint8_t       animFrame_  = 0;
  unsigned long lastDrawMs_ = 0;
};
