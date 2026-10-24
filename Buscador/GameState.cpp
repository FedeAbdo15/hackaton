#include "GameState.h"
#include "config.h"

GameState::GameState() : filter_(Config::EMA_ALPHA) {}

bool GameState::onButtonN2Pressed(unsigned long now) {
  if (phase_ != GamePhase::WaitingStart && phase_ != GamePhase::NotReady) {
    return false;
  }

  if (!treasureReady_) {
    phase_        = GamePhase::NotReady;
    phaseStartMs_ = now;
    return false;
  }

  filter_.reset();
  classifier_.reset();
  current_      = Proximity::Frio;
  phase_        = GamePhase::Starting;
  phaseStartMs_ = now;
  return true;
}

Proximity GameState::update(const RssiReading& reading, unsigned long now) {
  treasureReady_ = !isSignalLost(reading, now);

  if (phase_ == GamePhase::Victory) {
    return Proximity::Encontrado;
  }

  if (phase_ == GamePhase::NotReady) {
    if (now - phaseStartMs_ >= Config::NOT_READY_MSG_MS) {
      phase_ = GamePhase::WaitingStart;
    }
    return Proximity::SinSenal;
  }

  if (phase_ == GamePhase::WaitingStart) {
    return Proximity::SinSenal;
  }

  if (phase_ == GamePhase::Starting) {
    if (reading.fresh) {
      current_ = classifier_.classify(filter_.update(reading.rssi));
    }
    if (now - phaseStartMs_ >= Config::START_COUNTDOWN_MS) {
      phase_       = GamePhase::Playing;
      playStartMs_ = now;
    }
    return current_;
  }

  // GamePhase::Playing
  if (!treasureReady_) {
    loseSignal();
    return current_;
  }

  if (reading.fresh) {
    const float smoothed = filter_.update(reading.rssi);
    current_ = classifier_.classify(smoothed);

    if (current_ == Proximity::Encontrado) {
      phase_     = GamePhase::Victory;
      justFound_ = true;
      if (reading.treasureSeconds > 0) {
        victorySeconds_ = static_cast<uint32_t>(reading.treasureSeconds);
      } else {
        victorySeconds_ = static_cast<uint32_t>((now - playStartMs_) / 1000UL);
      }
    }
  }

  return current_;
}

bool GameState::consumeJustFound() {
  const bool val = justFound_;
  justFound_ = false;
  return val;
}

int GameState::distance() const {
  if (current_ == Proximity::Encontrado || phase_ == GamePhase::Victory) {
    return 0;
  }
  if (!hasSignal() || current_ == Proximity::SinSenal) {
    return -1;
  }
  const int rssi = static_cast<int>(lroundf(filter_.value()));
  const long mapped = map(-rssi, 35, 90, 0, 100);
  if (mapped < 1)   return 1;
  if (mapped > 100) return 100;
  return static_cast<int>(mapped);
}

uint8_t GameState::startingSecondsLeft(unsigned long now) const {
  if (phase_ != GamePhase::Starting) {
    return 0;
  }
  const unsigned long elapsed = now - phaseStartMs_;
  if (elapsed >= Config::START_COUNTDOWN_MS) {
    return 0;
  }
  return static_cast<uint8_t>((Config::START_COUNTDOWN_MS - elapsed + 999UL) / 1000UL);
}

bool GameState::isSignalLost(const RssiReading& reading, unsigned long now) const {
  if (!reading.everSeen) {
    return true;
  }
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
