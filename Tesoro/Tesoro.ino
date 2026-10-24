/*
 * Tesoro — Búsqueda del tesoro BLE
 *
 * Transmite un beacon BLE con el UUID del tesoro para que el Buscador lo encuentre.
 * Además tiene un cronómetro en la OLED y un buzzer que pita cada 5 s, ambos
 * controlados por dos botones.
 *
 * Flujo de eventos:
 *  - Al encender:        arranca el beacon y la OLED muestra 00:00:00 detenido.
 *  - Botón START (19):   inicia/reanuda el timer y el buzzer.
 *  - Botón STOP  (18):   pausa el timer y el buzzer (el tiempo queda en pantalla).
 *  - Cada 1 s:           el timer suma un segundo.
 *  - Cada 5 s:           pitido del buzzer (si está activo) y health check del beacon.
 *
 * Requiere:
 *  - Placa: esp32 by Espressif Systems
 *  - Biblioteca: NimBLE-Arduino (h2zero) v2.x
 *  - Bibliotecas: Adafruit SSD1306 + Adafruit GFX
 */

#include "config.h"
#include "TreasureBeacon.h"
#include "TimerDisplay.h"
#include "Buzzer.h"
#include "Button.h"

TreasureBeacon beacon;
TimerDisplay   timerDisplay;
Buzzer         buzzer;
Button         startButton(Config::BUTTON_START_PIN);
Button         stopButton(Config::BUTTON_STOP_PIN);

// Evento: botón START → arranca timer y buzzer juntos (si no estaban corriendo)
void onStartPressed(unsigned long now) {
  if (timerDisplay.isRunning()) {
    return;
  }
  timerDisplay.start(now);
  buzzer.start(now);
  Serial.println("[Tesoro] Timer y buzzer iniciados");
}

// Evento: botón STOP → pausa timer y buzzer (si estaban corriendo)
void onStopPressed() {
  if (!timerDisplay.isRunning()) {
    return;
  }
  timerDisplay.stop();
  buzzer.stop();
  Serial.print("[Tesoro] Timer y buzzer detenidos en ");
  Serial.print(timerDisplay.seconds());
  Serial.println(" s");
}

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  Serial.println("[Tesoro] Iniciando...");

  initLeds();
  startButton.begin();
  stopButton.begin();
  buzzer.begin();
  timerDisplay.begin();
  beacon.begin();

  Serial.println("[Tesoro] Listo. Apretá START para iniciar el timer.");
}

void loop() {
  const unsigned long now = millis();

  // 1) Entradas
  if (startButton.wasPressed(now)) {
    onStartPressed(now);
  }
  if (stopButton.wasPressed(now)) {
    onStopPressed();
  }

  // 2) Módulos (todos no bloqueantes)
  timerDisplay.update(now);
  buzzer.update(now);
  beacon.update(now);
}
