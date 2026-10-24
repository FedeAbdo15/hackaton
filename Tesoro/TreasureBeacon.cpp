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

  start();

  Serial.print("[Tesoro] Beacon anunciando UUID: ");
  Serial.println(Config::TREASURE_UUID);
}

void TreasureBeacon::update(unsigned long now) {
  if (now - lastHealthCheckMs_ < Config::HEALTH_CHECK_INTERVAL_MS) {
    return;
  }
  lastHealthCheckMs_ = now;

  if (isAdvertising()) {
    Serial.println("[Tesoro] Anunciando OK");
    return;
  }

  Serial.println("[Tesoro] Advertising detenido, reiniciando...");
  start();
}

bool TreasureBeacon::isAdvertising() const {
  return advertising_ != nullptr && advertising_->isAdvertising();
}

bool TreasureBeacon::start() {
  return advertising_->start();
}

NimBLEAdvertisementData TreasureBeacon::buildAdvertisementData() {
  NimBLEAdvertisementData data;
  data.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
  data.setCompleteServices(NimBLEUUID(Config::TREASURE_UUID));
  data.setName(Config::DEVICE_NAME);
  return data;
}
