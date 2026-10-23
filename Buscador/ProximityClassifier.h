#pragma once

#include "Proximity.h"

// Convierte RSSI filtrado en nivel de proximidad.
// Sube de nivel apenas supera el umbral; baja solo si cae HYSTERESIS_DB por debajo,
// así no parpadea cuando el jugador está justo en el borde.
class ProximityClassifier {
public:
  Proximity classify(float rssi);
  void reset();

private:
  static Proximity levelFor(float rssi, float margin);

  Proximity current_ = Proximity::Frio;
};
