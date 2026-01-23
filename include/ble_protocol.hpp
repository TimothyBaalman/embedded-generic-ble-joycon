
#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "ble_msg_ids.hpp"

namespace Proto {
   struct Frame {
      uint8_t msg_id;
      std::vector<uint8_t> payload;
   };
   
   inline void addChecksum(std::vector<uint8_t>& p) noexcept {
      uint8_t checksum = 0;
      // -1 because the assume msg_len should include the checksum in the len
      for(uint8_t i = 0; i < p.size(); i++) {
         checksum ^= p[i];
      }
      p.push_back(checksum);
   }
   
   inline std::vector<uint8_t> encode(const uint8_t& msg_id, const uint8_t* payload, const size_t& len) {
      std::vector<uint8_t> out;
      out.reserve(3 + len);
      out.push_back(static_cast<uint8_t>(HEAD_BYTE_0));
      out.push_back(static_cast<uint8_t>(HEAD_BYTE_1));
      out.push_back(msg_id);
      if (payload && len > 0) {
         out.insert(out.end(), payload, payload + len);
      }
      addChecksum(out);
      return out;
   }
   
   inline std::vector<uint8_t> encode(const uint8_t& msg, const std::vector<uint8_t>& payload) {
      return encode(msg, payload.data(), payload.size());
   }
   
   /**
    * Decode a buffer into a Frame.
    * Returns true on success (headers match and at least 3 bytes present).
    */
   inline bool decode(const uint8_t* data, size_t len, Frame& out) {
      Serial.print("Got data: [ ");
      for(size_t i = 0; i<len; i++){
         Serial.print("0x");
         Serial.print(data[i], HEX);
         Serial.print(", ");
      }
      Serial.println();

      if(!data || len < 4) {
         Serial.println("No data or under len of 4");
         return false;               // need at least H1,H2,ID,CHK
      }
      if (data[0] != HEAD_BYTE_0 || data[1] != HEAD_BYTE_1) {
         Serial.print("Incorrect header bytes: 0x");
         Serial.print(data[0], HEX);
         Serial.print(", 0x");
         Serial.println(data[1], HEX);
         return false;               // need at least H1,H2,ID,CHK
      }
      
      out.msg_id = data[2];
      const size_t payload_len = (len - 3);
      if (payload_len == 0) return false;
      const uint8_t recv_chk = data[len-1];
      
      uint8_t calc = 0;
      for (size_t i = 0; i < len - 1; ++i) {
         calc ^= data[i]; 
      }
      if (calc != recv_chk) {
         Serial.print("Checksum Failed: rx 0x");
         Serial.print(recv_chk, HEX);
         Serial.print(" vs c: 0x");
         Serial.println(calc, HEX);
         // return false; // Cyheck
      }
      
      out.payload.clear();
      if (payload_len > 1) {
         // exclude checksum (last byte)
         out.payload.insert(out.payload.end(), data + 3, data + (len - 1));
      }
      return true;
   }
   
   inline bool decode(const std::vector<uint8_t>& buf, Frame& out) {
      return decode(buf.data(), buf.size(), out);
   }
} // namespace Proto
