# 随机点名器 (Name Roller)

一个基于 SFML 多媒体库开发的 C++ 随机点名器，支持开场视频动画、角色立绘显示和自定义资源。

## ✨ 功能特点

- 🎬 **开场视频动画** - 支持播放视频作为开场动画，营造仪式感
- 🎭 **角色立绘显示** - 每个名字对应一张立绘图片，点名时同步显示
- 🎨 **自定义资源** - 轻松替换名字列表、立绘图片、视频和字体
- ⚡ **流畅动画** - 名字滚动效果、淡入淡出、缩放动画
- 🎯 **不重复点名** - 支持不重复点名模式，确保公平性
- 📱 **跨平台** - 基于 SFML，支持 Windows、Linux、macOS

## 📁 项目结构

```
name-roller/
├── CMakeLists.txt          # CMake 构建配置
├── README.md               # 项目说明（本文件）
├── src/                    # 源代码目录
│   ├── main.cpp            # 主程序入口
│   ├── NameRoller.h        # 点名器核心类
│   ├── NameRoller.cpp
│   ├── VideoPlayer.h       # 视频播放器类
│   ├── VideoPlayer.cpp
│   ├── CharacterDisplay.h  # 角色立绘显示类
│   ├── CharacterDisplay.cpp
│   ├── ResourceManager.h   # 资源管理器类
│   └── ResourceManager.cpp
├── resources/              # 资源文件目录
│   ├── video/              # 开场视频
│   │   ├── intro.mp4       # 视频文件
│   │   └── frames/         # 帧序列（备选方案）
│   ├── characters/         # 角色立绘图片
│   ├── fonts/              # 字体文件
│   └── config/             # 配置文件
│       └── names.txt       # 名字列表
└── docs/                   # 文档目录
    └── COMPILE_GUIDE.md    # 详细编译教程
```

## 🚀 快速开始

### 前置要求

- C++17 兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）
- CMake 3.16 或更高版本
- SFML 2.5 或更高版本
- （可选）sfeMovie 库（用于完整视频播放支持）

### 编译步骤

#### 1. 安装 SFML

**Ubuntu/Debian:**
```bash
sudo apt-get install libsfml-dev
```

**macOS (Homebrew):**
```bash
brew install sfml
```

**Windows:**
下载 SFML SDK 并解压，设置环境变量或在 CMake 中指定路径。

#### 2. 编译项目

```bash
# 进入项目目录
cd name-roller

# 创建构建目录
mkdir build && cd build

# 配置 CMake
cmake ..

# 编译
make -j$(nproc)
```

#### 3. 运行程序

```bash
# 在 build 目录下运行
./NameRoller
```

> 💡 详细的编译指南请参考 [docs/COMPILE_GUIDE.md](docs/COMPILE_GUIDE.md)

## 🎮 操作说明

| 按键 | 功能 |
|------|------|
| **空格键** | 开始点名 / 停止点名 / 下一轮 |
| **空格键**（开场时） | 跳过开场动画 |
| **Ctrl + R** | 重置点名记录 |
| **ESC** | 返回准备状态 |
| **关闭窗口** | 退出程序 |

## 🎨 自定义资源

### 替换名字列表

编辑 `resources/config/names.txt`，每行一个名字：

```
张三
李四
王五
# 这是注释，会被忽略
赵六
```

### 添加角色立绘

将立绘图片放入 `resources/characters/` 目录，文件名（不含扩展名）与名字一致：

```
resources/characters/
├── 张三.png    # 对应名字"张三"
├── 李四.png    # 对应名字"李四"
└── 王五.jpg    # 对应名字"王五"
```

支持格式：PNG（推荐，支持透明背景）、JPG、BMP

### 替换开场视频

**方式一：使用视频文件（需要 sfeMovie）**
- 将视频文件命名为 `intro.mp4`
- 放入 `resources/video/` 目录

**方式二：使用帧序列（无需额外库）**
- 将视频导出为帧序列图片
- 放入 `resources/video/frames/` 目录
- 文件名按顺序编号：`001.png`, `002.png`...

使用 FFmpeg 导出帧序列：
```bash
ffmpeg -i intro.mp4 -vf fps=30 resources/video/frames/%03d.png
```

### 替换字体

将字体文件（`.ttf` 或 `.otf`）放入 `resources/fonts/` 目录。

推荐中文字体：
- 思源黑体（Source Han Sans）
- 微软雅黑
- 文泉驿微米黑

> 📖 更多资源替换说明请参考 [resources/README.md](resources/README.md)

## 🏗️ 架构设计

### 模块化设计

项目采用面向对象的模块化设计，各模块职责清晰：

| 模块 | 职责 |
|------|------|
| **ResourceManager** | 统一管理所有资源（图片、字体、配置） |
| **VideoPlayer** | 视频播放，支持 sfeMovie 和帧序列两种模式 |
| **CharacterDisplay** | 角色立绘显示，带动画效果 |
| **NameRoller** | 点名器核心逻辑，状态管理 |
| **main** | 程序入口，主循环和事件处理 |

### 状态机

点名器使用状态机管理不同阶段：

```
Intro（开场动画）
    ↓
Ready（准备就绪） ←─────┐
    ↓                   │
Rolling（名字滚动）      │
    ↓                   │
Result（显示结果） ──────┘
```

## 🛠️ 技术栈

- **语言**: C++17
- **图形库**: SFML 2.5+
- **视频库**: sfeMovie（可选）
- **构建系统**: CMake 3.16+

## 📝 开发说明

### 代码风格

- 使用驼峰命名法（camelCase）
- 类名使用大驼峰（PascalCase）
- 成员变量以 `m_` 前缀开头
- 详细的 Doxygen 风格注释

### 扩展建议

- 添加背景音乐和音效
- 支持多种点名模式（随机、顺序、分组）
- 添加历史记录功能
- 支持导出点名结果
- 添加更多动画效果

## 📄 许可证

本项目仅供学习和个人使用。

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

---

**享受点名的乐趣吧！🎉**
