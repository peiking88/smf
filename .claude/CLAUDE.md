# SMF RPC Framework

## 项目概述
高性能C++ RPC框架，专为微秒级延迟设计。基于Seastar异步框架构建，提供微秒级的尾部延迟性能。

## 构建命令
```bash
# 清理构建
rm -rf build && mkdir build && cd build

# 配置构建
cmake .. -DCMAKE_BUILD_TYPE=Release

# 并行构建
make -j$(nproc)

# 启用测试
cmake .. -DSMF_ENABLE_TESTS=ON
```

## 技术栈
- **语言**: C++23
- **框架**: Seastar异步框架
- **构建系统**: CMake
- **序列化**: FlatBuffers
- **压缩**: Zstd, LZ4
- **哈希**: xxHash
- **直方图**: HdrHistogram

## 性能指标
- p50延迟: 7微秒
- p99延迟: 8微秒  
- p9999延迟: 15微秒
- 最大延迟: 26微秒

## 关键目录
- `src/core/` - RPC核心实现（客户端、服务器、压缩）
- `src/smfc/` - 代码生成器（支持C++、Go、Python）
- `seastar/` - 异步框架子模块
- `demo_apps/` - 示例应用
- `cmake/` - CMake配置模块

## 开发约定
- 使用g++-15编译器
- 遵循C++23标准
- 启用-Werror在Debug模式
- 使用FlatBuffers进行序列化
- 异步编程基于Seastar框架

## 常用CMake选项
```bash
# 禁用警告（生产构建）
cmake .. -DCMAKE_CXX_FLAGS="-w -Wno-note"

# 启用HWLOC支持
cmake .. -DSMF_ENABLE_HWLOC=ON

# 启用DPDK支持
cmake .. -DSMF_ENABLE_DPDK=ON

# 构建程序示例
cmake .. -DSMF_BUILD_PROGRAMS=ON
```

## 依赖安装
```bash
# 系统依赖
sudo apt install -y flatbuffers-compiler libflatbuffers-dev libgflags-dev libgoogle-glog-dev libzstd-dev libxxhash-dev

# HdrHistogram安装
git clone https://bgithub.xyz/HdrHistogram/HdrHistogram_c.git
cd HdrHistogram_c && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local 
make && sudo make install
```

## 代码生成
使用smfc工具生成客户端和服务端代码：
```bash
./bin/smfc -i protocol.fbs -o generated/
```

## 测试
```bash
# 运行单元测试
cd build && ctest

# 运行基准测试
cd build && make benchmarks && ./bin/benchmarks
```

## 维护状态
项目正在寻找新的维护者，请参考CONTRIBUTING.md了解贡献流程。