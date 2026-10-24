#include "Buzzer.h"
#include "config.h"

void Buzzer::begin() {
  pinMode(Config::BUZZER_PIN, OUTPUT);
}

void Buzzer::start(unsigned long now) {
  if (running_) {
    return;
  }
  running_    = true;
  lastBeepMs_ = now;
}

void Buzzer::stop() {
  running_ = false;
  beepOff();
}

void Buzzer::update(unsigned long now) {
  if (!running_) {
    return;
  }

  // Fin del pitido actual
  if (beeping_ && now - beepStartMs_ >= Config::BUZZER_BEEP_MS) {
    beepOff();
  }

  // Inicio del próximo pitido
  if (now - lastBeepMs_ >= Config::BUZZER_INTERVAL_MS) {
    lastBeepMs_ += Config::BUZZER_INTERVAL_MS;  // evita deriva acumulada
    beepOn(now);
  }
}

void Buzzer::beepOn(unsigned long now) {
  tone(Config::BUZZER_PIN, Config::BUZZER_FREQ_HZ);
  beepStartMs_ = now;
  beeping_     = true;
}

void Buzzer::beepOff() {
  if (!beeping_) {
    return;
  }
  noTone(Config::BUZZER_PIN);
  beeping_ = false;
}
