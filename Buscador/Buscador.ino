/*
 * Buscador — Búsqueda del tesoro BLE
 *
 * Escucha el beacon del Tesoro, filtra el RSSI y muestra en la OLED el RSSI filtrado;
 * además transmite su propio beacon BLE al iniciar con el botón 1.
 *
 * Requiere:
 *  - Placa: esp32 by Espressif Systems
 *  - Biblioteca: NimBLE-Arduino (h2zero) v2.x
 *  - Bibliotecas: Adafruit SSD1306 + Adafruit GFX
 */

#include "config.h"
#include "BleScanner.h"
#include "TreasureBeacon.h"
#include "Button.h"
#include "GameState.h"
#include "OledFeedback.h"
#include "SerialFeedback.h"

BleScanner     scanner(Config::TREASURE_UUID);
TreasureBeacon beacon(Config::BEACON_UUID);
Button         startButton(Config::BUTTON_START_PIN);
GameState      game;

OledFeedback   oled(game);
SerialFeedback serialFeedback;

Feedback* feedbacks[] = { &oled, &serialFeedback };

bool          searching      = false;
unsigned long lastDebugLogMs = 0;

// RSSI crudo vs filtrado, para calibrar umbrales y EMA_ALPHA en el lugar
void logDebug(const RssiReading& reading, unsigned long now) {
  if (now - lastDebugLogMs < Config::DEBUG_LOG_INTERVAL_MS) {
    return;
  }
  lastDebugLogMs = now;

  if (!game.hasSignal()) {
    return;
  }
  Serial.print("[Buscador] RSSI crudo: ");
  Serial.print(reading.rssi);
  Serial.print(" | filtrado: ");
  Serial.print(game.filteredRssi(), 1);
  Serial.print(" | ");
  Serial.println(toString(game.proximity()));
}

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  Serial.println("[Buscador] Iniciando...");

  startButton.begin();
  for (Feedback* feedback : feedbacks) {
    feedback->begin();
  }

  Serial.println("[Buscador] Listo. Apretá START para buscar y transmitir.");
}

void loop() {
  const unsigned long now = millis();

  if (!searching) {
    if (startButton.wasPressed(now)) {
      searching = true;
      beacon.begin();
      scanner.begin();
      Serial.println("[Buscador] Buscando el tesoro y transmitiendo beacon...");
    }
    return;
  }

  beacon.update(now);


  const RssiReading reading = scanner.read();
  const Proximity proximity = game.update(reading, now);

  for (Feedback* feedback : feedbacks) {
    feedback->update(proximity, now);
  }

  logDebug(reading, now);
}
