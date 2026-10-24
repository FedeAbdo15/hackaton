#include "TimerDisplay.h"
#include "config.h"

TimerDisplay::TimerDisplay()
  // Constructor de SPI por software: (ancho, alto, MOSI, CLK, DC, RST, CS)
  : display_(Config::OLED_WIDTH, Config::OLED_HEIGHT,
             Config::OLED_MOSI, Config::OLED_CLK,
             Config::OLED_DC, Config::OLED_RST, Config::OLED_CS) {}

bool TimerDisplay::begin() {
  ready_ = display_.begin(SSD1306_SWITCHCAPVCC);
  if (!ready_) {
    Serial.println("[Tesoro] No se pudo iniciar la pantalla OLED");
    return false;
  }
  display_.setRotation(Config::OLED_ROTATION);
  display_.setTextColor(SSD1306_WHITE);
  draw();  // "Presione el boton 1 para empezar"
  return true;
}

void TimerDisplay::showWaitingSeeker() {
  waitingSeeker_ = true;
  startedOnce_   = false;
  draw();
}

void TimerDisplay::start(unsigned long now) {
  if (running_) {
    return;
  }
  seconds_       = Config::TIMER_START_SECONDS;
  running_       = true;
  startedOnce_   = true;
  waitingSeeker_ = false;
  animFrame_     = 0;
  lastTickMs_    = now;
  lastAnimMs_    = now;
  draw();
}

void TimerDisplay::stop() {
  running_ = false;
  draw();
}

void TimerDisplay::update(unsigned long now) {
  if (!running_) {
    return;
  }

  bool needsRedraw = false;

  if (now - lastTickMs_ >= Config::TIMER_TICK_MS) {
    lastTickMs_ += Config::TIMER_TICK_MS;  // evita deriva acumulada
    seconds_++;
    needsRedraw = true;
  }

  if (now - lastAnimMs_ >= Config::OLED_REFRESH_MS) {
    lastAnimMs_ = now;
    animFrame_++;
    needsRedraw = true;
  }

  if (needsRedraw) {
    draw();
  }
}

void TimerDisplay::draw() {
  if (!ready_) {
    return;
  }

  display_.clearDisplay();

  if (!startedOnce_) {
    if (waitingSeeker_) {
      drawWaitingSeeker();
    } else {
      drawStartPrompt();
    }
  } else if (seconds_ < 0) {
    drawCountdown();
  } else {
    drawClockWithDigitalCenter();
  }

  display_.display();
}

void TimerDisplay::drawStartPrompt() {
  printCentered("Presione el boton 1", 1, 20);
  printCentered("para empezar", 1, 34);
}

void TimerDisplay::drawWaitingSeeker() {
  printCentered("Transmitiendo...", 1, 18);
  printCentered("Esperando Buscador", 1, 34);
}

void TimerDisplay::drawCountdown() {
  printCentered("Juego", 2, 8);
  printCentered("comenzado", 2, 28);

  const unsigned int remaining = static_cast<unsigned int>(-seconds_);  // 5, 4, 3, 2, 1
  char countdownText[16];
  snprintf(countdownText, sizeof(countdownText), "Iniciando en %u...", remaining);
  printCentered(countdownText, 1, 52);
}

void TimerDisplay::drawClockWithDigitalCenter() {
  const bool   tickAlt = running_ && ((animFrame_ % 2) != 0);
  const int8_t earTilt = tickAlt ? 2 : 0;
  const int8_t tap     = tickAlt ? 4 : 0;

  // --- 1) Conejo Blanco grande y detallado (izquierda, x = 2..75) ---
  // Orejas largas de conejo
  display_.fillRoundRect(13, 0,           10, 22, 4, SSD1306_WHITE);
  display_.fillRoundRect(16, 3,            4, 16, 2, SSD1306_BLACK);
  display_.fillRoundRect(27, 0 + earTilt, 10, 22, 4, SSD1306_WHITE);
  display_.fillRoundRect(30, 3 + earTilt,  4, 16, 2, SSD1306_BLACK);

  // Cabeza y mejillas
  display_.fillRoundRect(8, 18, 36, 24, 9, SSD1306_WHITE);
  display_.fillCircle(41, 33, 7, SSD1306_WHITE);
  display_.fillRoundRect(10, 20, 32, 20, 7, SSD1306_BLACK);
  display_.fillCircle(40, 33, 5, SSD1306_BLACK);

  // Cejas expresivas y dos ojos mirando hacia el reloj
  display_.drawLine(16, 23, 23, 24, SSD1306_WHITE);
  display_.drawLine(29, 24, 36, 22, SSD1306_WHITE);
  display_.drawCircle(20, 28, 3, SSD1306_WHITE);
  display_.fillCircle(21, 28, 1, SSD1306_WHITE);
  display_.drawCircle(32, 28, 3, SSD1306_WHITE);
  display_.fillCircle(33, 28, 1, SSD1306_WHITE);

  // Nariz triangular, dientitos y bigotes
  display_.fillTriangle(26, 32, 32, 32, 29, 35, SSD1306_WHITE);
  display_.drawRect(27, 36, 5, 3, SSD1306_WHITE);
  display_.drawPixel(29, 37, SSD1306_WHITE);
  display_.drawLine(2, 29, 10, 31, SSD1306_WHITE);
  display_.drawLine(2, 33, 10, 33, SSD1306_WHITE);
  display_.drawLine(2, 37, 10, 35, SSD1306_WHITE);

  // Cuerpo con saco y moño elegante
  display_.drawRoundRect(8, 41, 34, 22, 6, SSD1306_WHITE);
  display_.drawLine(26, 48, 26, 62, SSD1306_WHITE);  // solapa del chaleco
  display_.fillTriangle(18, 42, 26, 45, 18, 48, SSD1306_WHITE);
  display_.fillTriangle(34, 42, 26, 45, 34, 48, SSD1306_WHITE);
  display_.fillCircle(26, 45, 2, SSD1306_BLACK);
  display_.drawCircle(26, 45, 2, SSD1306_WHITE);

  // Brazo extendido, mano enguantada y dedo índice señalando el reloj ("tap-tap")
  display_.fillRoundRect(38, 44, 18 + tap, 7, 3, SSD1306_WHITE);
  display_.fillCircle(56 + tap, 46, 5, SSD1306_WHITE);          // puño/guante
  display_.fillRoundRect(59 + tap, 43, 12, 4, 2, SSD1306_WHITE); // dedo índice apuntando
  if (tickAlt) {
    display_.drawLine(72, 38, 75, 35, SSD1306_WHITE);
    display_.drawLine(72, 51, 75, 54, SSD1306_WHITE);
  }

  // --- 2) Reloj de bolsillo redondo compacto (derecha, cx = 102, cy = 38, r = 22) ---
  constexpr int16_t watchCx = 102;
  constexpr int16_t watchCy = 38;

  // Argolla y corona superior del reloj de bolsillo
  display_.drawCircle(watchCx, 10, 3, SSD1306_WHITE);
  display_.fillRect(watchCx - 3, 13, 7, 3, SSD1306_WHITE);

  // Caja circular doble del reloj de bolsillo
  display_.drawCircle(watchCx, watchCy, 22, SSD1306_WHITE);
  display_.drawCircle(watchCx, watchCy, 20, SSD1306_WHITE);

  // 12 marcas horarias circulares (radio 17) + segundero animado
  struct Point { int16_t x; int16_t y; };
  static constexpr Point kHourTicks[12] = {
    { 102, 21 }, // 12
    { 110, 23 }, // 1
    { 117, 29 }, // 2
    { 119, 38 }, // 3
    { 117, 47 }, // 4
    { 110, 53 }, // 5
    { 102, 55 }, // 6
    {  94, 53 }, // 7
    {  87, 47 }, // 8
    {  85, 38 }, // 9
    {  87, 29 }, // 10
    {  94, 23 }  // 11
  };

  for (uint8_t i = 0; i < 12; ++i) {
    display_.drawPixel(kHourTicks[i].x, kHourTicks[i].y, SSD1306_WHITE);
  }

  const uint8_t activeTick = running_ ? (animFrame_ % 12) : static_cast<uint8_t>(seconds_ % 12);
  display_.fillCircle(kHourTicks[activeTick].x, kHourTicks[activeTick].y, 2, SSD1306_WHITE);

  // --- 3) Reloj digital (MM:SS) centrado dentro del reloj de bolsillo ---
  const unsigned long absSec = static_cast<unsigned long>(seconds_);
  const unsigned long m      = absSec / 60;
  const unsigned int  sec    = absSec % 60;

  char text[16];
  snprintf(text, sizeof(text), "%02lu:%02u", m, sec);

  int16_t  x1, y1;
  uint16_t w, h;
  display_.setTextSize(1);
  display_.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display_.setCursor(watchCx - static_cast<int16_t>(w) / 2, watchCy - static_cast<int16_t>(h) / 2);
  display_.print(text);
}

void TimerDisplay::printCentered(const char* text, uint8_t size, int16_t y) {
  int16_t  x1, y1;
  uint16_t w, h;
  display_.setTextSize(size);
  display_.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display_.setCursor((Config::OLED_WIDTH - w) / 2, y);
  display_.print(text);
}
