// Copyright (c) 2016 Alexander Gallego. All rights reserved.
//
#pragma once
#include <iomanip>
#include <ostream>
#include <fmt/format.h>
namespace smf {

/// \brief prints a double as bytes, KB, MB, GB, TB, etc.
/// \code `std::cout << human_bytes(x) << std::endl;` \endcode
///
struct human_bytes {
  explicit human_bytes(double d) : data(d) {}
  double data;
};

// take by value
inline std::ostream &
operator<<(std::ostream &o, smf::human_bytes h) {
  auto const orig_precision = o.precision();
  o << std::fixed << std::setprecision(3);
  if (h.data < 1024) {
    if (h.data < 1) { h.data = 0; }
    o << h.data << " bytes";
  } else if ((h.data /= static_cast<double>(1024)) <
             static_cast<double>(1000)) {
    o << h.data << " KB";
  } else if ((h.data /= static_cast<double>(1024)) <
             static_cast<double>(1000)) {
    o << h.data << " MB";
  } else if ((h.data /= static_cast<double>(1024)) <
             static_cast<double>(1000)) {
    o << h.data << " GB";
  } else {
    o << h.data << " TB";
  }
  // reset to original
  o << std::setprecision(orig_precision);
  return o;
}

}  // namespace smf

template <>
struct fmt::formatter<smf::human_bytes> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
  
  template <typename FormatContext>
  auto format(smf::human_bytes h, FormatContext& ctx) const {
    if (h.data < 1024) {
      if (h.data < 1) { h.data = 0; }
      return fmt::format_to(ctx.out(), "{:.3f} bytes", h.data);
    } else if ((h.data /= 1024.0) < 1000.0) {
      return fmt::format_to(ctx.out(), "{:.3f} KB", h.data);
    } else if ((h.data /= 1024.0) < 1000.0) {
      return fmt::format_to(ctx.out(), "{:.3f} MB", h.data);
    } else if ((h.data /= 1024.0) < 1000.0) {
      return fmt::format_to(ctx.out(), "{:.3f} GB", h.data);
    } else {
      return fmt::format_to(ctx.out(), "{:.3f} TB", h.data);
    }
  }
};
