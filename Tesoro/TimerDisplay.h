#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Cronómetro en la OLED SSD1306 128x64 (0,96") por SPI, formato HH:MM:SS.
//
// Eventos:
//  - begin():  inicia la pantalla y muestra 00:00:00 detenido.
//              Si la OLED falla, el resto del Tesoro sigue funcionando.
//  - start():  empieza el conteo desde TIMER_START_SECONDS (-5 s: -00:00:05).
//  - stop():   pausa el conteo; la pantalla queda mostrando el último tiempo.
//  - update(): suma 1 s cada TIMER_TICK_MS y redibuja solo en ese momento.
class TimerDisplay {
public:
  TimerDisplay();

  bool begin();
  void start(unsigned long now);
  void stop();
  void update(unsigned long now);  // no bloqueante

  bool    isRunning() const { return running_; }
  int32_t seconds()   const { return seconds_; }

private:
  void draw();
  void printCentered(const char* text, uint8_t size);

  Adafruit_SSD1306 display_;
  bool          ready_      = false;
  bool          running_    = false;
  int32_t       seconds_    = 0;
  unsigned long lastTickMs_ = 0;
};
