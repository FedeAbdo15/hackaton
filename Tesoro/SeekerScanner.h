#pragma once

#include <NimBLEDevice.h>
#include <atomic>

// Escanea BLE para detectar cuándo el Buscador inicia su transmisión (al presionar BOTÓN N2)
// y cuándo deja de transmitir (al encontrar el Tesoro).
class SeekerScanner : public NimBLEScanCallbacks {
public:
  explicit SeekerScanner(const char* seekerUuid);

  void begin();
  void start();
  void stop();

  bool hasStarted() const;
  bool hasStopped(unsigned long now) const;

private:
  void onResult(const NimBLEAdvertisedDevice* device) override;
  void onScanEnd(const NimBLEScanResults& results, int reason) override;

  static void startScan();

  const NimBLEUUID targetUuid_;
  std::atomic<unsigned long> lastSeenMs_{0};
  std::atomic<bool> everSeen_{false};
  std::atomic<bool> active_{false};
};
