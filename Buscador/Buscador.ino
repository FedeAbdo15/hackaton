/*
 * Buscador — Búsqueda del tesoro por ESP-NOW
 *
 * Flujo de juego en el Buscador:
 *  1. Espera a que el Jugador 2 presione el BOTÓN N2 (GPIO 19).
 *  2. Si el Tesoro aún no transmite (Jugador 1 no presionó BOTÓN N1):
 *       -> Muestra en pantalla "Tesoro no preparado".
 *     Si el Tesoro ya transmite:
 *       -> Envía comando GameStart por ESP-NOW al Tesoro.
 *       -> Muestra "Juego comenzado" por 5 segundos.
 *  3. Durante la búsqueda:
 *       -> Filtra el RSSI de los paquetes ESP-NOW del Tesoro.
 *       -> Muestra en la OLED la distancia hacia el Tesoro (0 a 100) y una animación
 *          dependiendo del threshold (Frío, Tibio, Caliente).
 *  4. Cuando la distancia es 0 (Proximity::Encontrado):
 *       -> Envía comando TreasureFound por ESP-NOW al Tesoro para detener timer y buzzer.
 *       -> Muestra pantalla de victoria con animación según el tiempo tardado.
 */

#include "config.h"
#include "BleScanner.h"
#include "Button.h"
#include "GameState.h"
#include "OledFeedback.h"
#include "SerialFeedback.h"

BleScanner     scanner;
Button         buttonN2(Config::BUTTON_N2_PIN);
GameState      game;

OledFeedback   oled(game);
SerialFeedback serialFeedback;

Feedback* feedbacks[] = { &oled, &serialFeedback };

unsigned long lastDebugLogMs = 0;

// RSSI crudo vs filtrado, para calibrar umbrales y EMA_ALPHA en el lugar
void logDebug(const RssiReading& reading, unsigned long now) {
  if (now - lastDebugLogMs < Config::DEBUG_LOG_INTERVAL_MS) {
    return;
  }
  lastDebugLogMs = now;

  if (!game.hasSignal() || game.phase() != GamePhase::Playing) {
    return;
  }
  Serial.print("[Buscador] RSSI crudo: ");
  Serial.print(reading.rssi);
  Serial.print(" | filtrado: ");
  Serial.print(game.filteredRssi(), 1);
  Serial.print(" | dist: ");
  Serial.print(game.distance());
  Serial.print(" | ");
  Serial.println(toString(game.proximity()));
}

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  Serial.println("[Buscador] Iniciando...");

  buttonN2.begin();
  for (Feedback* feedback : feedbacks) {
    feedback->begin();
  }
  scanner.begin();

  Serial.println("[Buscador] Listo. Presiona BOTON N2 para iniciar el juego.");
}

void loop() {
  const unsigned long now = millis();

  // 1) Lectura de señal del Tesoro y actualización de estado
  const RssiReading reading   = scanner.read();
  const Proximity   proximity = game.update(reading, now);

  // 2) Botón N2 (Iniciar juego)
  if (buttonN2.wasPressed(now)) {
    if (game.onButtonN2Pressed(now)) {
      scanner.sendGameStart();
    } else if (game.phase() == GamePhase::NotReady) {
      Serial.println("[Buscador] BOTON N2 presionado: Tesoro no preparado");
    }
  }

  // 3) Notificar al Tesoro apenas se encuentra (distancia == 0)
  if (game.consumeJustFound()) {
    scanner.sendTreasureFound();
  }

  // 4) Salidas (OLED + Serial)
  for (Feedback* feedback : feedbacks) {
    feedback->update(proximity, now);
  }

  logDebug(reading, now);
}
