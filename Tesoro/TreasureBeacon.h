#pragma once

#include <NimBLEDevice.h>

// Beacon BLE no conectable que anuncia el UUID del tesoro para que el Buscador lo detecte.
//
// Eventos:
//  - begin():  inicializa NimBLE y configura el anuncio (sin transmitir todavía).
//  - start():  comienza a transmitir el anuncio BLE (al presionar BOTÓN N1).
//  - stop():   detiene la transmisión del anuncio BLE.
//  - update(): si está activo, cada HEALTH_CHECK_INTERVAL_MS verifica que siga anunciando.
class TreasureBeacon {
public:
  void begin();
  bool start();
  void stop();
  void update(unsigned long now);  // no bloqueante

  bool isAdvertising() const;
  bool isActive() const { return active_; }

private:
  static NimBLEAdvertisementData buildAdvertisementData();

  NimBLEAdvertising* advertising_ = nullptr;
  unsigned long lastHealthCheckMs_ = 0;
  bool active_ = false;
};
