
#pragma once

#include <NimBLEDevice.h>
#include <vector>

#include "ble_protocol.hpp"

#define BLE_DEVICE_NAME "GC_ESP32-Proto"

#define BLE_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"

#define BLE_RX_CHAR_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define BLE_TX_CHAR_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class BLEServerController {
    public:
        // Simple C-style callback: frame + user-provided context
        
        
        BLEServerController();
        void begin();
        
        // Register a handler without lambdas/std::function
        void setFrameHandler(Proto::FrameCallback cb, void* obj_ptr = nullptr);
        
        bool isConnected() const;
        bool sendMessage(const uint8_t& msg_id, const std::vector<uint8_t>& payload);
        bool sendMessage(const uint8_t& msg_id, const uint8_t* payload, const size_t& len);
        
    private:
        class RxCallbacks : public NimBLECharacteristicCallbacks {
            public:
                explicit RxCallbacks(BLEServerController& parent) : _parent(parent) {}
                // NimBLE-Arduino v2.x signature
                void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override;
            private:
                BLEServerController& _parent;
        };
        
        class ServerCallbacks : public NimBLEServerCallbacks {
            public:
                explicit ServerCallbacks(BLEServerController& parent) : _parent(parent) {}
                void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
                void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
            private:
                BLEServerController& _parent;
        };
        
        void handleIncomingRaw(const std::vector<uint8_t>& buf);
        
        NimBLEServer* _server{nullptr};
        NimBLEService* _service{nullptr};
        NimBLECharacteristic* _rxChar{nullptr};
        NimBLECharacteristic* _txChar{nullptr};
        volatile bool _connected{false};
        
        // Stored callback + opaque context pointer
        Proto::FrameCallback _cb{nullptr};
        void* _cb_obj_ptr{nullptr};
};
