#include "BleScanner.h"
#include "config.h"

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <cstring>

namespace {
  constexpr uint8_t BROADCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  BleScanner* g_scannerInstance = nullptr;

#if defined(ESP_ARDUINO_VERSION) && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  void onEspNowRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (g_scannerInstance == nullptr || info == nullptr || info->rx_ctrl == nullptr) {
      return;
    }
    if (len < static_cast<int>(sizeof(EspNowPacket))) {
      return;
    }
    EspNowPacket pkt;
    std::memcpy(&pkt, data, sizeof(EspNowPacket));
    if (pkt.magic != Config::PACKET_MAGIC || pkt.type != MsgType::TreasureBeacon) {
      return;
    }
    g_scannerInstance->onBeaconReceived(info->rx_ctrl->rssi, pkt.timerSeconds);
  }
#else
  std::atomic<int> g_lastPromiscuousRssi{-70};

  void onPromiscuousRx(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) {
      return;
    }
    const auto* pkt = static_cast<const wifi_promiscuous_pkt_t*>(buf);
    g_lastPromiscuousRssi = pkt->rx_ctrl.rssi;
  }

  void onEspNowRecv(const uint8_t* mac, const uint8_t* data, int len) {
    if (g_scannerInstance == nullptr || len < static_cast<int>(sizeof(EspNowPacket))) {
      return;
    }
    EspNowPacket pkt;
    std::memcpy(&pkt, data, sizeof(EspNowPacket));
    if (pkt.magic != Config::PACKET_MAGIC || pkt.type != MsgType::TreasureBeacon) {
      return;
    }
    g_scannerInstance->onBeaconReceived(g_lastPromiscuousRssi.load(), pkt.timerSeconds);
  }
#endif
}

void BleScanner::begin() {
  g_scannerInstance = this;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(Config::ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
#if !(defined(ESP_ARDUINO_VERSION) && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
  esp_wifi_set_promiscuous_rx_cb(&onPromiscuousRx);
#else
  esp_wifi_set_promiscuous(false);
#endif

  if (esp_now_init() != ESP_OK) {
    Serial.println("[Buscador] Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onEspNowRecv);

  esp_now_peer_info_t peerInfo{};
  std::memcpy(peerInfo.peer_addr, BROADCAST_MAC, 6);
  peerInfo.channel = Config::ESPNOW_CHANNEL;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("[Buscador] ESP-NOW listo en canal 1");
}

RssiReading BleScanner::read() {
  RssiReading reading;
  reading.fresh           = fresh_.exchange(false);
  reading.rssi            = lastRssi_.load();
  reading.lastSeenMs      = lastSeenMs_.load();
  reading.everSeen        = everSeen_.load();
  reading.treasureSeconds = lastTreasureSeconds_.load();
  return reading;
}

void BleScanner::onBeaconReceived(int rssi, int32_t timerSeconds) {
  lastRssi_            = rssi;
  lastTreasureSeconds_ = timerSeconds;
  lastSeenMs_          = millis();
  everSeen_            = true;
  fresh_               = true;
}

void BleScanner::sendGameStart() {
  sendCommand(MsgType::GameStart);
  Serial.println("[Buscador] ESP-NOW -> GameStart enviado");
}

void BleScanner::sendTreasureFound() {
  sendCommand(MsgType::TreasureFound);
  Serial.println("[Buscador] ESP-NOW -> TreasureFound enviado");
}

void BleScanner::sendCommand(MsgType type) {
  EspNowPacket pkt{};
  pkt.magic        = Config::PACKET_MAGIC;
  pkt.type         = type;
  pkt.timerSeconds = 0;

  for (uint8_t i = 0; i < Config::CMD_BURST_COUNT; ++i) {
    esp_now_send(BROADCAST_MAC, reinterpret_cast<const uint8_t*>(&pkt), sizeof(pkt));
  }
}
