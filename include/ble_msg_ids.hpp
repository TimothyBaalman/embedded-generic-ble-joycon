
#pragma once
#include <cstdint>

#define HEAD_BYTE_0 0x47// G
#define HEAD_BYTE_1 0x43// C

namespace GC_BLE_MSG {
   enum : uint8_t {
      PING       = 0xF1,
      PONG       = 0xF2,
      ECHO       = 0xF3,
      
      ESTAB_CON = 0x00,
      JOYCON    = 0x01,
      LARGE     = 0x02
   };
}
