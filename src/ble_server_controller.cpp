
#include "ble_server_controller.hpp"
#include "ble_msg_ids.hpp"
#include <Arduino.h>


BLEServerController::BLEServerController() {}

void BLEServerController::begin() {
   NimBLEDevice::init(BLE_DEVICE_NAME);
   
   // NimBLE-Arduino v2.x: set power in dBm, not with ESP_PWR_LVL_*
   NimBLEDevice::setPower(7);
   NimBLEDevice::setMTU(247);
   
   _server = NimBLEDevice::createServer();
   _server->setCallbacks(new ServerCallbacks(*this));
   
   _service = _server->createService(BLE_SERVICE_UUID);
   
   _txChar = _service->createCharacteristic(
      BLE_TX_CHAR_UUID,
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
   );
   
   _rxChar = _service->createCharacteristic(
      BLE_RX_CHAR_UUID,
      NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
   );
   _rxChar->setCallbacks(new RxCallbacks(*this));
   
   _service->start();
   
   // --- Advertising Setup (robust, explicit) ---
   NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
   
   // Use public address to avoid privacy-related discovery issues on some scanners
   NimBLEDevice::setOwnAddrType(BLE_OWN_ADDR_PUBLIC);
   
   // Build advertisement data: put NAME and SERVICE UUID in the ADV (not just scan response)
   NimBLEAdvertisementData advData;
   advData.setName(BLE_DEVICE_NAME);                 // Name in primary ADV
   advData.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
   advData.addServiceUUID(BLE_SERVICE_UUID);         // Your 128-bit service UUID
   
   // (Optional) separate scan response data if you want to add more later
   NimBLEAdvertisementData scanData;
   // e.g., scanData.addTxPower(); or manufacturer data…
   scanData.setName(BLE_DEVICE_NAME);
   
   adv->setAdvertisementData(advData);
   adv->setScanResponseData(scanData);
   
   // Be connectable + general discoverable
   adv->setConnectableMode(BLE_GAP_CONN_MODE_UND);
   adv->setDiscoverableMode(BLE_GAP_DISC_MODE_GEN);
   
   // Do NOT require whitelist filters
   adv->setScanFilter(false, false);
   
   // Explicitly enable scan response (in case you later add data there)
   adv->enableScanResponse(true);
   
   // Start indefinite advertising (argument 0 = no timeout in v2.x)
   adv->start(0);
   
   Serial.printf("[BLE] Addr: %s\n", NimBLEDevice::getAddress().toString().c_str());
   Serial.printf("[BLE] Advertising? %s\n", adv->isAdvertising() ? "yes" : "no");
}

void BLEServerController::setFrameHandler(Proto::FrameCallback cb, void* obj_ptr) {
   _cb = cb;
   _cb_obj_ptr = obj_ptr;
}

bool BLEServerController::isConnected() const {
   return _connected;
}

bool BLEServerController::sendMessage(const uint8_t& msg_id, const std::vector<uint8_t>& payload) {
   return sendMessage(msg_id, payload.data(), payload.size());
}

bool BLEServerController::sendMessage(const uint8_t& msg_id, const uint8_t* payload, const size_t& len) {
   if (!_txChar || !_connected) return false;
   auto packet = Proto::encode(msg_id, payload, len);
   _txChar->setValue(packet.data(), packet.size());
   _txChar->notify();
   return true;
}

void BLEServerController::RxCallbacks::onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
   (void)connInfo;
   std::string v = pCharacteristic->getValue();
   if (v.empty()) return;
   
   std::vector<uint8_t> buf(v.begin(), v.end());
   _parent.handleIncomingRaw(buf);
}

void BLEServerController::handleIncomingRaw(const std::vector<uint8_t>& buf) {
   Proto::Frame frame;
   if (!Proto::decode(buf, frame)) {
      Serial.println("[BLE] Invalid frame: bad headers or too short");
      return;
   }
   
   // Call user callback if provided
   if(_cb) {
      _cb(frame, _cb_obj_ptr);
      return;
   }
   
   // Default behavior
   switch (frame.msg_id) {
      case GC_BLE_MSG::PING: {
         const char* pong = "pong";
         sendMessage(GC_BLE_MSG::PONG, reinterpret_cast<const uint8_t*>(pong), strlen(pong));
         break;
      }
      case GC_BLE_MSG::ECHO: {
         sendMessage(GC_BLE_MSG::ECHO, frame.payload);
         break;
      }
      default:
         break;
   }
}

void BLEServerController::ServerCallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
   (void)pServer; (void)connInfo;
   _parent._connected = true;
   Serial.println("[BLE] Central connected");
}

void BLEServerController::ServerCallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
   (void)pServer; (void)connInfo; (void)reason;
   _parent._connected = false;
   Serial.println("[BLE] Central disconnected, restarting advertising");
   NimBLEDevice::startAdvertising(0);
}
