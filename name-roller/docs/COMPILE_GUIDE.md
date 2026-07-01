# 编译教程

本教程将详细指导你如何在不同平台上编译随机点名器项目。

## 📋 目录

- [Linux 编译指南](#linux-编译指南)
- [macOS 编译指南](#macos-编译指南)
- [Windows 编译指南](#windows-编译指南)
- [sfeMovie 安装（可选）](#sfemovie-安装可选)
- [常见问题](#常见问题)

---

## 🐧 Linux 编译指南

### 1. 安装依赖

#### Ubuntu / Debian / Linux Mint

```bash
# 更新软件包列表
sudo apt update

# 安装编译工具
sudo apt install build-essential cmake git

# 安装 SFML 开发库
sudo apt install libsfml-dev

# （可选）安装 sfeMovie 用于视频播放
# 注意：部分发行版可能没有预编译包，需要从源码编译
sudo apt install libsfemovie-dev  # 如果可用的话
```

#### Fedora / CentOS / RHEL

```bash
# Fedora
sudo dnf install gcc-c++ cmake make SFML-devel

# CentOS/RHEL (需要启用 EPEL 源)
sudo yum install gcc-c++ cmake make SFML-devel
```

#### Arch Linux / Manjaro

```bash
sudo pacman -S base-devel cmake sfml

# 可选：sfeMovie (AUR)
yay -S sfemovie-git
```

### 2. 编译项目

```bash
# 进入项目目录
cd name-roller

# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake ..

# 编译（-j 后面跟 CPU 核心数，加速编译）
make -j$(nproc)
```

### 3. 运行程序

```bash
# 在 build 目录下运行
./NameRoller

# 或者指定资源目录
./NameRoller ../resources
```

---

## 🍎 macOS 编译指南

### 1. 安装依赖

推荐使用 [Homebrew](https://brew.sh/) 安装依赖：

```bash
# 安装 Homebrew（如果还没安装）
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装编译工具
brew install cmake

# 安装 SFML
brew install sfml

# 可选：安装 sfeMovie（需要从源码编译）
# 参考下方的 sfeMovie 安装章节
```

### 2. 编译项目

```bash
# 进入项目目录
cd name-roller

# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake ..

# 编译
make -j$(sysctl -n hw.ncpu)
```

### 3. 运行程序

```bash
./NameRoller
```

> 💡 如果遇到动态库路径问题，可以设置环境变量：
> ```bash
> export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
> ```

---

## 🪟 Windows 编译指南

### 方法一：使用 Visual Studio（推荐）

#### 1. 安装工具

1. 下载并安装 [Visual Studio](https://visualstudio.microsoft.com/)（Community 版即可）
   - 安装时勾选 "使用 C++ 的桌面开发" 工作负载
2. 下载并安装 [CMake](https://cmake.org/download/)
3. 下载 [SFML SDK](https://www.sfml-dev.org/download.php)
   - 选择与你的 Visual Studio 版本匹配的版本
   - 解压到某个目录，例如 `C:\SFML`

#### 2. 配置环境变量

将 SFML 的 bin 目录添加到系统 PATH 环境变量：
```
C:\SFML\bin
```

#### 3. 使用 CMake GUI 配置

1. 打开 CMake GUI
2. "Where is the source code" 选择项目目录
3. "Where to build the binaries" 选择 build 目录（点击 Create Directory 创建）
4. 点击 "Configure"
5. 选择你的 Visual Studio 版本
6. 如果找不到 SFML，设置 `SFML_DIR` 变量为 `C:/SFML/lib/cmake/SFML`
7. 点击 "Generate" 生成 Visual Studio 项目

#### 4. 编译

1. 点击 "Open Project" 打开 Visual Studio
2. 在顶部工具栏选择 "Release" 配置
3. 右键点击 "NameRoller" 项目，选择 "生成"

#### 5. 运行

1. 编译完成后，可执行文件在 `build/Release/` 目录下
2. 确保 resources 目录与可执行文件在同一目录
3. 双击运行 `NameRoller.exe`

### 方法二：使用 MinGW

#### 1. 安装工具

1. 下载并安装 [MinGW-w64](https://www.mingw-w64.org/)
2. 下载并安装 [CMake](https://cmake.org/download/)
3. 下载 SFML 的 MinGW 版本 SDK

#### 2. 编译

```bash
# 进入项目目录
cd name-roller

# 创建构建目录
mkdir build
cd build

# 配置 CMake（指定 MinGW 生成器）
cmake -G "MinGW Makefiles" .. -DSFML_DIR="C:/SFML/lib/cmake/SFML"

# 编译
mingw32-make -j4
```

---

## 🎬 sfeMovie 安装（可选）

sfeMovie 是一个 SFML 的视频播放扩展库。如果安装了它，点名器可以直接播放 MP4 等视频文件作为开场动画。

如果不安装 sfeMovie，程序会自动使用帧序列动画方案，功能同样可用。

### 从源码编译 sfeMovie

#### Linux / macOS

```bash
# 克隆源码
git clone https://github.com/SFML/sfeMovie.git
cd sfeMovie

# 创建构建目录
mkdir build && cd build

# 配置和编译
cmake ..
make -j$(nproc)

# 安装
sudo make install
```

#### Windows

参考官方文档：https://github.com/SFML/sfeMovie

### 验证 sfeMovie 是否被识别

重新编译项目时，如果 CMake 输出中出现：
```
-- 找到sfeMovie库，启用完整视频播放功能
```
说明 sfeMovie 已成功安装并被识别。

如果输出：
```
-- 未找到sfeMovie库，将使用帧序列动画方案
```
说明没有安装 sfeMovie，程序会使用帧序列方案。

---

## ❓ 常见问题

### Q1: 编译时提示找不到 SFML

**A:** 确保 SFML 已正确安装，并且 CMake 能找到它。

可以手动指定 SFML 路径：
```bash
cmake .. -DSFML_DIR=/path/to/SFML/lib/cmake/SFML
```

### Q2: 运行时提示缺少动态库

**Linux:**
```bash
export LD_LIBRARY_PATH=/path/to/SFML/lib:$LD_LIBRARY_PATH
```

**Windows:**
确保 SFML 的 bin 目录在 PATH 环境变量中，或者将 SFML 的 .dll 文件复制到可执行文件同目录。

### Q3: 中文显示乱码

**A:** 确保使用了支持中文的字体文件。

1. 下载一个中文字体（如思源黑体）
2. 将 .ttf 文件放入 `resources/fonts/` 目录
3. 重新运行程序

### Q4: 开场视频不播放

**A:** 检查以下几点：

1. 视频文件是否在 `resources/video/intro.mp4`
2. 是否安装了 sfeMovie 库
3. 如果没有 sfeMovie，是否有帧序列在 `resources/video/frames/`
4. 查看控制台输出的日志信息

### Q5: 立绘图片不显示

**A:** 检查以下几点：

1. 图片文件是否在 `resources/characters/` 目录
2. 文件名（不含扩展名）是否与名字完全一致
3. 图片格式是否支持（PNG, JPG, BMP）
4. 图片文件是否损坏
5. 查看控制台输出的日志信息

### Q6: 如何调整窗口大小

**A:** 修改 `src/main.cpp` 中的窗口大小参数：

```cpp
const unsigned int windowWidth = 900;   // 修改这里
const unsigned int windowHeight = 700;  // 修改这里
```

然后重新编译。

### Q7: 如何调整名字滚动速度

**A:** 在 `NameRoller` 类中调用 `setRollSpeed()` 方法，或者修改默认值。

默认值在 `NameRoller.cpp` 构造函数中：
```cpp
m_rollSpeed(10.0f)  // 每秒10个名字
```

数值越大滚动越快。

---

## 📞 获取帮助

如果遇到其他问题：

1. 查看控制台输出的错误信息
2. 检查资源文件是否正确放置
3. 确认 SFML 版本是否为 2.5 或更高
4. 确认编译器支持 C++17

祝编译顺利！🎉
