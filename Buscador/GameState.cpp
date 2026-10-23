#include "GameState.h"
#include "config.h"

GameState::GameState() : filter_(Config::EMA_ALPHA) {}

Proximity GameState::update(const RssiReading& reading, unsigned long now) {
  if (found_ && Config::LATCH_FOUND) {
    return current_;
  }

  if (isSignalLost(reading, now)) {
    loseSignal();
    return current_;
  }

  if (reading.fresh) {
    const float smoothed = filter_.update(reading.rssi);
    current_ = classifier_.classify(smoothed);
    found_ = (current_ == Proximity::Encontrado);
  }

  return current_;
}

bool GameState::isSignalLost(const RssiReading& reading, unsigned long now) const {
  if (!reading.everSeen) {
    return true;
  }
  // lastSeenMs puede ser apenas mayor que now (se escribe desde otra tarea)
  if (reading.lastSeenMs >= now) {
    return false;
  }
  return now - reading.lastSeenMs > Config::SIGNAL_TIMEOUT_MS;
}

void GameState::loseSignal() {
  filter_.reset();
  classifier_.reset();
  current_ = Proximity::SinSenal;
}
