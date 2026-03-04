
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
   
   // #define MAX_PAYLOAD_BYTES 238
   // #define MAX_PACK_MSG_SIZE 242
   
   // static inline void put_u8(uint8_t* dst, size_t& off, const uint8_t& v) {
   //    dst[off] = v;
   //    ++off;
   // }
   // static inline void put_u16_le(uint8_t* dst, size_t& off, const uint16_t& v) {
   //    dst[off++] = static_cast<uint8_t>(v & 0xFF);
   //    dst[off++] = static_cast<uint8_t>((v >> 8) & 0xFF);
   // }
   // static inline void put_u32_le(uint8_t* dst, size_t& off, const uint32_t& v) {
   //    dst[off++] = static_cast<uint8_t>(v & 0xFF);
   //    dst[off++] = static_cast<uint8_t>((v >> 8) & 0xFF);
   //    dst[off++] = static_cast<uint8_t>((v >> 16) & 0xFF);
   //    dst[off++] = static_cast<uint8_t>((v >> 24) & 0xFF);
   // }
   // static inline void put_f32_le(uint8_t* dst, size_t& off, float v) {
   //    static_assert(sizeof(float) == 4, "float must be 4 bytes");
      
   //    uint32_t bits;
   //    std::memcpy(&bits, &v, sizeof(bits)); // reinterpret bits safely
   //    put_u32_le(dst, off, bits);
   // }
   // static inline void put_f64_le(uint8_t* dst, size_t& off, const double& v) {
   //    static_assert(sizeof(double) == 8, "double must be 8 bytes");
   //    uint64_t u;
   //    std::memcpy(&u, &v, 8);
   //    // Write as little-endian bytes:
   //    for (int b = 0; b < 8; ++b) {
   //       dst[off++] = static_cast<uint8_t>((u >> (8 * b)) & 0xFF);
   //    }
   // }
   
   // static inline bool get_u8(const uint8_t* src, uint8_t& off, const uint8_t& cap, uint8_t& out) {
   //    if (off + 1 > cap) return false;
   //    out = src[off++];
   //    return true;
   // }
   // static inline bool get_u16_le(const uint8_t* src, uint8_t& off, const uint8_t& cap, uint16_t& out) {
   //    if (off + 2 > cap) return false;
   //    out = static_cast<uint16_t>(src[off])
   //       | (static_cast<uint16_t>(src[off + 1]) << 8);
   //    off += 2;
   //    return true;
   // }
   // static inline bool get_u32_le(const uint8_t* src, uint8_t& off, const uint8_t& cap, uint32_t& out) {
   //    if (off + 4 > cap) return false;
   //    out =  static_cast<uint32_t>(src[off])
   //       | (static_cast<uint32_t>(src[off + 1]) << 8)
   //       | (static_cast<uint32_t>(src[off + 2]) << 16)
   //       | (static_cast<uint32_t>(src[off + 3]) << 24);
   //    off += 4;
   //    return true;
   // }
   
   // static inline bool get_f32_le(const uint8_t* src, uint8_t& off, const uint8_t& cap, float& out) {
   //    static_assert(sizeof(float) == 4, "float must be 4 bytes");
   //    if (off + 4 > cap) return false;
   //    std::memcpy(&out, src + off, 4);
   //    off += 4;
   //    return true;
   // }
   
   // static inline bool get_f64_le(const uint8_t* src, uint8_t& off, const uint8_t& cap, double& out) {
   //    static_assert(sizeof(double) == 8, "double must be 8 bytes");
   //    if (off + 8 > cap) return false;
   //    std::memcpy(&out, src + off, 8);
   //    off += 8;
   //    return true;
   // }
   
   // struct decode_packet_t {
   //    uint8_t hdr_byte_0 = HEAD_BYTE_0;
   //    uint8_t hdr_byte_1 = HEAD_BYTE_1;
   //    uint8_t msg_id = 0;
      
   //    uint8_t paylod_len = 0;
   //    uint8_t paylod[MAX_PAYLOAD_BYTES];
      
   //    decode_packet_t(const uint8_t& size) : paylod_len(size) {}
   //    decode_packet_t() : paylod_len(0) {}
      
   //    uint8_t ck_sum = 0;
   // };
   
   // struct encode_packet_t {
   //    uint8_t msg_len = 0;
   //    uint8_t msg[MAX_PACK_MSG_SIZE];
   //    encode_packet_t(uint8_t size) : msg_len(size) {}
   //    encode_packet_t() : msg_len(0) {}
   // };
   
   // inline void set_xor_checksum(decode_packet_t& packet) {
   //    packet.ck_sum = 0;
   //    packet.ck_sum ^= packet.hdr_byte_0;
   //    packet.ck_sum ^= packet.hdr_byte_1;
   //    packet.ck_sum ^= packet.msg_id;
   //    for(size_t i = 0; i < packet.paylod_len; ++i) packet.ck_sum ^= packet.paylod[i];
   // }
   
   // inline uint8_t xor_checksum(const uint8_t* data, const size_t& len_without_checksum) {
   //    uint8_t c = 0;
   //    for (size_t i = 0; i < len_without_checksum; ++i) c ^= data[i];
   //    return c;
   // }
   
   // inline bool decode(const uint8_t* data, const uint8_t& len, decode_packet_t& out) {
   //    Serial.print("Got data: [ ");
   //    for(size_t i = 0; i<len; i++){
   //       Serial.print("0x");
   //       Serial.print(data[i], HEX);
   //       Serial.print(", ");
   //    }
   //    Serial.println();
      
   //    if(!data || len < 4) {
   //       Serial.println("No data or under len of 4");
   //       return false;               // need at least H1,H2,ID,CHK
   //    }
      
   //    if(data[0] != HEAD_BYTE_0 || data[1] != HEAD_BYTE_1) {
   //       Serial.print("Incorrect header bytes: 0x");
   //       Serial.print(data[0], HEX);
   //       Serial.print(", 0x");
   //       Serial.println(data[1], HEX);
   //       return false;               // need at least H1,H2,ID,CHK
   //    }
   //    uint8_t offset = 0;
   //    out.hdr_byte_0 = data[offset];
   //    offset++;
   //    out.hdr_byte_1 = data[offset];
   //    offset++;
   //    out.msg_id = data[offset];
   //    offset++;
   //    out.paylod_len = (len - offset);
      
   //    for(uint8_t i = 0; i < out.paylod_len; i++) {
   //       out.paylod[i] = data[i+offset];
   //    }
      
   //    set_xor_checksum(out);
   //    const uint8_t recv_chk = data[len-1];
      
   //    if (recv_chk != out.ck_sum) {
   //       Serial.print("Checksum Failed: rx 0x");
   //       Serial.print(recv_chk, HEX);
   //       Serial.print(" vs c: 0x");
   //       Serial.println(out.ck_sum, HEX);
   //       return false; // Check
   //    }
      
   //    return true;
   // }
   
   // inline encode_packet_t encode(const uint8_t& msg_id, const uint8_t* payload, const uint8_t& len) {
   //    encode_packet_t out(3+len+1);
   //    size_t i = 0;
      
   //    put_u8(out.msg, i, HEAD_BYTE_0);
   //    put_u8(out.msg, i, HEAD_BYTE_1);
   //    put_u8(out.msg, i, msg_id);
      
   //    for(uint8_t j = 0; j<len; j++) {
   //       put_u8(out.msg, i, payload[j]);
   //    }
      
   //    put_u8(out.msg, i, xor_checksum(out.msg, i));
      
   //    return out;
   // }
   using FrameCallback = void(*)(const Proto::Frame&, void*);
   
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
