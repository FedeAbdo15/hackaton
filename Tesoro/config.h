#pragma once

#include <Arduino.h>

// Toda la configuración del Tesoro en un solo lugar.
// Cambiar pines, tiempos o potencia acá, sin tocar los módulos.
namespace Config {
  // --- Identidad del tesoro (tiene que coincidir con Buscador/config.h) ---
  constexpr char TREASURE_UUID[] = "7e2a1c4b-3f9d-4e8a-9b21-5c6d0f3a8e17";
  constexpr char DEVICE_NAME[]   = "Tesoro";

  // --- Serial ---
  constexpr unsigned long SERIAL_BAUD = 9600;

  // --- Beacon BLE ---
  constexpr int8_t TX_POWER_DBM = 9;

  // Unidades de 0.625 ms (80 = 50 ms, 160 = 100 ms)
  constexpr uint16_t ADV_INTERVAL_MIN = 80;
  constexpr uint16_t ADV_INTERVAL_MAX = 160;

  constexpr unsigned long HEALTH_CHECK_INTERVAL_MS = 5000;  // cada cuánto se verifica el advertising

  // --- OLED SSD1306 0,96" por SPI ---
  constexpr int8_t  OLED_MOSI     = 2;
  constexpr int8_t  OLED_CLK      = 1;
  constexpr int8_t  OLED_CS       = 3;
  constexpr int8_t  OLED_DC       = 6;
  constexpr int8_t  OLED_RST      = 7;
  constexpr uint8_t OLED_WIDTH    = 128;
  constexpr uint8_t OLED_HEIGHT   = 64;
  constexpr uint8_t OLED_ROTATION = 2;  // 0 = normal, 2 = 180°

  // --- Botones (entre el pin y GND, con pull-up interno) ---
  constexpr uint8_t       BUTTON_START_PIN   = 19;
  constexpr uint8_t       BUTTON_STOP_PIN    = 18;
  constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;

  // --- Timer ---
  constexpr unsigned long TIMER_TICK_MS = 1000;

  // --- Buzzer ---
  constexpr uint8_t       BUZZER_PIN         = 25;    // ojo: en ESP32-S3 el GPIO 25 no existe
  constexpr unsigned long BUZZER_INTERVAL_MS = 5000;  // cada 5 s
  constexpr unsigned long BUZZER_BEEP_MS     = 150;   // duración del pitido
  constexpr unsigned int  BUZZER_FREQ_HZ     = 2000;
}
