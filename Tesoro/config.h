#pragma once

#include <Arduino.h>

// Toda la configuración del Tesoro en un solo lugar.
// Cambiar pines, tiempos o canal acá, sin tocar los módulos.
namespace Config {
  // --- Protocolo ESP-NOW (tiene que coincidir con Buscador/config.h) ---
  constexpr uint32_t      PACKET_MAGIC       = 0x5445534F;  // "TESO"
  constexpr uint8_t       ESPNOW_CHANNEL     = 1;
  constexpr unsigned long BEACON_INTERVAL_MS = 80;          // ~12.5 paquetes/s para medir RSSI

  // --- Serial ---
  constexpr unsigned long SERIAL_BAUD = 9600;

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
  constexpr unsigned long TIMER_TICK_MS       = 1000;
  constexpr int32_t       TIMER_START_SECONDS = -5;

  // --- Buzzer ---
  constexpr uint8_t       BUZZER_PIN         = 25;    // ojo: en ESP32-S3 el GPIO 25 no existe
  constexpr unsigned long BUZZER_INTERVAL_MS = 5000;  // cada 5 s
  constexpr unsigned long BUZZER_BEEP_MS     = 150;   // duración del pitido
  constexpr unsigned int  BUZZER_FREQ_HZ     = 2000;
}
