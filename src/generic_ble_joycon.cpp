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

float GenericBleJoycon::getSlider1() {
   return static_cast<float>(_joy_msg.slider1)*0.01f;
}

float GenericBleJoycon::getSlider2() {
   return static_cast<float>(_joy_msg.slider2)*0.01f;
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

void GenericBleJoycon::onFrameReceived(const Proto::Frame& frame, void* ctx) {
   (void)ctx;
   
   switch (frame.msg_id) {
      case GC_BLE_MSG::JOYCON: {
         std::memcpy(&_joy_msg, frame.payload.data(), sizeof(_joy_msg));
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
            Serial.print("[BLE] Unhandled msg: 0x");
            Serial.println(frame.msg_id, HEX);
         }
      break;
   }
}