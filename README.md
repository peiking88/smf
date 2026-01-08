# [smf - the fastest RPC in the West](http://smfrpc.github.io/smf/) [![Travis-ci Build Status](https://app.travis-ci.com/senior7515/smf.svg?branch=master)](https://travis-ci.org/smfrpc/smf)

---

## Refactoring Summary (2026-01-08)

This project has been refactored to support modern C++ standards and updated dependencies:

### Compiler & Standards
- Upgraded to **C++23** standard with **g++-15** compiler
- Updated to latest **Boost** and **fmt** libraries

### Log System Refactoring
- Replaced log macros (`LOG_INFO`, `LOG_DEBUG`, `LOG_ERROR`, etc.) with template functions
- Implemented using `std::source_location` (C++20) for automatic file/line/function capture
- Integrated with **Seastar logger** interface

### CMake Compatibility Fixes
- Added **CMP0167** policy setting to use Boost's own CMake config instead of deprecated FindBoost module (CMake 3.30+)

### Seastar API Compatibility Fixes
- **Metrics API**: Replaced deprecated `sm::make_derive` with `sm::make_counter` and `sm::make_gauge`
- **Server Socket**: Changed `lw_shared_ptr<server_socket>` to `std::optional<server_socket>`
- **Future API**: Replaced `.get0()` with `.get()`
- **Loop Iteration**: Replaced `boost::counting_iterator` with `seastar::repeat`

### FlatBuffers API Updates
- Fixed API compatibility issues with newer FlatBuffers versions

### fmt Library Integration
- Added `fmt::formatter` specializations for custom types:
  - `smf::human_bytes`
  - `smf::load_generator_duration`
  - `smf::load_generator_args`

---

![](docs/public/logo.png)

> We're looking for a new maintainer for the SMF project. As I have little time to keep up with issues. Please let me know by filing an issue.


**smf** is pronounced **/smɝf/**

Site         | Link
------------ | --------
Mailing List  | https://groups.google.com/forum/#!forum/smf-dev
Documentation | https://smfrpc.github.io/smf/

# [Official Documentation](https://smfrpc.github.io/smf)

Please visit our official documentation, 
it'll get you up and running in no time!

If you are using **smf**, drop us a line on the mailing list introducing your project. 


# What is smf?

**smf** is a new RPC system and code generation like gRPC, Cap n Proto,
Apache Thrift, etc, but designed for **microsecond tail latency***.

Current benchmarks in microseconds (e2e see docs)

| 60 byte payload  | latency   |
| ---------------- | --------- |
| p50              | 7us       |
| p90              | 8us       |
| p99              | 8us       |
| p9999            | 15us      |
| p100             | 26us      |



# Getting started

Please see our quick
[getting started on our official docs!](https://smfrpc.github.io/smf//getting_started/)

# how to build

```
git clone https://github.com/peiking88/smf.git
cd smf
#安装依赖包执行./install-deps.sh或手工运行以下命令
sudo apt update && sudo apt upgrade
sudo apt install -y flatbuffers-compiler libflatbuffers-dev
sudo apt install -y libgflags-dev libgoogle-glog-dev libzstd-dev libflatbuffers-dev flatbuffers-compiler libxxhash-dev systemtap-sdt-dev xfslibs-dev
#安装HdrHistogram_c
git clone https://bgithub.xyz/HdrHistogram/HdrHistogram_c.git
cd HdrHistogram_c && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local 
make && sudo make install
cd ..
mkdir build && cd build && cmake ..
make -j$(nproc)
```