#include "BleScanner.h"
#include "config.h"

namespace {
  constexpr uint32_t SCAN_FOREVER = 0;
}

BleScanner::BleScanner(const char* targetUuid) : targetUuid_(targetUuid) {}

void BleScanner::begin() {
  if (!NimBLEDevice::isInitialized()) {
    NimBLEDevice::init(Config::DEVICE_NAME);
  }

  NimBLEScan* scan = NimBLEDevice::getScan();
  scan->setScanCallbacks(this, true);  // true = recibir duplicados (una lectura por anuncio)
  scan->setActiveScan(false);          // pasivo: solo escuchar, no pedir scan response
  scan->setInterval(Config::SCAN_INTERVAL_MS);
  scan->setWindow(Config::SCAN_WINDOW_MS);
  scan->setDuplicateFilter(false);
  scan->setMaxResults(0);              // no acumular resultados en memoria

  startScan();
}

RssiReading BleScanner::read() {
  RssiReading reading;
  reading.fresh      = fresh_.exchange(false);
  reading.rssi       = lastRssi_.load();
  reading.lastSeenMs = lastSeenMs_.load();
  reading.everSeen   = everSeen_.load();
  return reading;
}

void BleScanner::onResult(const NimBLEAdvertisedDevice* device) {
  if (!device->isAdvertisingService(targetUuid_)) {
    return;
  }

  lastRssi_   = device->getRSSI();
  lastSeenMs_ = millis();
  everSeen_   = true;
  fresh_      = true;
}

void BleScanner::onScanEnd(const NimBLEScanResults& results, int reason) {
  startScan();  // si por algo se corta, vuelve a escanear
}

void BleScanner::startScan() {
  NimBLEDevice::getScan()->start(SCAN_FOREVER, false, true);
}
