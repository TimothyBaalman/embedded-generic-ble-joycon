#include <Arduino.h>
#include "ble_server_controller.hpp"
#include "ble_msg_ids.hpp"

#include "joy_con_msg.hpp"
#include "establish_msg.hpp"

struct stick_t {
   float x = 0.0f;
   float y = 0.0f;
};

class GenericBleJoycon : public BLEServerController{
   public:
      GenericBleJoycon() = default;
      
      void setFrameExtra(Proto::FrameCallback cb);
      
      void setup();
      void start();
      
      stick_t getLeftStick();
      stick_t getRightStick();
      float getSlider1();
      float getSlider2();
      
      bool isBtnADown();
      bool isBtnBDown();
      bool isBtnXDown();
      bool isBtnYDown();
      
      bool isDpadLeftPressed();
      bool isDpadUpPressed();
      bool isDpadRightPressed();
      bool isDpadDownPressed();

      bool getFlag0() {
         return _joy_msg.flags.flag_0;
      }
      bool getFlag1() {
         return _joy_msg.flags.flag_1;
      }
      bool getFlag2() {
         return _joy_msg.flags.flag_2;
      }
      bool getFlag3() {
         return _joy_msg.flags.flag_3;
      }
      bool getFlag4() {
         return _joy_msg.flags.flag_4;
      }
      bool getFlag5() {
         return _joy_msg.flags.flag_5;
      }
      bool getFlag6() {
         return _joy_msg.flags.flag_6;
      }
      bool getFlag7() {
         return _joy_msg.flags.flag_7;
      }
   
   protected:
      
   private:
      void onFrameReceived(const Proto::Frame& frame, void* ctx);
      
      // Static trampoline to adapt to C callback signature
      static void FrameThunk(const Proto::Frame& frame, void* ctx) {
         auto* self = static_cast<GenericBleJoycon*>(ctx);
         self->onFrameReceived(frame, ctx);
      }
      
      Proto::FrameCallback _extras_cb{nullptr};
      
      rx_joy_con_msg_t _joy_msg{};
};