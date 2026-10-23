/*
 * Tesoro — Búsqueda del tesoro BLE
 * Requiere: esp32 by Espressif Systems + NimBLE-Arduino (h2zero) v2.x
 */

#include <NimBLEDevice.h>

namespace Config {
  constexpr char TREASURE_UUID[] = "7e2a1c4b-3f9d-4e8a-9b21-5c6d0f3a8e17";
  constexpr char DEVICE_NAME[]   = "Tesoro";

  constexpr int8_t TX_POWER_DBM = 9;

  // Unidades de 0.625 ms (80 = 50 ms, 160 = 100 ms)
  constexpr uint16_t ADV_INTERVAL_MIN = 80;
  constexpr uint16_t ADV_INTERVAL_MAX = 160;

  constexpr unsigned long SERIAL_BAUD              = 9600;
  constexpr unsigned long HEALTH_CHECK_INTERVAL_MS = 5000;
}

class TreasureBeacon {
public:
  void begin() {
    NimBLEDevice::init(Config::DEVICE_NAME);
    NimBLEDevice::setPower(Config::TX_POWER_DBM);

    advertising_ = NimBLEDevice::getAdvertising();
    advertising_->setAdvertisementData(buildAdvertisementData());
    advertising_->setConnectableMode(BLE_GAP_CONN_MODE_NON);
    advertising_->setMinInterval(Config::ADV_INTERVAL_MIN);
    advertising_->setMaxInterval(Config::ADV_INTERVAL_MAX);

    start();
  }

  bool isAdvertising() const {
    return advertising_ != nullptr && advertising_->isAdvertising();
  }

  bool start() {
    return advertising_->start();
  }

private:
  NimBLEAdvertising* advertising_ = nullptr;

  static NimBLEAdvertisementData buildAdvertisementData() {
    NimBLEAdvertisementData data;
    data.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
    data.setCompleteServices(NimBLEUUID(Config::TREASURE_UUID));
    data.setName(Config::DEVICE_NAME);
    return data;
  }
};

TreasureBeacon beacon;
unsigned long lastHealthCheckMs = 0;

void logStatus(bool advertising) {
  Serial.print("[Tesoro] ");
  Serial.println(advertising ? "Anunciando OK" : "Advertising detenido, reiniciando...");
}

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  Serial.println("[Tesoro] Iniciando...");

  beacon.begin();

  Serial.print("[Tesoro] UUID: ");
  Serial.println(Config::TREASURE_UUID);
}

void loop() {
  const unsigned long now = millis();
  if (now - lastHealthCheckMs < Config::HEALTH_CHECK_INTERVAL_MS) {
    return;
  }
  lastHealthCheckMs = now;

  const bool advertising = beacon.isAdvertising();
  logStatus(advertising);
  if (!advertising) {
    beacon.start();
  }
}