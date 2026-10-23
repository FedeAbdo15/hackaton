#pragma once

#include "Feedback.h"

// Informa por Serial cada vez que cambia el nivel de proximidad.
class SerialFeedback : public Feedback {
public:
  void begin() override {}

  void update(Proximity proximity, unsigned long now) override {
    if (hasReported_ && proximity == lastReported_) {
      return;
    }
    Serial.print("[Buscador] Estado: ");
    Serial.println(toString(proximity));
    lastReported_ = proximity;
    hasReported_ = true;
  }

private:
  Proximity lastReported_ = Proximity::SinSenal;
  bool hasReported_ = false;
};
