#pragma once

#include <NimBLEDevice.h>

// Beacon BLE no conectable que anuncia el UUID propio para que otro Buscador lo detecte.
class TreasureBeacon {
public:
  explicit TreasureBeacon(const char* beaconUuid);

  void begin();
  void update(unsigned long now);  // no bloqueante

  bool isAdvertising() const;

private:
  bool start();
  NimBLEAdvertisementData buildAdvertisementData() const;

  const char* beaconUuid_;
  NimBLEAdvertising* advertising_ = nullptr;
  unsigned long lastHealthCheckMs_ = 0;
};
