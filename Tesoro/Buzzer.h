#pragma once

#include <Arduino.h>

// Buzzer que pita cada BUZZER_INTERVAL_MS mientras está activo.
//
// Eventos:
//  - start(): activa los pitidos. El primero suena a los BUZZER_INTERVAL_MS
//             (queda sincronizado con el timer: 00:00:05, 00:00:10...).
//  - stop():  desactiva los pitidos y corta el sonido si estaba sonando.
//  - update(): enciende el tono al cumplirse el intervalo y lo apaga
//              después de BUZZER_BEEP_MS.
class Buzzer {
public:
  void begin();
  void start(unsigned long now);
  void stop();
  void update(unsigned long now);  // no bloqueante

  bool isRunning() const { return running_; }

private:
  void beepOn(unsigned long now);
  void beepOff();

  unsigned long lastBeepMs_  = 0;
  unsigned long beepStartMs_ = 0;
  bool          beeping_     = false;
  bool          running_     = false;
};
