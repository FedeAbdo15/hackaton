#include "Button.h"
#include "config.h"

Button::Button(uint8_t pin) : pin_(pin) {}

void Button::begin() {
  pinMode(pin_, INPUT_PULLUP);
  stableState_ = digitalRead(pin_);
  lastReading_ = stableState_;
}

bool Button::wasPressed(unsigned long now) {
  const bool reading = digitalRead(pin_);

  // Cualquier cambio reinicia la ventana de antirrebote
  if (reading != lastReading_) {
    lastChangeMs_ = now;
    lastReading_  = reading;
  }

  const bool stable = now - lastChangeMs_ >= Config::BUTTON_DEBOUNCE_MS;
  if (!stable || reading == stableState_) {
    return false;
  }

  stableState_ = reading;
  return stableState_ == LOW;  // solo cuenta la transición a apretado
}
