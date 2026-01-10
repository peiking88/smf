---
name: /test
description: 运行SMF测试
allowed-tools: [Bash, ExecuteCommand]
---

# 运行SMF测试

## 构建并运行测试
```bash
cd build
cmake .. -DSMF_ENABLE_TESTS=ON
make -j$(nproc)
ctest --verbose
```

## 仅运行测试（如果已构建）
```bash
cd build && ctest
```

## 运行基准测试
```bash
cd build && make benchmarks
./bin/benchmarks
```

## 运行特定测试
```bash
cd build && ctest -R "test_name"
```