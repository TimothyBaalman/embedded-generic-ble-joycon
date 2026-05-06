#include <ESP32Servo.h>
#include "generic_ble_joycon.hpp"

GenericBleJoycon ble_joycon;

// Plain C callback invoked for every valid Proto frame
void extra_frame_processing(const Proto::Frame& frame, void* ctx) {
   (void)ctx;
   
   switch (frame.msg_id) {
      default:
         Serial.print(" Extra msg to parse: ");
         Serial.println(frame.msg_id);
      break;
   }
}

#define L_IN1  12
#define L_IN2  14
#define R_IN1  13
#define R_IN2  15

#define LI_IN1 17
#define LI_IN2 16

#define motorEnable 26

#define BUCKET_PIVOT_MIN -180 // min bucket pivot angle (servo write value)
#define BUCKET_PIVOT_MAX 180 // max bucket pivot angle (servo write value)
#define BUCKET_PIVOT_SPEED 3 // Speed at which the bucket pivots (1-10, adjust as needed)

#define LIFT_SPEED 255 // Max speed for the lifter motor (0-255)

Servo bucket;
int lifter_cmd;
int bucket_pivot;

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

void setup() {
   // Serial.begin(115200);
   // while(!Serial);
   
   pinMode(L_IN1, OUTPUT);
   pinMode(L_IN2, OUTPUT);
   pinMode(R_IN1, OUTPUT);
   pinMode(R_IN2, OUTPUT);
   pinMode(motorEnable, OUTPUT);
   digitalWrite(motorEnable, 0);
   
   bucket.attach(22);
   bucket.write(116);
   
   // Serial.println("\n[BOOT] BLE Protocol Example (no lambdas)");
   ble_joycon.setFrameExtra(&extra_frame_processing);
   ble_joycon.setup();
   ble_joycon.start();

}

// uint32_t lastPing = 0;

void loop() {
   
   if(!ble_joycon.isConnected()) {
      driveHBridge(L_IN1, L_IN2, 0);
      driveHBridge(R_IN1, R_IN2, 0);
      digitalWrite(motorEnable, 0);
      return;
   }

   bool is_single_stick = ble_joycon.getFlag0();
   bool is_right_stick_drive = ble_joycon.getFlag1();

   if(is_single_stick){
      // Single Stick Mode
      if(is_right_stick_drive){
         // right stick drive
         driveHBridge(R_IN1, R_IN2, (-ble_joycon.getRightStick().x - ble_joycon.getRightStick().y)*255);
         driveHBridge(L_IN1, L_IN2, (ble_joycon.getRightStick().x - ble_joycon.getRightStick().y)*255);
         bucket_pivot += -ble_joycon.getLeftStick().x * BUCKET_PIVOT_SPEED; // Adjust sensitivity as needed
         if(bucket_pivot > BUCKET_PIVOT_MAX) bucket_pivot = BUCKET_PIVOT_MAX;
         if(bucket_pivot < BUCKET_PIVOT_MIN) bucket_pivot = BUCKET_PIVOT_MIN;
         bucket.write(bucket_pivot);
         lifter_cmd = -ble_joycon.getLeftStick().y * LIFT_SPEED;
         driveHBridge(LI_IN1, LI_IN2, lifter_cmd);
      }
      else
      {
         // left stick drive
         driveHBridge(R_IN1, R_IN2, (-ble_joycon.getLeftStick().x - ble_joycon.getLeftStick().y)*255);
         driveHBridge(L_IN1, L_IN2, (ble_joycon.getLeftStick().x - ble_joycon.getLeftStick().y)*255);
         bucket_pivot += -ble_joycon.getRightStick().x * BUCKET_PIVOT_SPEED; // Adjust sensitivity as needed
         if(bucket_pivot > BUCKET_PIVOT_MAX) bucket_pivot = BUCKET_PIVOT_MAX;
         if(bucket_pivot < BUCKET_PIVOT_MIN) bucket_pivot = BUCKET_PIVOT_MIN;
         bucket.write(bucket_pivot);
         lifter_cmd = -ble_joycon.getRightStick().y * LIFT_SPEED;
         driveHBridge(LI_IN1, LI_IN2, lifter_cmd);
      }

   }
   else
   {
      // Dual Stick Mode
      // Left Stick X axis controls bucket pivot, Left Stick Y axis controls left motor
      driveHBridge(L_IN1, L_IN2, -ble_joycon.getLeftStick().y * 255);
      bucket_pivot += -ble_joycon.getLeftStick().x * BUCKET_PIVOT_SPEED; // Adjust sensitivity as needed
      if(bucket_pivot > BUCKET_PIVOT_MAX) bucket_pivot = BUCKET_PIVOT_MAX;
      if(bucket_pivot < BUCKET_PIVOT_MIN) bucket_pivot = BUCKET_PIVOT_MIN;
      bucket.write(bucket_pivot);

      // Right Stick X axis controls arm speed, Right Stick Y axis controls right motor
      driveHBridge(R_IN1, R_IN2, -ble_joycon.getRightStick().y * 255);
      lifter_cmd = -ble_joycon.getRightStick().x * 255; // Adjust sensitivity as needed
      driveHBridge(LI_IN1, LI_IN2, lifter_cmd);
   }
   
   digitalWrite(motorEnable, 1);
   
   delay(20);
}
