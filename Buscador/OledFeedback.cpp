#include "OledFeedback.h"
#include "config.h"

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
  display_.clearDisplay();
  display_.setTextColor(SSD1306_WHITE);
  display_.display();
}

void OledFeedback::update(Proximity proximity, unsigned long now) {
  if (!ready_) {
    return;
  }
  int rssiDraw = 0;
  const bool hasSignal = game_.hasSignal() && proximity != Proximity::SinSenal;
  const int rssi = hasSignal ? static_cast<int>(lroundf(game_.filteredRssi())) : 0;

  const bool changed = !drawnOnce_ ||
                       proximity != lastProximity_ ||
                       hasSignal != lastHasSignal_ ||
                       rssi != lastRssi_;
  if (!changed || now - lastDrawMs_ < Config::OLED_REFRESH_MS) {
    return;
  }

  rssiDraw = map(-rssi, 35, 90, 0, 100);
  if (rssiDraw >= 0)
  {
      draw(proximity, hasSignal, rssiDraw);
  }
  

  drawnOnce_ = true;
  lastProximity_ = proximity;
  lastHasSignal_ = hasSignal;
  lastRssi_ = rssi;
  lastDrawMs_ = now;
}

void OledFeedback::draw(Proximity proximity, bool hasSignal, int rssi) {
  display_.clearDisplay();

  // Estado arriba (texto chico)
  printCentered(toString(proximity), 1, 0);

  // RSSI filtrado en grande
  char value[8];
  if (hasSignal) {
    snprintf(value, sizeof(value), "%d", rssi);
  } else {
    snprintf(value, sizeof(value), "--");
  }
  printCentered(value, 4, 18);

  display_.display();
}

void OledFeedback::printCentered(const char* text, uint8_t size, int16_t y) {
  int16_t x1, y1;
  uint16_t w, h;
  display_.setTextSize(size);
  display_.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display_.setCursor((Config::OLED_WIDTH - w) / 2, y);
  display_.print(text);
}
