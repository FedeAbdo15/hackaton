#include "ProximityClassifier.h"
#include "config.h"

Proximity ProximityClassifier::classify(float rssi) {
  const Proximity raw = levelFor(rssi, 0.0f);

  if (raw > current_) {
    current_ = raw;
  } else if (raw < current_) {
    // Para bajar, se exige caer por debajo del umbral menos la histéresis
    const Proximity withHysteresis = levelFor(rssi, Config::HYSTERESIS_DB);
    current_ = withHysteresis < current_ ? withHysteresis : current_;
  }

  return current_;
}

void ProximityClassifier::reset() {
  current_ = Proximity::Frio;
}

Proximity ProximityClassifier::levelFor(float rssi, float margin) {
  if (rssi >= Config::THRESHOLD_ENCONTRADO - margin) return Proximity::Encontrado;
  if (rssi >= Config::THRESHOLD_CALIENTE - margin)   return Proximity::Caliente;
  if (rssi >= Config::THRESHOLD_TIBIO - margin)      return Proximity::Tibio;
  return Proximity::Frio;
}
