#include "TreasureBeacon.h"
#include "config.h"

TreasureBeacon::TreasureBeacon(const char* beaconUuid) : beaconUuid_(beaconUuid) {}

void TreasureBeacon::begin() {
  if (!NimBLEDevice::isInitialized()) {
    NimBLEDevice::init(Config::DEVICE_NAME);
  }
  NimBLEDevice::setPower(Config::TX_POWER_DBM);

  advertising_ = NimBLEDevice::getAdvertising();
  advertising_->setAdvertisementData(buildAdvertisementData());
  advertising_->setConnectableMode(BLE_GAP_CONN_MODE_NON);  // solo anuncia, no acepta conexiones
  advertising_->setMinInterval(Config::ADV_INTERVAL_MIN);
  advertising_->setMaxInterval(Config::ADV_INTERVAL_MAX);

  start();

  Serial.print("[Buscador] Beacon anunciando UUID: ");
  Serial.println(beaconUuid_);
}

void TreasureBeacon::update(unsigned long now) {
  if (now - lastHealthCheckMs_ < Config::HEALTH_CHECK_INTERVAL_MS) {
    return;
  }
  lastHealthCheckMs_ = now;

  if (isAdvertising()) {
    Serial.println("[Buscador] Anunciando OK");
    return;
  }

  Serial.println("[Buscador] Advertising detenido, reiniciando...");
  start();
}

bool TreasureBeacon::isAdvertising() const {
  return advertising_ != nullptr && advertising_->isAdvertising();
}

bool TreasureBeacon::start() {
  return advertising_->start();
}

NimBLEAdvertisementData TreasureBeacon::buildAdvertisementData() const {
  NimBLEAdvertisementData data;
  data.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
  data.setCompleteServices(NimBLEUUID(beaconUuid_));
  data.setName(Config::DEVICE_NAME);
  return data;
}
