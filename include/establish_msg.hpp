#pragma once

#include <cstdint>
#include "ble_msg_ids.hpp"

struct __attribute__((packed)) rx_establish_t {
   // Maybe add secret hash here too?
   uint8_t seed = 0xFF;
   uint8_t msg_idx = 0;
};

struct __attribute__((packed)) tx_establish_t {
   // Maybe add secret hash here too?
   uint8_t seed = 0xFF;
   uint8_t msg_idx = 0;
};