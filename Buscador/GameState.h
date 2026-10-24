#pragma once

#include "EspNowScanner.h"
#include "Proximity.h"
#include "ProximityClassifier.h"
#include "SignalFilter.h"

enum class GamePhase : uint8_t {
  WaitingStart,  // Esperando que Jugador 2 presione BOTÓN N2
  NotReady,      // Presionó BOTÓN N2 sin que Tesoro transmita ("Tesoro no preparado")
  Starting,      // "Juego comenzado" por 5 segundos
  Playing,       // En juego: mostrando distancia y animación por threshold
  Victory        // Distancia == 0: pantalla de victoria con animación según tiempo
};

// Lógica del juego: maneja las fases (inicio con BOTÓN N2, espera de 5s, búsqueda y victoria),
// filtra la señal y clasifica la proximidad.
class GameState {
public:
  GameState();

  // Procesa la pulsación del BOTÓN N2. Devuelve true si el juego arrancó válidamente
  // (para que se envíe MsgType::GameStart al Tesoro).
  bool onButtonN2Pressed(unsigned long now);

  Proximity update(const RssiReading& reading, unsigned long now);

  // Devuelve true una única vez en el instante en que se encuentra el Tesoro
  bool consumeJustFound();

  GamePhase phase() const { return phase_; }
  Proximity proximity() const { return current_; }
  float     filteredRssi() const { return filter_.value(); }
  bool      hasSignal() const { return filter_.hasValue(); }
  bool      isTreasureReady() const { return treasureReady_; }
  int       distance() const;
  uint32_t  victorySeconds() const { return victorySeconds_; }
  uint8_t   startingSecondsLeft(unsigned long now) const;

private:
  bool isSignalLost(const RssiReading& reading, unsigned long now) const;
  void loseSignal();

  SignalFilter        filter_;
  ProximityClassifier classifier_;
  GamePhase           phase_          = GamePhase::WaitingStart;
  Proximity           current_        = Proximity::SinSenal;
  bool                treasureReady_  = false;
  bool                justFound_      = false;
  unsigned long       phaseStartMs_   = 0;
  unsigned long       playStartMs_    = 0;
  uint32_t            victorySeconds_ = 0;
};
