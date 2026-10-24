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
  draw();  // 00:00:00 detenido
  return true;
}

void TimerDisplay::start(unsigned long now) {
  if (running_) {
    return;
  }
  seconds_    = Config::TIMER_START_SECONDS;
  running_    = true;
  lastTickMs_ = now;
  draw();
}

void TimerDisplay::stop() {
  running_ = false;
}

void TimerDisplay::update(unsigned long now) {
  if (!running_ || now - lastTickMs_ < Config::TIMER_TICK_MS) {
    return;
  }
  lastTickMs_ += Config::TIMER_TICK_MS;  // evita deriva acumulada
  seconds_++;
  draw();
}

void TimerDisplay::draw() {
  if (!ready_) {
    return;
  }

  const bool          negative = seconds_ < 0;
  const unsigned long absSec   = negative
                                   ? static_cast<unsigned long>(-seconds_)
                                   : static_cast<unsigned long>(seconds_);
  const unsigned long h   = absSec / 3600;
  const unsigned int  m   = (absSec % 3600) / 60;
  const unsigned int  sec = absSec % 60;

  char text[16];
  if (negative) {
    snprintf(text, sizeof(text), "-%02lu:%02u:%02u", h, m, sec);
  } else {
    snprintf(text, sizeof(text), "%02lu:%02u:%02u", h, m, sec);
  }

  display_.clearDisplay();
  printCentered(text, 2);
  display_.display();
}

void TimerDisplay::printCentered(const char* text, uint8_t size) {
  int16_t  x1, y1;
  uint16_t w, h;
  display_.setTextSize(size);
  display_.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display_.setCursor((display_.width() - w) / 2, (display_.height() - h) / 2);
  display_.print(text);
}
