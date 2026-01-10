---
name: /build
description: 构建SMF项目
allowed-tools: [Bash, ExecuteCommand]
---

# 构建SMF项目

## 快速构建（Release模式）
```bash
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 带测试的构建
```bash
rm -rf build && mkdir build && cd build
cmake .. -DSMF_ENABLE_TESTS=ON
make -j$(nproc)
```

## 静默构建（关闭警告）
```bash
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-w -Wno-note"
make -j$(nproc)
```