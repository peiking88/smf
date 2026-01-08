// Copyright (c) 2016 Alexander Gallego. All rights reserved.
//

#pragma once
#ifndef SMF_PLATFORM_LOG_H
#define SMF_PLATFORM_LOG_H

#include <fmt/format.h>
#include <seastar/util/log.hh>
#include <source_location>
#include <stdexcept>
#include <string>

#include "smf/macros.h"

namespace smf {

struct internal_logger {
  static seastar::logger &get() {
    static seastar::logger l("smf");
    return l;
  }
};

namespace log_detail {

// Helper to extract filename from path
inline std::string_view extract_filename(const char* path) {
  std::string_view sv(path);
  auto pos = sv.rfind('/');
  if (pos != std::string_view::npos) {
    return sv.substr(pos + 1);
  }
  return sv;
}

// A small helper for throw_if_null().
template <typename T>
T* throw_if_null(const char *file, int line, const char *names, T *t) {
  if (SMF_UNLIKELY(t == nullptr)) {
    auto s = fmt::format("{}:{} check_not_null({})", extract_filename(file), line, names);
    smf::internal_logger::get().error("{}", s);
    throw std::runtime_error(s);
  }
  return t;
}

template <typename... Args>
inline void noop(Args &&...) {}

// Log functions with source location
template <typename... Args>
inline void log_info(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().info("{}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void log_error(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().error("{}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void log_warn(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().warn("{}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void log_debug(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().debug("{}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void log_trace(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  if (SMF_UNLIKELY(smf::internal_logger::get().is_enabled(seastar::log_level::trace))) {
    auto msg = fmt::format(fmt, std::forward<Args>(args)...);
    smf::internal_logger::get().trace("{}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
  }
}

template <typename... Args>
[[noreturn]] inline void log_throw(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  auto full_msg = fmt::format("{}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
  smf::internal_logger::get().error("{}", full_msg);
  throw std::runtime_error(full_msg);
}

template <typename... Args>
inline void log_throw_if(bool condition, std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  if (SMF_UNLIKELY(condition)) {
    auto msg = fmt::format(fmt, std::forward<Args>(args)...);
    auto full_msg = fmt::format("{}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
    smf::internal_logger::get().error("{}", full_msg);
    throw std::runtime_error(full_msg);
  }
}

// Debug versions
template <typename... Args>
inline void dlog_info(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().info("D {}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void dlog_error(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().error("D {}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void dlog_warn(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().warn("D {}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void dlog_debug(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  smf::internal_logger::get().debug("D {}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
}

template <typename... Args>
inline void dlog_trace(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  if (SMF_UNLIKELY(smf::internal_logger::get().is_enabled(seastar::log_level::trace))) {
    auto msg = fmt::format(fmt, std::forward<Args>(args)...);
    smf::internal_logger::get().trace("D {}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
  }
}

template <typename... Args>
[[noreturn]] inline void dlog_throw(std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  auto msg = fmt::format(fmt, std::forward<Args>(args)...);
  auto full_msg = fmt::format("D {}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
  smf::internal_logger::get().error("{}", full_msg);
  throw std::runtime_error(full_msg);
}

template <typename... Args>
inline void dlog_throw_if(bool condition, std::source_location loc, fmt::format_string<Args...> fmt, Args&&... args) {
  if (SMF_UNLIKELY(condition)) {
    auto msg = fmt::format(fmt, std::forward<Args>(args)...);
    auto full_msg = fmt::format("D {}:{} {}", extract_filename(loc.file_name()), loc.line(), msg);
    smf::internal_logger::get().error("{}", full_msg);
    throw std::runtime_error(full_msg);
  }
}

}  // namespace log_detail

/// brief Reliably takes effect inside a seastar app_
inline void app_run_log_level(seastar::log_level l) {
  smf::internal_logger::get().set_level(l);
  seastar::global_logger_registry().set_logger_level("smf", l);
}

}  // namespace smf

// Macro wrappers that capture source location
#define LOG_INFO(...) \
  smf::log_detail::log_info(std::source_location::current(), __VA_ARGS__)

#define LOG_ERROR(...) \
  smf::log_detail::log_error(std::source_location::current(), __VA_ARGS__)

#define LOG_WARN(...) \
  smf::log_detail::log_warn(std::source_location::current(), __VA_ARGS__)

#define LOG_DEBUG(...) \
  smf::log_detail::log_debug(std::source_location::current(), __VA_ARGS__)

#define LOG_TRACE(...) \
  smf::log_detail::log_trace(std::source_location::current(), __VA_ARGS__)

#define LOG_THROW(...) \
  smf::log_detail::log_throw(std::source_location::current(), __VA_ARGS__)

#define LOG_THROW_IF(condition, ...) \
  smf::log_detail::log_throw_if((condition), std::source_location::current(), __VA_ARGS__)

#define THROW_IFNULL(val) \
  smf::log_detail::throw_if_null(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

#define LOG_INFO_IF(condition, ...) \
  do { if (condition) { LOG_INFO(__VA_ARGS__); } } while (false)

#define LOG_ERROR_IF(condition, ...) \
  do { if (condition) { LOG_ERROR(__VA_ARGS__); } } while (false)

#define LOG_DEBUG_IF(condition, ...) \
  do { if (condition) { LOG_DEBUG(__VA_ARGS__); } } while (false)

#define LOG_WARN_IF(condition, ...) \
  do { if (condition) { LOG_WARN(__VA_ARGS__); } } while (false)

#define LOG_TRACE_IF(condition, ...) \
  do { if (condition) { LOG_TRACE(__VA_ARGS__); } } while (false)

#ifndef NDEBUG

#define DTHROW_IFNULL(val) \
  smf::log_detail::throw_if_null(__FILE__, __LINE__, "D '" #val "' Must be non NULL", (val))

#define DLOG_THROW(...) \
  smf::log_detail::dlog_throw(std::source_location::current(), __VA_ARGS__)

#define DLOG_INFO(...) \
  smf::log_detail::dlog_info(std::source_location::current(), __VA_ARGS__)

#define DLOG_ERROR(...) \
  smf::log_detail::dlog_error(std::source_location::current(), __VA_ARGS__)

#define DLOG_WARN(...) \
  smf::log_detail::dlog_warn(std::source_location::current(), __VA_ARGS__)

#define DLOG_DEBUG(...) \
  smf::log_detail::dlog_debug(std::source_location::current(), __VA_ARGS__)

#define DLOG_TRACE(...) \
  smf::log_detail::dlog_trace(std::source_location::current(), __VA_ARGS__)

#define DLOG_INFO_IF(condition, ...) \
  do { if (condition) { DLOG_INFO(__VA_ARGS__); } } while (false)

#define DLOG_ERROR_IF(condition, ...) \
  do { if (condition) { DLOG_ERROR(__VA_ARGS__); } } while (false)

#define DLOG_DEBUG_IF(condition, ...) \
  do { if (condition) { DLOG_DEBUG(__VA_ARGS__); } } while (false)

#define DLOG_WARN_IF(condition, ...) \
  do { if (condition) { DLOG_WARN(__VA_ARGS__); } } while (false)

#define DLOG_TRACE_IF(condition, ...) \
  do { if (condition) { DLOG_TRACE(__VA_ARGS__); } } while (false)

#define DLOG_THROW_IF(condition, ...) \
  smf::log_detail::dlog_throw_if((condition), std::source_location::current(), __VA_ARGS__)

#else
#define DTHROW_IFNULL(x) (x)
#define DLOG_INFO(...) ((void)0)
#define DLOG_ERROR(...) ((void)0)
#define DLOG_WARN(...) ((void)0)
#define DLOG_DEBUG(...) ((void)0)
#define DLOG_TRACE(...) ((void)0)
#define DLOG_THROW(...) ((void)0)
#define DLOG_INFO_IF(condition, ...) ((void)0)
#define DLOG_ERROR_IF(condition, ...) ((void)0)
#define DLOG_DEBUG_IF(condition, ...) ((void)0)
#define DLOG_WARN_IF(condition, ...) ((void)0)
#define DLOG_TRACE_IF(condition, ...) ((void)0)
#define DLOG_THROW_IF(condition, ...) ((void)0)
#endif

#endif  // SMF_PLATFORM_LOG_H
