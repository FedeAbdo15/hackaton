#pragma once

#include <Arduino.h>

// Botón con pull-up interno y antirrebote. Conectar entre el pin y GND:
// suelto lee HIGH, apretado lee LOW.
//
// Eventos:
//  - wasPressed(): devuelve true una sola vez por cada apretada, recién cuando
//                  la lectura se mantuvo estable BUTTON_DEBOUNCE_MS.
//                  Soltar el botón no genera evento.
class Button {
public:
  explicit Button(uint8_t pin);

  void begin();
  bool wasPressed(unsigned long now);  // llamar en cada loop()

  bool isPressed() const { return stableState_ == LOW; }

private:
  const uint8_t pin_;
  bool          stableState_  = HIGH;
  bool          lastReading_  = HIGH;
  unsigned long lastChangeMs_ = 0;
};
