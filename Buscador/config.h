#pragma once

#include <Arduino.h>

namespace Config {
  // --- Identidad del tesoro (tiene que coincidir con Tesoro.ino) ---
  constexpr char TREASURE_UUID[] = "7e2a1c4b-3f9d-4e8a-9b21-5c6d0f3a8e17";

  // --- Serial ---
  constexpr unsigned long SERIAL_BAUD           = 9600;
  constexpr unsigned long DEBUG_LOG_INTERVAL_MS = 500;

  // --- Escaneo BLE (ms) ---
  constexpr uint16_t SCAN_INTERVAL_MS = 100;
  constexpr uint16_t SCAN_WINDOW_MS   = 100;  // igual al intervalo = escucha continua

  // --- Señal ---
  constexpr float         EMA_ALPHA         = 0.25f;  // más bajo = más suave pero más lento
  constexpr unsigned long SIGNAL_TIMEOUT_MS = 3000;   // sin lecturas por este tiempo = SIN SEÑAL

  // --- Umbrales de proximidad (dBm, calibrar en el lugar) ---
  constexpr float THRESHOLD_FRIO      = -110.0f;
  constexpr float THRESHOLD_TIBIO      = -80.0f;
  constexpr float THRESHOLD_CALIENTE   = -68.0f;
  constexpr float THRESHOLD_ENCONTRADO = -35.0f;
  constexpr float HYSTERESIS_DB        = 3.0f;

  // --- Juego ---
  constexpr bool LATCH_FOUND = true;  // una vez encontrado, queda encontrado hasta reiniciar

  // --- Pines (ajustar según tu placa) ---
  constexpr uint8_t BUZZER_PIN = 25;  // buzzer activo (desactivado: se usa la OLED)

  // OLED SSD1306 0,96" por SPI
  constexpr int8_t   OLED_MOSI   = 2;
  constexpr int8_t   OLED_CLK    = 1;
  constexpr int8_t   OLED_CS     = 3;
  constexpr int8_t   OLED_DC     = 6;
  constexpr int8_t   OLED_RST    = 7;
  constexpr uint8_t  OLED_WIDTH  = 128;
  constexpr uint8_t  OLED_HEIGHT = 64;
  constexpr uint8_t LED_PINS[] = {5, 13, 14};
  constexpr size_t  LED_COUNT  = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

  // --- Feedback ---
  constexpr unsigned long BEEP_DURATION_MS   = 50;
  constexpr unsigned long BEEP_PERIOD_FRIO_MS     = 1200;
  constexpr unsigned long BEEP_PERIOD_TIBIO_MS    = 600;
  constexpr unsigned long BEEP_PERIOD_CALIENTE_MS = 250;
  constexpr unsigned long BEEP_PERIOD_ENCONTRADO_MS = 100;
  constexpr unsigned long FOUND_BLINK_MS     = 200;
  constexpr unsigned long OLED_REFRESH_MS    = 150;  // máx. ~6 refrescos/s
}
