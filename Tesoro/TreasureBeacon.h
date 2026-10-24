#pragma once

#include <NimBLEDevice.h>

// Beacon BLE no conectable que anuncia el UUID del tesoro para que el Buscador lo detecte.
//
// Eventos:
//  - begin():  inicializa NimBLE, configura el anuncio y empieza a transmitir.
//  - update(): cada HEALTH_CHECK_INTERVAL_MS verifica que siga anunciando.
//              Loguea "Anunciando OK" o, si se cortó, "Advertising detenido"
//              y lo vuelve a arrancar.
class TreasureBeacon {
public:
  void begin();
  void update(unsigned long now);  // no bloqueante

  bool isAdvertising() const;

private:
  bool start();
  static NimBLEAdvertisementData buildAdvertisementData();

  NimBLEAdvertising* advertising_ = nullptr;
  unsigned long lastHealthCheckMs_ = 0;
};
