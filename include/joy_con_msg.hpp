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
   uint8_t reserved : 6;
   uint8_t is_left_single_stick : 1;
   uint8_t is_single_stick : 1;
};

struct __attribute__((packed)) rx_joy_con_msg_t {
   int8_t left_x = 0; // scale 0.01 Left - ... Right +
   int8_t left_y = 0; // scale 0.01 Up - ... Down +
   int8_t right_x = 0; // scale 0.01 Left - ... Right +
   int8_t right_y = 0; // scale 0.01 Up - ... Down +
   joy_con_btns_t btns;
   int8_t max_throttle = 0; // scale 0.01
   int8_t trim = 0; // scale 0.01
   joy_con_flags_t flags;
   
   uint8_t msg_idx = 0;
};
