#pragma once

#include <Arduino.h>
#include <atomic>

enum class MsgType : uint8_t {
  TreasureBeacon = 1,  // Emitido por el Tesoro periódicamente tras BOTÓN N1
  GameStart      = 2,  // Emitido por el Buscador al presionar BOTÓN N2
  TreasureFound  = 3   // Emitido por el Buscador al llegar a distancia 0
};

struct __attribute__((packed)) EspNowPacket {
  uint32_t magic;
  MsgType  type;
  int32_t  timerSeconds;
};

struct RssiReading {
  bool          everSeen;        // se vio al tesoro al menos una vez
  bool          fresh;           // hay una lectura nueva desde el último read()
  int           rssi;            // último RSSI crudo (dBm)
  unsigned long lastSeenMs;      // millis() de la última lectura
  int32_t       treasureSeconds; // último tiempo reportado por el Tesoro (s)
};

// Comunicación 100% ESP-NOW del Buscador:
//  - Recibe paquetes broadcast TreasureBeacon y extrae el RSSI de la cabecera de radio.
//  - Envía comandos broadcast GameStart (al presionar BOTÓN N2) y TreasureFound (al encontrarlo).
class EspNowScanner {
public:
  EspNowScanner() = default;

  void begin();
  RssiReading read();

  void sendGameStart();
  void sendTreasureFound();

  void onBeaconReceived(int rssi, int32_t timerSeconds);

private:
  void sendCommand(MsgType type);

  std::atomic<int>           lastRssi_{0};
  std::atomic<unsigned long> lastSeenMs_{0};
  std::atomic<int32_t>       lastTreasureSeconds_{0};
  std::atomic<bool>          fresh_{false};
  std::atomic<bool>          everSeen_{false};
};
