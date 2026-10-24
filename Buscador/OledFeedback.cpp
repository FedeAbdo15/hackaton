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
  drawWaitingStart();
  display_.display();
}

void OledFeedback::update(Proximity proximity, unsigned long now) {
  if (!ready_) {
    return;
  }
  if (now - lastDrawMs_ < Config::OLED_REFRESH_MS) {
    return;
  }

  display_.clearDisplay();

  switch (game_.phase()) {
    case GamePhase::WaitingStart:
      drawWaitingStart();
      break;
    case GamePhase::NotReady:
      drawNotReady();
      break;
    case GamePhase::Starting:
      drawStarting(now);
      break;
    case GamePhase::Playing:
      drawPlaying(proximity);
      break;
    case GamePhase::Victory:
      drawVictory();
      break;
  }

  display_.display();
  animFrame_++;
  lastDrawMs_ = now;
}

void OledFeedback::drawWaitingStart() {
  OledAnimations::printCentered(display_, "Presiona el boton 2", 1, 18);
  OledAnimations::printCentered(display_, "para empezar", 1, 32);
}

void OledFeedback::drawNotReady() {
  if ((animFrame_ % 2) == 0) {
    display_.drawRect(0, 0, Config::OLED_WIDTH, Config::OLED_HEIGHT, SSD1306_WHITE);
  }
  OledAnimations::printCentered(display_, "Tesoro no", 2, 14);
  OledAnimations::printCentered(display_, "preparado", 2, 36);
}

void OledFeedback::drawStarting(unsigned long now) {
  OledAnimations::printCentered(display_, "Juego", 2, 8);
  OledAnimations::printCentered(display_, "comenzado", 2, 28);

  char countdownText[16];
  snprintf(countdownText, sizeof(countdownText), "Iniciando en %u...", game_.startingSecondsLeft(now));
  OledAnimations::printCentered(display_, countdownText, 1, 52);
}

void OledFeedback::drawPlaying(Proximity proximity) {
  if (proximity == Proximity::Encontrado) {
    drawVictory();
    return;
  }

  // Arriba en texto chico: SIN SENAL solo cuando el estado es SinSenal, si no la distancia (0 a 100)
  if (proximity == Proximity::SinSenal) {
    OledAnimations::printCentered(display_, toString(Proximity::SinSenal), 1, 0);
  } else {
    char value[8];
    snprintf(value, sizeof(value), "%d", game_.distance());
    OledAnimations::printCentered(display_, value, 1, 0);
  }

  // En el medio grande: animación según el threshold (Pingüino, Tibia, This is fine)
  drawAnimation(proximity);
}

void OledFeedback::drawVictory() {
  const uint32_t secs = game_.victorySeconds();
  uint8_t speedFrame = animFrame_;
  if (secs > Config::MEDIUM_WIN_SECONDS) {
    speedFrame = animFrame_ / 4;  // más lento si tardó mucho
  } else if (secs > Config::FAST_WIN_SECONDS) {
    speedFrame = animFrame_ / 2;  // velocidad media
  }

  OledAnimations::drawEncontrado(display_, speedFrame);
}

void OledFeedback::drawAnimation(Proximity proximity) {
  constexpr int16_t cx = Config::OLED_WIDTH / 2;
  constexpr int16_t cy = 39;

  switch (proximity) {
    case Proximity::SinSenal:
      OledAnimations::drawSinSenal(display_, animFrame_, cx, cy);
      break;
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
