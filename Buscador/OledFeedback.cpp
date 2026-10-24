#include "OledFeedback.h"
#include "OledAnimations.h"
#include "config.h"
#include <math.h>

OledFeedback::OledFeedback(const GameState& game)
  : game_(game),
    // Constructor de SPI por software: (ancho, alto, MOSI, CLK, DC, RST, CS)
    display_(Config::OLED_WIDTH, Config::OLED_HEIGHT,
             Config::OLED_MOSI, Config::OLED_CLK,
             Config::OLED_DC, Config::OLED_RST, Config::OLED_CS) {}

void OledFeedback::begin() {
  ready_ = display_.begin(SSD1306_SWITCHCAPVCC);
  if (!ready_) {
    Serial.println("[Buscador] No se pudo iniciar la pantalla OLED");
    return;
  }
  display_.setRotation(Config::OLED_ROTATION);
  display_.clearDisplay();
  display_.setTextColor(SSD1306_WHITE);
  OledAnimations::printCentered(display_, "Presiona el boton 1", 1, 20);
  OledAnimations::printCentered(display_, "para empezar", 1, 34);
  display_.display();
}

void OledFeedback::update(Proximity proximity, unsigned long now) {
  if (!ready_) {
    return;
  }
  if (now - lastDrawMs_ < Config::OLED_REFRESH_MS) {
    return;
  }

  const bool hasSignal = game_.hasSignal() && proximity != Proximity::SinSenal;
  const int rssi = hasSignal ? static_cast<int>(lroundf(game_.filteredRssi())) : 0;

  int rssiDraw = 0;
  if (hasSignal) {
    rssiDraw = map(-rssi, 35, 90, 0, 100);
    if (rssiDraw < 0) {
      rssiDraw = 0;
    }
  }

  draw(proximity, hasSignal, rssiDraw);

  animFrame_++;
  lastDrawMs_ = now;
}

void OledFeedback::draw(Proximity proximity, bool hasSignal, int rssi) {
  display_.clearDisplay();

  if (proximity == Proximity::Encontrado) {
    OledAnimations::drawEncontrado(display_, animFrame_);
    display_.display();
    return;
  }

  // Arriba en texto chico: número (o SIN SENAL cuando no hay señal)
  if (hasSignal) {
    char value[8];
    snprintf(value, sizeof(value), "%d", rssi);
    OledAnimations::printCentered(display_, value, 1, 0);
  } else {
    OledAnimations::printCentered(display_, toString(Proximity::SinSenal), 1, 0);
  }

  // En el medio grande: animación según la temperatura
  drawAnimation(proximity, hasSignal);

  display_.display();
}

void OledFeedback::drawAnimation(Proximity proximity, bool hasSignal) {
  constexpr int16_t cx = Config::OLED_WIDTH / 2;
  constexpr int16_t cy = 39;

  if (!hasSignal || proximity == Proximity::SinSenal) {
    OledAnimations::drawSinSenal(display_, animFrame_, cx, cy);
    return;
  }

  switch (proximity) {
    case Proximity::Frio:
      OledAnimations::drawFrio(display_, animFrame_, cx, cy);
      break;
    case Proximity::Tibio:
      OledAnimations::drawTibio(display_, animFrame_, cx, cy);
      break;
    case Proximity::Caliente:
      OledAnimations::drawCaliente(display_, animFrame_);
      break;
    default:
      break;
  }
}
