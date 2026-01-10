---
name: /clean
description: 清理SMF构建目录
allowed-tools: [Bash, ExecuteCommand]
---

# 清理SMF项目

## 完全清理构建目录
```bash
rm -rf build
```

## 保留CMake缓存但重新构建
```bash
cd build && make clean
```

## 清理并重新配置
```bash
rm -rf build && mkdir build && cd build && cmake ..
```