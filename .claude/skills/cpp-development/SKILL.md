# C++开发技能

## 适用范围
C++高性能项目开发、CMake配置、RPC系统设计、异步编程

## 核心能力
- CMake配置分析和优化
- C++23代码审查和最佳实践
- 高性能RPC系统设计指导
- 异步编程模式（基于Seastar框架）
- 依赖管理和第三方库集成

## 技术栈专长
- **编译器**: g++-15, C++23标准
- **构建系统**: CMake 3.16+
- **异步框架**: Seastar
- **序列化**: FlatBuffers
- **性能优化**: 微秒级延迟调优

## 常用分析模式

### CMake配置分析
```bash
# 读取主CMake配置
read_file: CMakeLists.txt

# 分析依赖配置
search_content: "find_package"
search_content: "target_link_libraries"
```

### 代码质量检查
```bash
# 搜索代码模式
search_content: "TODO|FIXME|XXX"
search_content: "LOG_DEBUG|LOG_INFO|LOG_ERROR"

# 分析性能关键代码
search_content: "future<.*>"
search_content: "co_await|co_return"
```

### 项目结构理解
```bash
# 探索目录结构
list_files: src/ depth=3
list_files: seastar/ depth=2

# 理解模块关系
search_content: "#include.*smf"
search_content: "using namespace smf"
```

## 最佳实践建议

### C++23代码规范
- 使用`std::source_location`替代`__FILE__`和`__LINE__`
- 优先使用协程进行异步编程
- 使用概念(concepts)进行模板约束
- 利用结构化绑定简化代码

### 性能优化原则
- 零拷贝序列化（FlatBuffers）
- 异步I/O和非阻塞操作
- 内存池和对象复用
- 避免不必要的锁竞争

### CMake配置优化
- 使用现代CMake目标属性
- 合理设置编译标志
- 管理依赖版本兼容性
- 配置交叉编译支持

## 常见问题解决

### 构建问题
- 依赖库版本冲突
- 编译器兼容性问题
- 链接器错误处理

### 性能问题
- 内存分配优化
- 网络I/O调优
- 并发控制策略

### 调试技巧
- 异步调用栈跟踪
- 性能剖析工具使用
- 内存泄漏检测