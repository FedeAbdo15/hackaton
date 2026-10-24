#include "OledAnimations.h"
#include "config.h"

namespace OledAnimations {

namespace {
  void drawStar(Adafruit_SSD1306& display, int16_t x, int16_t y, int16_t r) {
    display.drawLine(x - r, y, x + r, y, SSD1306_WHITE);
    display.drawLine(x, y - r, x, y + r, SSD1306_WHITE);
  }
}

void printCentered(Adafruit_SSD1306& display, const char* text, uint8_t size, int16_t y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(size);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((Config::OLED_WIDTH - w) / 2, y);
  display.print(text);
}

void drawSinSenal(Adafruit_SSD1306& display, uint8_t animFrame, int16_t cx, int16_t cy) {
  // Ondas de radar expandiéndose + signo de pregunta en el centro
  const uint8_t step = (animFrame / 2) % 3;
  display.drawCircle(cx, cy, 8 + step * 5, SSD1306_WHITE);
  display.drawCircle(cx, cy, 8 + ((step + 1) % 3) * 5, SSD1306_WHITE);
  printCentered(display, "?", 2, cy - 7);
}

void drawFrio(Adafruit_SSD1306& display, uint8_t animFrame, int16_t cx, int16_t cy) {
  // Pingüino aleteando y caminando en la nieve
  (void)cy;
  const bool alt = ((animFrame / 2) % 2) != 0;

  // Patitas alternando el paso
  display.fillRoundRect(cx - 10, alt ? 57 : 59, 7, 4, 1, SSD1306_WHITE);
  display.fillRoundRect(cx + 3,  alt ? 59 : 57, 7, 4, 1, SSD1306_WHITE);

  // Aletas a los costados (suben y bajan)
  display.fillTriangle(cx - 12, 33, cx - 12, 45, cx - 21, alt ? 29 : 47, SSD1306_WHITE);
  display.fillTriangle(cx + 12, 33, cx + 12, 45, cx + 21, alt ? 29 : 47, SSD1306_WHITE);

  // Cuerpo y panza del pingüino
  display.fillRoundRect(cx - 12, 28, 24, 30, 10, SSD1306_WHITE);
  display.fillRoundRect(cx - 9,  32, 18, 23, 7,  SSD1306_BLACK);
  display.fillRoundRect(cx - 7,  34, 14, 19, 5,  SSD1306_WHITE);

  // Cabeza
  display.fillCircle(cx, 23, 10, SSD1306_WHITE);

  // Ojos con pupilas
  display.fillCircle(cx - 4, 21, 2, SSD1306_BLACK);
  display.fillCircle(cx + 4, 21, 2, SSD1306_BLACK);
  display.drawPixel(cx - 4, 20, SSD1306_WHITE);
  display.drawPixel(cx + 4, 20, SSD1306_WHITE);

  // Piquito triangular
  display.fillTriangle(cx - 3, 25, cx + 3, 25, cx, 29, SSD1306_BLACK);

  // Copitos de nieve cayendo alrededor
  const int16_t y1 = 16 + ((animFrame * 2) % 42);
  const int16_t y2 = 16 + ((animFrame * 2 + 21) % 42);
  display.fillCircle(20,  y1, 1, SSD1306_WHITE);
  display.fillCircle(108, y2, 1, SSD1306_WHITE);
  display.fillCircle(34,  y2, 1, SSD1306_WHITE);
  display.fillCircle(94,  y1, 1, SSD1306_WHITE);
}

void drawTibio(Adafruit_SSD1306& display, uint8_t animFrame, int16_t cx, int16_t cy) {
  // Una tibia (hueso) balanceándose en el centro
  const int8_t tilt = ((animFrame / 2) % 2 == 0) ? 4 : -4;
  const int16_t lx = cx - 24;
  const int16_t ly = cy - tilt;
  const int16_t rx = cx + 24;
  const int16_t ry = cy + tilt;

  // Epífisis (extremos redondeados del hueso)
  display.fillCircle(lx - 2, ly - 5, 6, SSD1306_WHITE);
  display.fillCircle(lx - 2, ly + 5, 6, SSD1306_WHITE);
  display.fillCircle(rx + 2, ry - 5, 6, SSD1306_WHITE);
  display.fillCircle(rx + 2, ry + 5, 6, SSD1306_WHITE);

  // Diáfisis (cuerpo central de la tibia)
  display.fillTriangle(lx, ly - 4, lx, ly + 4, rx, ry - 4, SSD1306_WHITE);
  display.fillTriangle(lx, ly + 4, rx, ry - 4, rx, ry + 4, SSD1306_WHITE);

  // Muescas en los extremos para darle relieve de hueso
  display.drawLine(lx - 8, ly, lx - 5, ly, SSD1306_BLACK);
  display.drawLine(rx + 5, ry, rx + 8, ry, SSD1306_BLACK);

  // Líneas de movimiento en los extremos
  display.drawLine(lx - 6, ly - 14, lx + 2, ly - 14, SSD1306_WHITE);
  display.drawLine(rx - 2, ry + 14, rx + 6, ry + 14, SSD1306_WHITE);
}

void drawCaliente(Adafruit_SSD1306& display, uint8_t animFrame) {
  // Meme "This is fine": perrito con sombrero y taza de café rodeado de fuego
  const int8_t sway = (animFrame % 2 == 0) ? -2 : 2;

  // Llamas a la izquierda
  display.fillCircle(12, 44, 7, SSD1306_WHITE);
  display.fillTriangle(5, 44, 19, 44, 12 + sway, 22, SSD1306_WHITE);
  display.fillTriangle(2, 46, 10, 46, 5 - sway, 30, SSD1306_WHITE);
  display.fillCircle(12, 46, 3, SSD1306_BLACK);

  // Llamas a la derecha
  display.fillCircle(116, 44, 7, SSD1306_WHITE);
  display.fillTriangle(109, 44, 123, 44, 116 - sway, 22, SSD1306_WHITE);
  display.fillTriangle(118, 46, 126, 46, 123 + sway, 30, SSD1306_WHITE);
  display.fillCircle(116, 46, 3, SSD1306_BLACK);

  // Mesa
  display.drawFastHLine(26, 50, 76, SSD1306_WHITE);

  // Sombrerito del perro
  display.drawFastHLine(37, 17, 16, SSD1306_WHITE);
  display.fillRoundRect(40, 12, 10, 5, 2, SSD1306_WHITE);

  // Cabeza y oreja caída
  display.drawRoundRect(35, 18, 20, 17, 5, SSD1306_WHITE);
  display.fillRoundRect(31, 22, 4, 11, 2, SSD1306_WHITE);

  // Hocico hacia la derecha + nariz
  display.fillRoundRect(53, 23, 12, 9, 3, SSD1306_WHITE);
  display.fillRoundRect(54, 24, 10, 7, 2, SSD1306_BLACK);
  display.fillCircle(63, 25, 2, SSD1306_WHITE);

  // Ojos grandes tranquilos (parpadean cada tanto) y sonrisa
  if (animFrame % 8 != 0) {
    display.drawCircle(42, 23, 2, SSD1306_WHITE);
    display.drawCircle(49, 23, 2, SSD1306_WHITE);
  } else {
    display.drawFastHLine(40, 23, 4, SSD1306_WHITE);
    display.drawFastHLine(47, 23, 4, SSD1306_WHITE);
  }
  display.drawFastHLine(46, 31, 7, SSD1306_WHITE);

  // Cuerpo sentado a la mesa
  display.drawLine(39, 35, 37, 50, SSD1306_WHITE);
  display.drawLine(53, 35, 55, 50, SSD1306_WHITE);
  display.drawLine(54, 41, 63, 47, SSD1306_WHITE);

  // Taza de café sobre la mesa con humito animado
  display.drawRect(72, 41, 9, 9, SSD1306_WHITE);
  display.drawRect(81, 43, 3, 5, SSD1306_WHITE);
  const int8_t steamShift = (animFrame % 2 == 0) ? 0 : 1;
  display.drawLine(75 + steamShift, 38, 75 - steamShift, 34, SSD1306_WHITE);
  display.drawLine(79 - steamShift, 38, 79 + steamShift, 34, SSD1306_WHITE);

  // Frase abajo
  printCentered(display, "THIS IS FINE", 1, 55);
}

void drawEncontrado(Adafruit_SSD1306& display, uint8_t animFrame) {
  // Texto "ENCONTRADO" grande rebotando arriba + cofre del tesoro abriéndose con destellos
  const bool alt = (animFrame % 2) != 0;
  printCentered(display, toString(Proximity::Encontrado), 2, alt ? 3 : 6);

  constexpr int16_t cx = Config::OLED_WIDTH / 2;

  // Base del cofre del tesoro
  display.drawRect(cx - 18, 44, 36, 16, SSD1306_WHITE);
  display.drawFastHLine(cx - 18, 49, 36, SSD1306_WHITE);
  display.fillRect(cx - 3, 46, 6, 7, SSD1306_WHITE);
  display.fillRect(cx - 1, 48, 2, 3, SSD1306_BLACK);

  // Tapa del cofre que se abre y cierra + rayos del tesoro
  const int16_t lidY = alt ? 34 : 38;
  display.drawRoundRect(cx - 18, lidY, 36, 8, 3, SSD1306_WHITE);
  if (alt) {
    display.drawLine(cx - 8, 33, cx - 14, 25, SSD1306_WHITE);
    display.drawLine(cx, 32, cx, 24, SSD1306_WHITE);
    display.drawLine(cx + 8, 33, cx + 14, 25, SSD1306_WHITE);
  }

  // Estrellas / fuegos artificiales a los costados
  drawStar(display, 18,  alt ? 34 : 38, alt ? 5 : 3);
  drawStar(display, 110, alt ? 38 : 34, alt ? 3 : 5);
  drawStar(display, 30,  alt ? 54 : 50, alt ? 3 : 4);
  drawStar(display, 98,  alt ? 50 : 54, alt ? 4 : 3);
}

}  // namespace OledAnimations
