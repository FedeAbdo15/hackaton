#pragma once

#include <NimBLEDevice.h>
#include <atomic>

struct RssiReading {
  bool everSeen;            // se vio al tesoro al menos una vez
  bool fresh;               // hay una lectura nueva desde el último read()
  int rssi;                 // último RSSI crudo (dBm)
  unsigned long lastSeenMs; // millis() de la última lectura
};

// Escanea BLE de forma continua y guarda el RSSI solo del dispositivo con el UUID objetivo.
// Los callbacks corren en la tarea de NimBLE, por eso el estado compartido es atómico.
class BleScanner : public NimBLEScanCallbacks {
public:
  explicit BleScanner(const char* targetUuid);

  void begin();
  RssiReading read();

private:
  void onResult(const NimBLEAdvertisedDevice* device) override;
  void onScanEnd(const NimBLEScanResults& results, int reason) override;

  static void startScan();

  const NimBLEUUID targetUuid_;
  std::atomic<int> lastRssi_{0};
  std::atomic<unsigned long> lastSeenMs_{0};
  std::atomic<bool> fresh_{false};
  std::atomic<bool> everSeen_{false};
};
