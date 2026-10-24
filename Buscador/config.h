#pragma once

#include <Arduino.h>

namespace Config {
  // --- Protocolo ESP-NOW (tiene que coincidir con Tesoro/config.h) ---
  constexpr uint32_t PACKET_MAGIC      = 0x5445534F;  // "TESO": filtra paquetes ajenos en el canal
  constexpr uint8_t  ESPNOW_CHANNEL    = 1;
  constexpr uint8_t  CMD_BURST_COUNT   = 3;           // envíos por comando para asegurar entrega

  // --- Serial ---
  constexpr unsigned long SERIAL_BAUD           = 9600;
  constexpr unsigned long DEBUG_LOG_INTERVAL_MS = 500;

  // --- Señal ---
  constexpr float         EMA_ALPHA         = 0.25f;  // más bajo = más suave pero más lento
  constexpr unsigned long SIGNAL_TIMEOUT_MS = 3000;   // sin lecturas por este tiempo = SIN SEÑAL

  // --- Umbrales de proximidad (dBm, calibrar en el lugar) ---
  constexpr float THRESHOLD_TIBIO      = -55.0f;
  constexpr float THRESHOLD_CALIENTE   = -45.0f;
  constexpr float THRESHOLD_ENCONTRADO = -10.0f;
  constexpr float HYSTERESIS_DB        = 3.0f;

  // --- Juego y Tiempos ---
  constexpr unsigned long START_COUNTDOWN_MS = 5000;  // 5 s mostrando "Juego comenzado"
  constexpr unsigned long NOT_READY_MSG_MS   = 2500;  // duración de "Tesoro no preparado"
  constexpr uint32_t      FAST_WIN_SECONDS   = 30;
  constexpr uint32_t      MEDIUM_WIN_SECONDS = 60;

  // --- Botón N2 (entre el pin y GND, con pull-up interno) ---
  constexpr uint8_t       BUTTON_N2_PIN      = 19;
  constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;

  // --- OLED SSD1306 0,96" por SPI ---
  constexpr int8_t        OLED_MOSI          = 2;
  constexpr int8_t        OLED_CLK           = 1;
  constexpr int8_t        OLED_CS            = 3;
  constexpr int8_t        OLED_DC            = 6;
  constexpr int8_t        OLED_RST           = 7;
  constexpr uint8_t       OLED_WIDTH         = 128;
  constexpr uint8_t       OLED_HEIGHT        = 64;
  constexpr uint8_t       OLED_ROTATION      = 2;     // 0 = normal, 2 = 180°
  constexpr unsigned long OLED_REFRESH_MS    = 150;   // máx. ~6 refrescos/s
}
