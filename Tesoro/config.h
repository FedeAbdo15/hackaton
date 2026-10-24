#pragma once

#include <Arduino.h>

// Toda la configuración del Tesoro en un solo lugar.
// Cambiar pines, tiempos o potencia acá, sin tocar los módulos.
namespace Config {
  // --- Identidad BLE (tienen que coincidir con Buscador/config.h) ---
  constexpr char TREASURE_UUID[] = "7e2a1c4b-3f9d-4e8a-9b21-5c6d0f3a8e17";
  constexpr char SEEKER_UUID[]   = "c3f8a2d1-6b4e-4d9a-8c15-9e0b3f7a2d61";
  constexpr char DEVICE_NAME[]   = "Tesoro";

  // --- Serial ---
  constexpr unsigned long SERIAL_BAUD = 9600;

  // --- Beacon BLE ---
  constexpr int8_t TX_POWER_DBM = 9;

  // Unidades de 0.625 ms (80 = 50 ms, 160 = 100 ms)
  constexpr uint16_t ADV_INTERVAL_MIN = 80;
  constexpr uint16_t ADV_INTERVAL_MAX = 160;

  constexpr unsigned long HEALTH_CHECK_INTERVAL_MS = 5000;  // cada cuánto se verifica el advertising

  // --- Escaneo BLE del Buscador ---
  constexpr uint16_t      SCAN_INTERVAL_MS  = 100;
  constexpr uint16_t      SCAN_WINDOW_MS    = 50;    // deja ventana de radio para transmitir el beacon
  constexpr unsigned long SEEKER_TIMEOUT_MS = 5000;  // sin señal del Buscador por este tiempo = encontrado

  // --- OLED SSD1306 0,96" por SPI ---
  constexpr int8_t  OLED_MOSI     = 2;
  constexpr int8_t  OLED_CLK      = 1;
  constexpr int8_t  OLED_CS       = 3;
  constexpr int8_t  OLED_DC       = 6;
  constexpr int8_t  OLED_RST      = 7;
  constexpr uint8_t OLED_WIDTH    = 128;
  constexpr uint8_t OLED_HEIGHT   = 64;
  constexpr uint8_t OLED_ROTATION = 2;  // 0 = normal, 2 = 180°

  // --- Botón N1 (entre el pin y GND, con pull-up interno) ---
  constexpr uint8_t       BUTTON_N1_PIN      = 19;
  constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;

  // --- Timer ---
  constexpr unsigned long TIMER_TICK_MS         = 1000;
  constexpr int32_t       TIMER_START_SECONDS   = -5;

  // --- Buzzer ---
  constexpr uint8_t       BUZZER_PIN         = 25;
  constexpr unsigned long BUZZER_INTERVAL_MS = 5000;  // cada 5 s
  constexpr unsigned long BUZZER_BEEP_MS     = 150;   // duración del pitido
  constexpr unsigned int  BUZZER_FREQ_HZ     = 2000;
}
