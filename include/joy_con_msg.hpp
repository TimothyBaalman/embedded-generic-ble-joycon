#pragma once

#include <cstdint>

struct __attribute__((packed)) joy_con_btns_t {
   uint8_t btn_a : 1;
   uint8_t btn_b : 1;
   uint8_t btn_x : 1;
   uint8_t btn_y : 1;
   
   uint8_t dpad_l : 1;
   uint8_t dpad_u : 1;
   uint8_t dpad_r : 1;
   uint8_t dpad_d : 1;
};

struct __attribute__((packed)) joy_con_flags_t {
   uint8_t flag_0 : 1;
   uint8_t flag_1 : 1;
   uint8_t flag_2 : 1;
   uint8_t flag_3 : 1;
   uint8_t flag_4 : 1;
   uint8_t flag_5 : 1;
   uint8_t flag_6 : 1;
   uint8_t flag_7 : 1;
};

struct __attribute__((packed)) rx_joy_con_msg_t {
   int8_t left_x = 0; // scale 0.01 Left - ... Right +
   int8_t left_y = 0; // scale 0.01 Up - ... Down +
   int8_t right_x = 0; // scale 0.01 Left - ... Right +
   int8_t right_y = 0; // scale 0.01 Up - ... Down +
   joy_con_btns_t btns;
   int8_t slider2 = 0; // scale 0.01
   int8_t slider1 = 0; // scale 0.01
   joy_con_flags_t flags;
   
   uint8_t msg_idx = 0;
};
