#pragma once

#include "Proximity.h"

// Interfaz común para toda salida al jugador (buzzer, LEDs, Serial, OLED, vibración...).
// Agregar un tipo nuevo de feedback es sumar una clase, sin tocar la lógica del juego.
class Feedback {
public:
  virtual ~Feedback() = default;

  virtual void begin() = 0;
  virtual void update(Proximity proximity, unsigned long now) = 0;  // no bloqueante
};
