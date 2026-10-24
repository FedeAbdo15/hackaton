/*
 * Tesoro — Búsqueda del tesoro por ESP-NOW
 *
 * Flujo de juego en el Tesoro:
 *  1. Al encender:        la OLED muestra 00:00:00 detenido; no transmite aún.
 *  2. Botón N1 (GPIO 19): Jugador 1 coloca el Tesoro y presiona N1 → inicia la transmisión
 *                         de paquetes TreasureBeacon por ESP-NOW y queda esperando al Buscador.
 *  3. Inicio de juego:    cuando el Buscador presiona Botón N2 y envía GameStart por ESP-NOW,
 *                         el Tesoro inicia el timer en -00:00:05 (+1s cada segundo)
 *                         y activa el buzzer cada 5s (primer pitido en 00:00:00).
 *  4. Tesoro encontrado:  cuando el Buscador llega a distancia 0 y envía TreasureFound por ESP-NOW,
 *                         el Tesoro detiene al instante el timer y el buzzer.
 */

#include "config.h"
#include "TreasureBeacon.h"
#include "TimerDisplay.h"
#include "Buzzer.h"
#include "Button.h"

enum class TreasureState {
  Idle,          // Esperando que Jugador 1 presione BOTÓN N1
  WaitingSeeker, // Transmitiendo por ESP-NOW y esperando GameStart del Buscador (BOTÓN N2)
  Running,       // Juego en curso: timer desde -5s y buzzer cada 5s
  Found          // Tesoro encontrado: timer y buzzer detenidos
};

TreasureBeacon beacon;
TimerDisplay   timerDisplay;
Buzzer         buzzer;
Button         buttonN1(Config::BUTTON_N1_PIN);

TreasureState state = TreasureState::Idle;

// Evento: Jugador 1 presiona BOTÓN N1 → comienza a transmitir y espera al Buscador
void onButtonN1Pressed() {
  if (state != TreasureState::Idle && state != TreasureState::Found) {
    return;
  }
  beacon.start();
  state = TreasureState::WaitingSeeker;
  Serial.println("[Tesoro] BOTON N1 presionado: transmitiendo y esperando al Buscador...");
}

// Evento: el Buscador envió GameStart (Jugador 2 presionó BOTÓN N2)
void onGameStarted(unsigned long now) {
  timerDisplay.start(now);
  buzzer.start(now);
  state = TreasureState::Running;
  Serial.println("[Tesoro] GameStart recibido: timer iniciado en -5s y buzzer activo");
}

// Evento: el Buscador envió TreasureFound (distancia == 0)
void onTreasureFound() {
  timerDisplay.stop();
  buzzer.stop();
  beacon.stop();
  state = TreasureState::Found;
  Serial.print("[Tesoro] Tesoro encontrado! Timer y buzzer detenidos en ");
  Serial.print(timerDisplay.seconds());
  Serial.println(" s");
}

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  Serial.println("[Tesoro] Iniciando...");

  buttonN1.begin();
  buzzer.begin();
  timerDisplay.begin();
  beacon.begin();

  Serial.println("[Tesoro] Listo. Presiona BOTON N1 para comenzar a transmitir.");
}

void loop() {
  const unsigned long now = millis();

  // 1) Entradas y eventos ESP-NOW
  if (buttonN1.wasPressed(now)) {
    onButtonN1Pressed();
  }

  if (state == TreasureState::WaitingSeeker && beacon.consumeGameStart()) {
    onGameStarted(now);
  } else if (state == TreasureState::Running && beacon.consumeTreasureFound()) {
    onTreasureFound();
  }

  // 2) Módulos (todos no bloqueantes)
  timerDisplay.update(now);
  buzzer.update(now);
  beacon.update(now, timerDisplay.seconds());
}
