/*
 * Tesoro — Búsqueda del tesoro BLE
 *
 * Flujo de juego en el Tesoro:
 *  1. Al encender:        la OLED muestra 00:00:00 detenido; el beacon no transmite aún.
 *  2. Botón N1 (GPIO 19): Jugador 1 coloca el Tesoro y presiona N1 → inicia la transmisión
 *                         del beacon BLE y queda escuchando la señal del Buscador.
 *  3. Inicio de juego:    cuando el Buscador presiona Botón N2 y comienza a transmitir su señal,
 *                         el Tesoro la detecta, inicia el timer en -00:00:05 (+1s cada segundo)
 *                         y activa el buzzer cada 5s (primer pitido en 00:00:00).
 *  4. Tesoro encontrado:  cuando el Buscador encuentra el Tesoro y deja de transmitir,
 *                         el Tesoro detiene el timer y el buzzer.
 *
 * Requiere:
 *  - Placa: esp32 by Espressif Systems
 *  - Biblioteca: NimBLE-Arduino (h2zero) v2.x
 *  - Bibliotecas: Adafruit SSD1306 + Adafruit GFX
 */

#include "config.h"
#include "TreasureBeacon.h"
#include "SeekerScanner.h"
#include "TimerDisplay.h"
#include "Buzzer.h"
#include "Button.h"

enum class TreasureState {
  Idle,          // Esperando que Jugador 1 presione BOTÓN N1
  WaitingSeeker, // Transmitiendo beacon y esperando señal del Buscador (BOTÓN N2)
  Running,       // Juego en curso: timer desde -5s y buzzer cada 5s
  Found          // Tesoro encontrado: timer y buzzer detenidos
};

TreasureBeacon beacon;
SeekerScanner  seekerScanner(Config::SEEKER_UUID);
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
  seekerScanner.start();
  state = TreasureState::WaitingSeeker;
  Serial.println("[Tesoro] BOTON N1 presionado: transmitiendo y esperando al Buscador...");
}

// Evento: el Buscador comenzó a transmitir (Jugador 2 presionó BOTÓN N2)
void onGameStarted(unsigned long now) {
  timerDisplay.start(now);
  buzzer.start(now);
  state = TreasureState::Running;
  Serial.println("[Tesoro] Buscador detectado: timer iniciado en -5s y buzzer activo");
}

// Evento: el Buscador dejó de transmitir (encontró el Tesoro)
void onTreasureFound() {
  timerDisplay.stop();
  buzzer.stop();
  seekerScanner.stop();
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
  seekerScanner.begin();

  Serial.println("[Tesoro] Listo. Presiona BOTON N1 para comenzar a transmitir.");
}

void loop() {
  const unsigned long now = millis();

  // 1) Entradas y transiciones de estado
  if (buttonN1.wasPressed(now)) {
    onButtonN1Pressed();
  }

  if (state == TreasureState::WaitingSeeker && seekerScanner.hasStarted()) {
    onGameStarted(now);
  } else if (state == TreasureState::Running && seekerScanner.hasStopped(now)) {
    onTreasureFound();
  }

  // 2) Módulos (todos no bloqueantes)
  timerDisplay.update(now);
  buzzer.update(now);
  beacon.update(now);
}
