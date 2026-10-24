#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pantalla OLED SSD1306 128x64 (0,96") por SPI para el Tesoro:
//  - begin():             muestra "Presione el boton 1 / para empezar".
//  - showWaitingSeeker(): muestra pantalla de espera tras presionar el botón 1.
//  - start():             inicia cuenta regresiva ("El juego comienza en 5..4..3..2..1.")
//                         y luego muestra el reloj animado haciendo tick-tack con el
//                         reloj digital en el centro.
//  - stop():              detiene el conteo y la animación dejando el tiempo final en pantalla.
class TimerDisplay {
public:
  TimerDisplay();

  bool begin();
  void showWaitingSeeker();
  void start(unsigned long now);
  void stop();
  void update(unsigned long now);  // no bloqueante

  bool    isRunning() const { return running_; }
  int32_t seconds()   const { return seconds_; }

private:
  void draw();
  void drawStartPrompt();
  void drawWaitingSeeker();
  void drawCountdown();
  void drawClockWithDigitalCenter();
  void printCentered(const char* text, uint8_t size, int16_t y);

  Adafruit_SSD1306 display_;
  bool          ready_         = false;
  bool          running_       = false;
  bool          startedOnce_   = false;
  bool          waitingSeeker_ = false;
  int32_t       seconds_       = 0;
  uint8_t       animFrame_     = 0;
  unsigned long lastTickMs_    = 0;
  unsigned long lastAnimMs_    = 0;
};
