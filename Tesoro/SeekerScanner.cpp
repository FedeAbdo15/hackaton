#include "SeekerScanner.h"
#include "config.h"

namespace {
  constexpr uint32_t SCAN_FOREVER = 0;
}

SeekerScanner::SeekerScanner(const char* seekerUuid) : targetUuid_(seekerUuid) {}

void SeekerScanner::begin() {
  NimBLEScan* scan = NimBLEDevice::getScan();
  scan->setScanCallbacks(this, true);  // true = recibir duplicados (una lectura por anuncio)
  scan->setActiveScan(false);          // pasivo: solo escuchar anuncios
  scan->setInterval(Config::SCAN_INTERVAL_MS);
  scan->setWindow(Config::SCAN_WINDOW_MS);
  scan->setDuplicateFilter(false);
  scan->setMaxResults(0);              // no acumular resultados en memoria
}

void SeekerScanner::start() {
  everSeen_   = false;
  lastSeenMs_ = 0;
  active_     = true;
  startScan();
}

void SeekerScanner::stop() {
  active_ = false;
  NimBLEScan* scan = NimBLEDevice::getScan();
  if (scan != nullptr && scan->isScanning()) {
    scan->stop();
  }
}

bool SeekerScanner::hasStarted() const {
  return everSeen_.load();
}

bool SeekerScanner::hasStopped(unsigned long now) const {
  if (!everSeen_.load()) {
    return false;
  }
  const unsigned long lastSeen = lastSeenMs_.load();
  // lastSeenMs_ puede ser apenas mayor que now (se escribe desde la tarea de NimBLE)
  if (lastSeen >= now) {
    return false;
  }
  return (now - lastSeen) > Config::SEEKER_TIMEOUT_MS;
}

void SeekerScanner::onResult(const NimBLEAdvertisedDevice* device) {
  if (!active_.load()) {
    return;
  }
  if (!device->isAdvertisingService(targetUuid_)) {
    return;
  }

  lastSeenMs_ = millis();
  everSeen_   = true;
}

void SeekerScanner::onScanEnd(const NimBLEScanResults& results, int reason) {
  if (active_.load()) {
    startScan();
  }
}

void SeekerScanner::startScan() {
  NimBLEDevice::getScan()->start(SCAN_FOREVER, false, true);
}
