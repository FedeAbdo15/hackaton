#pragma once

#include "BleScanner.h"
#include "Proximity.h"
#include "ProximityClassifier.h"
#include "SignalFilter.h"

// Lógica del juego: filtra la señal, clasifica la proximidad y maneja
// la pérdida de señal y el estado "encontrado".
class GameState {
public:
  GameState();

  Proximity update(const RssiReading& reading, unsigned long now);

  Proximity proximity() const { return current_; }
  float filteredRssi() const { return filter_.value(); }
  bool hasSignal() const { return filter_.hasValue(); }

private:
  bool isSignalLost(const RssiReading& reading, unsigned long now) const;
  void loseSignal();

  SignalFilter filter_;
  ProximityClassifier classifier_;
  Proximity current_ = Proximity::SinSenal;
  bool found_ = false;
};
