#include "TreasureBeacon.h"
#include "config.h"

void TreasureBeacon::begin() {
  NimBLEDevice::init(Config::DEVICE_NAME);
  NimBLEDevice::setPower(Config::TX_POWER_DBM);

  advertising_ = NimBLEDevice::getAdvertising();
  advertising_->setAdvertisementData(buildAdvertisementData());
  advertising_->setConnectableMode(BLE_GAP_CONN_MODE_NON);  // solo anuncia, no acepta conexiones
  advertising_->setMinInterval(Config::ADV_INTERVAL_MIN);
  advertising_->setMaxInterval(Config::ADV_INTERVAL_MAX);
}

bool TreasureBeacon::start() {
  if (advertising_ == nullptr) {
    return false;
  }
  active_ = true;
  lastHealthCheckMs_ = millis();
  const bool ok = advertising_->start();
  if (ok) {
    Serial.print("[Tesoro] Beacon anunciando UUID: ");
    Serial.println(Config::TREASURE_UUID);
  }
  return ok;
}

void TreasureBeacon::stop() {
  active_ = false;
  if (advertising_ != nullptr && advertising_->isAdvertising()) {
    advertising_->stop();
  }
}

void TreasureBeacon::update(unsigned long now) {
  if (!active_) {
    return;
  }
  if (now - lastHealthCheckMs_ < Config::HEALTH_CHECK_INTERVAL_MS) {
    return;
  }
  lastHealthCheckMs_ = now;

  if (isAdvertising()) {
    Serial.println("[Tesoro] Anunciando OK");
    return;
  }

  Serial.println("[Tesoro] Advertising detenido, reiniciando...");
  advertising_->start();
}

bool TreasureBeacon::isAdvertising() const {
  return advertising_ != nullptr && advertising_->isAdvertising();
}

NimBLEAdvertisementData TreasureBeacon::buildAdvertisementData() {
  NimBLEAdvertisementData data;
  data.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
  data.setCompleteServices(NimBLEUUID(Config::TREASURE_UUID));
  data.setName(Config::DEVICE_NAME);
  return data;
}
