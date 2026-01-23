
#include <Arduino.h>
#include "ble_server_controller.hpp"
#include "ble_msg_ids.hpp"

#include "joy_con_msg.hpp"
#include "establish_msg.hpp"

BLEServerController ble;


static inline float unscaleStick(int8_t v) {
   // Example: map [-100..100] to [-1.0..1.0]
   return static_cast<float>(v) / 100.0f;
}

rx_joy_con_msg_t joy_msg{};

// Plain C callback invoked for every valid Proto frame
void onFrameReceived(const Proto::Frame& frame, void* ctx) {
   (void)ctx;
   
   switch (frame.msg_id) {
      case GC_BLE_MSG::JOYCON: {
         std::memcpy(&joy_msg, frame.payload.data(), sizeof(joy_msg));
         
         float lx = joy_msg.left_x*0.01f;
         float ly = joy_msg.left_y*0.01f;
         
         float rx = joy_msg.right_x*0.01f;
         float ry = joy_msg.right_y*0.01f;
         
         Serial.print("[JOY] L("); Serial.print(lx, 2); Serial.print(", "); Serial.print(ly, 2);
         Serial.print(") R("); Serial.print(rx, 2); Serial.print(", "); Serial.print(ry, 2);
         Serial.print(") btns=0x"); 
         uint8_t btns_raw =
            (joy_msg.btns.btn_a ? 0x80 : 0) |
            (joy_msg.btns.btn_b ? 0x40 : 0) |
            (joy_msg.btns.btn_x ? 0x20 : 0) |
            (joy_msg.btns.btn_y ? 0x10 : 0) |
            (joy_msg.btns.dpad_l ? 0x08 : 0) |
            (joy_msg.btns.dpad_u ? 0x04 : 0) |
            (joy_msg.btns.dpad_r ? 0x02 : 0) |
            (joy_msg.btns.dpad_d ? 0x01 : 0);
         Serial.print(btns_raw, HEX);
         
         Serial.print(" max="); Serial.print(joy_msg.max_throttle);
         Serial.print(" trim="); Serial.print(joy_msg.trim);
         Serial.print(" flags=0x"); 
         uint8_t flags_raw =
            (joy_msg.flags.is_single_stick ? 0x80 : 0) |
            (joy_msg.flags.is_left_single_stick ? 0x40 : 0);
         Serial.print(flags_raw, HEX);
         
         Serial.print(" cnt="); Serial.println(joy_msg.msg_idx);
      }
      break;
      
      case GC_BLE_MSG::ESTAB_CON: {
         rx_establish_t rx_msg{};
         std::memcpy(&rx_msg, frame.payload.data(), sizeof(rx_msg));
         
         rx_establish_t tx_msg{};
         ble.sendMessage(
            static_cast<uint8_t>(GC_BLE_MSG::ESTAB_CON), 
            reinterpret_cast<const uint8_t*>(&tx_msg), 
            sizeof(tx_msg)
         );
      }
      break;
      
      default:
         Serial.print(" Unregistered msg: ");
         Serial.println(frame.msg_id);
      break;
   }
}


void setup() {
   Serial.begin(115200);
   while(!Serial);
   
   Serial.println("\n[BOOT] BLE Protocol Example (no lambdas)");
   
   ble.begin();
   ble.setFrameHandler(&onFrameReceived, /*ctx*/ nullptr);
}

// uint32_t lastPing = 0;

void loop() {
   // if (ble.isConnected()) {
   //    uint32_t now = millis();
   //    if (now - lastPing > 5000) {
   //       const char* txt = "ping";
   //       ble.sendMessage(GC_BLE_MSG::PING, reinterpret_cast<const uint8_t*>(txt), strlen(txt));
   //       lastPing = now;
   //       Serial.println("[TX] Sent PING");
   //    }
   // }
   delay(10);
}
