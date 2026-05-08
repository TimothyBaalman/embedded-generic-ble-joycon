#include "generic_ble_joycon.hpp"

void GenericBleJoycon::setFrameExtra(Proto::FrameCallback cb) {
   _extras_cb = cb;
}

void GenericBleJoycon::setup() {
   // Register the trampoline and pass 'this' as context
   setFrameHandler(&GenericBleJoycon::FrameThunk, this);
}

void GenericBleJoycon::start() {
   // Initialize BLE (advertising etc.)
   begin();
}

stick_t GenericBleJoycon::getLeftStick() {
   stick_t stick; 
   stick.x = static_cast<float>(_joy_msg.left_x)*0.01f;
   stick.y = static_cast<float>(_joy_msg.left_y)*0.01f;
   return stick;
}

stick_t GenericBleJoycon::getRightStick() {
   stick_t stick; 
   stick.x = static_cast<float>(_joy_msg.right_x)*0.01f;
   stick.y = static_cast<float>(_joy_msg.right_y)*0.01f;
   return stick;
}

float GenericBleJoycon::getMaxThrottle() {
   return static_cast<float>(_joy_msg.max_throttle)*0.01f;
}

float GenericBleJoycon::getTrim() {
   return static_cast<float>(_joy_msg.trim)*0.01f;
}

bool GenericBleJoycon::isBtnADown() {
   return _joy_msg.btns.btn_a;
}

bool GenericBleJoycon::isBtnBDown() {
   return _joy_msg.btns.btn_b;
}

bool GenericBleJoycon::isBtnXDown() {
   return _joy_msg.btns.btn_x;
}

bool GenericBleJoycon::isBtnYDown() {
   return _joy_msg.btns.btn_y;
}

bool GenericBleJoycon::isDpadLeftPressed() {
   return _joy_msg.btns.dpad_l;
}

bool GenericBleJoycon::isDpadUpPressed() {
   return _joy_msg.btns.dpad_u;
}

bool GenericBleJoycon::isDpadRightPressed() {
   return _joy_msg.btns.dpad_r;
}

bool GenericBleJoycon::isDpadDownPressed() {
   return _joy_msg.btns.dpad_d;
}

bool GenericBleJoycon::isSingleStick() {
   return _joy_msg.flags.is_single_stick;
}

bool GenericBleJoycon::isLeftSingleStick() {
   return _joy_msg.flags.is_left_single_stick;
}

void GenericBleJoycon::onFrameReceived(const Proto::Frame& frame, void* ctx) {
   (void)ctx;
   
   switch (frame.msg_id) {
      case GC_BLE_MSG::JOYCON: {
         std::memcpy(&_joy_msg, frame.payload.data(), sizeof(_joy_msg));
         
         // float left_x = _joy_msg.left_x*0.01f;
         // float left_y = _joy_msg.left_y*0.01f;
         //
         // float right_x = _joy_msg.right_x*0.01f;
         // float right_y = _joy_msg.right_y*0.01f;
         //
         // float max_throttle = _joy_msg.max_throttle*0.01f;
         // float trim = _joy_msg.trim*0.01f;
         //
         // Serial.print("[JOY] L("); Serial.print(left_x, 2); Serial.print(", "); Serial.print(left_y, 2);
         // Serial.print(") R("); Serial.print(right_x, 2); Serial.print(", "); Serial.print(right_y, 2);
         // Serial.print(") btns=0x"); 
         // uint8_t btns_raw =
         //    (_joy_msg.btns.btn_a ? 0x80 : 0) |
         //    (_joy_msg.btns.btn_b ? 0x40 : 0) |
         //    (_joy_msg.btns.btn_x ? 0x20 : 0) |
         //    (_joy_msg.btns.btn_y ? 0x10 : 0) |
         //    (_joy_msg.btns.dpad_l ? 0x08 : 0) |
         //    (_joy_msg.btns.dpad_u ? 0x04 : 0) |
         //    (_joy_msg.btns.dpad_r ? 0x02 : 0) |
         //    (_joy_msg.btns.dpad_d ? 0x01 : 0);
         // Serial.print(btns_raw, HEX);
         //
         // Serial.print(" max="); Serial.print(max_throttle);
         // Serial.print(" trim="); Serial.print(trim);
         // Serial.print(" flags=0x"); 
         // uint8_t flags_raw =
         //    (_joy_msg.flags.is_single_stick ? 0x80 : 0) |
         //    (_joy_msg.flags.is_left_single_stick ? 0x40 : 0);
         // Serial.print(flags_raw, HEX);
         //
         // Serial.print(" cnt="); Serial.println(_joy_msg.msg_idx);
      }
      break;
      
      case GC_BLE_MSG::ESTAB_CON: {
         rx_establish_t rx_msg{};
         std::memcpy(&rx_msg, frame.payload.data(), sizeof(rx_msg));
         
         rx_establish_t tx_msg{};
         sendMessage(
            static_cast<uint8_t>(GC_BLE_MSG::ESTAB_CON), 
            reinterpret_cast<const uint8_t*>(&tx_msg), 
            sizeof(tx_msg)
         );
      }
      break;
      
      default:
         if (_extras_cb) {
            // Let user-provided extra handler see unhandled frames.
            _extras_cb(frame, this);
         } 
         else {
            #ifdef DEBUG
               Serial.print("[BLE] Unhandled msg: 0x");
               Serial.println(frame.msg_id, HEX);
            #endif
         }
      break;
   }
}