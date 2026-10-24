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

// Comunicación 100% ESP-NOW del Tesoro:
//  - begin():  inicializa Wi-Fi en modo STA y ESP-NOW (sin transmitir todavía).
//  - start():  comienza a transmitir paquetes broadcast TreasureBeacon (al presionar BOTÓN N1).
//  - stop():   detiene la transmisión de beacons.
//  - update(): envía un paquete cada BEACON_INTERVAL_MS con los segundos actuales del timer.
//  - Recibe instantáneamente los eventos GameStart y TreasureFound desde el Buscador.
class TreasureBeacon {
public:
  void begin();
  void start();
  void stop();
  void update(unsigned long now, int32_t currentTimerSeconds);  // no bloqueante

  bool isActive() const { return active_; }

  bool consumeGameStart();
  bool consumeTreasureFound();

  void onCommandReceived(MsgType type);

private:
  void sendBeacon(int32_t currentTimerSeconds);

  unsigned long     lastBeaconMs_          = 0;
  bool              active_                = false;
  std::atomic<bool> gameStartReceived_{false};
  std::atomic<bool> treasureFoundReceived_{false};
};
