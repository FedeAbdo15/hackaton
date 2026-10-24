#include "TreasureBeacon.h"
#include "config.h"

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <cstring>

namespace {
  constexpr uint8_t BROADCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  TreasureBeacon* g_beaconInstance = nullptr;

#if defined(ESP_ARDUINO_VERSION) && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  void onEspNowRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    (void)info;
    if (g_beaconInstance == nullptr || len < static_cast<int>(sizeof(EspNowPacket))) {
      return;
    }
    EspNowPacket pkt;
    std::memcpy(&pkt, data, sizeof(EspNowPacket));
    if (pkt.magic != Config::PACKET_MAGIC) {
      return;
    }
    g_beaconInstance->onCommandReceived(pkt.type);
  }
#else
  void onEspNowRecv(const uint8_t* mac, const uint8_t* data, int len) {
    (void)mac;
    if (g_beaconInstance == nullptr || len < static_cast<int>(sizeof(EspNowPacket))) {
      return;
    }
    EspNowPacket pkt;
    std::memcpy(&pkt, data, sizeof(EspNowPacket));
    if (pkt.magic != Config::PACKET_MAGIC) {
      return;
    }
    g_beaconInstance->onCommandReceived(pkt.type);
  }
#endif
}

void TreasureBeacon::begin() {
  g_beaconInstance = this;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(Config::ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("[Tesoro] Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onEspNowRecv);

  esp_now_peer_info_t peerInfo{};
  std::memcpy(peerInfo.peer_addr, BROADCAST_MAC, 6);
  peerInfo.channel = Config::ESPNOW_CHANNEL;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("[Tesoro] ESP-NOW listo en canal 1");
}

void TreasureBeacon::start() {
  gameStartReceived_     = false;
  treasureFoundReceived_ = false;
  active_                = true;
  lastBeaconMs_          = millis();
  sendBeacon(0);
  Serial.println("[Tesoro] Transmitiendo beacons ESP-NOW...");
}

void TreasureBeacon::stop() {
  active_ = false;
}

void TreasureBeacon::update(unsigned long now, int32_t currentTimerSeconds) {
  if (!active_) {
    return;
  }
  if (now - lastBeaconMs_ < Config::BEACON_INTERVAL_MS) {
    return;
  }
  lastBeaconMs_ = now;
  sendBeacon(currentTimerSeconds);
}

bool TreasureBeacon::consumeGameStart() {
  return gameStartReceived_.exchange(false);
}

bool TreasureBeacon::consumeTreasureFound() {
  return treasureFoundReceived_.exchange(false);
}

void TreasureBeacon::onCommandReceived(MsgType type) {
  if (type == MsgType::GameStart) {
    gameStartReceived_ = true;
  } else if (type == MsgType::TreasureFound) {
    treasureFoundReceived_ = true;
  }
}

void TreasureBeacon::sendBeacon(int32_t currentTimerSeconds) {
  EspNowPacket pkt{};
  pkt.magic        = Config::PACKET_MAGIC;
  pkt.type         = MsgType::TreasureBeacon;
  pkt.timerSeconds = currentTimerSeconds;
  esp_now_send(BROADCAST_MAC, reinterpret_cast<const uint8_t*>(&pkt), sizeof(pkt));
}
