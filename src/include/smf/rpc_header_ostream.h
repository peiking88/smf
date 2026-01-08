// Copyright (c) 2018 Alexander Gallego. All rights reserved.
//
#pragma once
#include <bitset>
#include <iostream>
#include <sstream>

#include <fmt/format.h>
#include <smf/rpc_generated.h>

namespace std {
inline ostream &
operator<<(ostream &o, const ::smf::rpc::header &h) {
  o << "rpc::header={compression:"
    << ::smf::rpc::EnumNamecompression_flags(h.compression())
    << ", header_bit_flags:"
    << std::bitset<8>(static_cast<uint8_t>(h.bitflags()))
    << ", session:" << h.session() << ", size:" << h.size()
    << ", checksum:" << h.checksum() << ", meta:" << h.meta() << "}";
  return o;
}
}  // namespace std

// fmt formatter for smf::rpc::header
template <>
struct fmt::formatter<smf::rpc::header> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
  
  template <typename FormatContext>
  auto format(const smf::rpc::header& h, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(),
      "rpc::header={{compression:{}, header_bit_flags:{}, session:{}, size:{}, checksum:{}, meta:{}}}",
      smf::rpc::EnumNamecompression_flags(h.compression()),
      static_cast<uint8_t>(h.bitflags()),
      h.session(), h.size(), h.checksum(), h.meta());
  }
};
