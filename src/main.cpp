#include <ESP32Servo.h>
#include "generic_ble_joycon.hpp"
#include <functional>

GenericBleJoycon ble_joycon;

// Plain C callback invoked for every valid Proto frame
void extra_frame_processing(const Proto::Frame& frame, void* ctx) {
   (void)ctx;
   
   switch (frame.msg_id) {
      default:
         #ifdef DEBUG
            Serial.print(" Extra msg to parse: ");
            Serial.println(frame.msg_id);
         #endif
      break;
   }
}

#define L_IN1  12
#define L_IN2  14
#define R_IN1  15
#define R_IN2  13

#define LI_IN1 17
#define LI_IN2 16

#define motorEnable 26

Servo bucket;
int lifter_pos;
int bucket_pivot;
int lifter_speed = 3;

void applyDeadzone(stick_t& stick, const float& deadzone) {
   if(fabs(stick.x) < deadzone) stick.x = 0.0f;
   if(fabs(stick.y) < deadzone) stick.y = 0.0f;
}
// DRV8833 motor driver function (PWM–PWM mode)
void driveHBridge(uint8_t in1, uint8_t in2, int value) {
   // forward
   if(value > 0) {
      analogWrite(in1, value);
      analogWrite(in2, 0);
   }
   // reverse
   else if(value < 0) {
      analogWrite(in1, 0);
      analogWrite(in2, -value);
   }
   // stop / coast
   else{
      analogWrite(in1, 0);
      analogWrite(in2, 0);
   }
}
void singleJoystickControl(
   const float& x, const float& y,
   float& left_spd, float& right_spd
){
   // FWD
   if(y > 0.0f) {
      // FWD with Right Turn
      if(x > 0.0f) {
         float offset_pwr = fabs(y - x);
         // Is our turning force larger than fwd force
         if(offset_pwr > y) {
            left_spd = offset_pwr;
            right_spd = y;
         }
         else {
            left_spd = y;
            right_spd = offset_pwr;
         }
      }
      // FWD with Left Turn
      else if(x < 0.0f) {
         float offset_pwr = fabs(y + x);
         // Is our turning force larger than fwd force
         if(offset_pwr > y) {
            right_spd = offset_pwr;
            left_spd = y;
         }
         else {
            right_spd = y;
            left_spd = offset_pwr;
         }
      }
      // FWD Only
      else {
         left_spd = y;
         right_spd = y;
      }
   } 
   // BWD
   else if(y < 0.0f) {
      float pos_y = -y;
      // BWD with Right Turn
      if(x > 0.0f) {
         float offset_pwr = fabs(pos_y - x);
         // Is our turning force larger than fwd force
         if(offset_pwr > pos_y) {
            right_spd = -offset_pwr;
            left_spd = -pos_y;
         }
         else {
            right_spd = -pos_y;
            left_spd = -offset_pwr;
         }
      }
      // BWD with Left Turn
      else if(x < 0.0f) {
         float offset_pwr = fabs(pos_y + x);
         // Is our turning force larger than fwd force
         if(offset_pwr > pos_y) {
            left_spd = -offset_pwr;
            right_spd = -pos_y;
         }
         else {
            left_spd = -pos_y;
            right_spd = -offset_pwr;
         }
      }
      // BWD Only
      else {
         left_spd = -pos_y;
         right_spd = -pos_y;
      }
   }
   
   // Turn Only
   else {
      // Right Turn
      if(x > 0.0f) {
         right_spd = -x;
         left_spd = x;
      }
      // Left Turn
      else if(x < 0.0f) {
         right_spd = -x;
         left_spd = x;
      }
      // Stopping
      else {
         right_spd = 0.0f;
         left_spd = 0.0f;
      }
   }
}

void setup() {
   #ifdef DEBUG
      Serial.begin(115200);
      while(!Serial){delay(5);};
   #endif
   
   pinMode(L_IN1, OUTPUT);
   pinMode(L_IN2, OUTPUT);
   pinMode(R_IN1, OUTPUT);
   pinMode(R_IN2, OUTPUT);
   pinMode(motorEnable, OUTPUT);
   digitalWrite(motorEnable, 0);
   
   bucket.attach(22);
   bucket.write(116);
   #ifdef DEBUG
      Serial.println("\n[BOOT] BLE Protocol Example (no lambdas)");
   #endif
   ble_joycon.setFrameExtra(&extra_frame_processing);
   ble_joycon.setup();
   ble_joycon.start();

}

// uint32_t lastPing = 0;

void loop() {
   // if (ble_joycon.isConnected()) {
   //    uint32_t now = millis();
   //    if (now - lastPing > 5000) {
   //       const char* txt = "ping";
   //       ble_joycon.sendMessage(GC_BLE_MSG::PING, reinterpret_cast<const uint8_t*>(txt), strlen(txt));
   //       lastPing = now;
   //       Serial.println("[TX] Sent PING");
   //    }
   // }
   if(!ble_joycon.isConnected()) {
      driveHBridge(L_IN1, L_IN2, 0);
      driveHBridge(R_IN1, R_IN2, 0);
      delay(20);
      return;
   }
   
   float left_spd = 0.0f;
   float right_spd = 0.0f;
   
   stick_t left = ble_joycon.getLeftStick();
   stick_t right = ble_joycon.getRightStick();
   applyDeadzone(left, 0.10f);
   applyDeadzone(right, 0.10f);
   
   if(ble_joycon.isSingleStick()) {
      if(ble_joycon.isLeftSingleStick()) {
         singleJoystickControl(left.x, -left.y, left_spd, right_spd);
      }
      // Right Stick
      else {
         singleJoystickControl(right.x, -right.y, left_spd, right_spd);
      }
   }
   // Dual Stick Drive (Y-Axis Drive)
   else {
      left_spd = -left.y;
      right_spd = -right.y;
   }
   
   if(ble_joycon.isDpadUpPressed()) {
      lifter_pos = -255;
   }
   else if(ble_joycon.isDpadDownPressed()) {
      lifter_pos = 255;
   }
   else {
      lifter_pos = 0;
   }
   
   if(ble_joycon.isDpadRightPressed()) {
      bucket_pivot += lifter_speed;
      if(bucket_pivot > 116) bucket_pivot = 116;
   }
   else if(ble_joycon.isDpadLeftPressed()) {
      bucket_pivot -= lifter_speed;
      if(bucket_pivot < 0) bucket_pivot = 0;
   }
   
   int leftPWM  = left_spd  * 255;
   int rightPWM = right_spd * 255;
   
   // Output
   digitalWrite(motorEnable, 1);
   driveHBridge(L_IN1, L_IN2, leftPWM);
   driveHBridge(R_IN2, R_IN1, rightPWM);
   driveHBridge(LI_IN1, LI_IN2, lifter_pos);
   bucket.write(bucket_pivot);
   
   delay(20);
}
