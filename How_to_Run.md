# PlatformIO ESP32项目设置指南

本文档详细介绍如何使用PlatformIO设置、编译和烧写ESP32项目。基于您的NaviControl-ESP32项目，我们将从零开始逐步完成所有设置。

## 前提条件

- **操作系统**: macOS (其他系统类似)
- **Python**: 3.6或更高版本 (我们使用3.13.3)
- **ESP32开发板**: 通过USB连接到电脑
- **USB驱动**: macOS通常内置，无需额外安装

## 步骤1: 安装Python和虚拟环境

首先确保Python可用：

```bash
python3 --version
```

如果没有Python，请先安装Python 3。

## 步骤2: 配置Python虚拟环境

为项目创建独立的Python环境：

```bash
cd <你的项目路径>
python3 -m venv .venv
```

激活虚拟环境：

```bash
source .venv/bin/activate
```

## 步骤3: 安装PlatformIO

在虚拟环境中安装PlatformIO：

```bash
pip install platformio
```

验证安装：

```bash
pio --version
```

## 步骤4: 项目结构设置

PlatformIO期望源代码位于src目录中。确保您的项目结构如下：

```
NaviControl-ESP32/
├── platformio.ini
├── src/
│   └── main.ino
└── ...其他文件
```

如果src目录不存在，创建它：

```bash
mkdir src
```

将您的主文件移动到src目录：

```bash
mv main.ino src/
```

## 步骤5: 配置platformio.ini

确保您的platformio.ini文件包含以下内容：

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
```

## 步骤6: 编译项目

激活虚拟环境并编译项目：

```bash
cd /Users/esap/Desktop/Github/NaviControl-ESP32
source .venv/bin/activate
pio run
```

编译成功后，您会看到类似以下输出：

```
===================== [SUCCESS] Took 9.46 seconds =====================
```

## 步骤7: 烧写固件到ESP32

确保ESP32板通过USB连接到电脑，然后运行烧写命令：

```bash
cd /Users/esap/Desktop/Github/NaviControl-ESP32
source .venv/bin/activate
pio run --target upload
```

烧写过程会显示进度：

```
Uploading .pio/build/esp32dev/firmware.bin
esptool.py v4.9.0
Serial port /dev/cu.usbserial-14220
Connecting..........
...
===================== [SUCCESS] Took 28.43 seconds =====================
```

## 步骤8: 验证烧写结果

烧写完成后，ESP32会自动重启。您可以通过以下方式验证：

1. **观察LED**: 如果您的代码包含LED闪烁，板载LED会开始闪烁
2. **串口监控**: 查看串口输出

```bash
pio device monitor
```

## 常用命令

### 编译和烧写
```bash
pio run                    # 仅编译
pio run --target upload    # 编译并烧写
```

### 串口监控
```bash
pio device monitor
```

### 清理构建文件
```bash
pio run --target clean
```

### 查看可用板子
```bash
pio boards espressif32
```

## 故障排除

### 1. 找不到pio命令
确保激活了虚拟环境：
```bash
source .venv/bin/activate
```

### 2. 编译失败
检查src目录中是否有有效的源文件（.ino, .cpp, .c等）

### 3. 烧写失败
- 检查ESP32是否正确连接
- 确保没有其他程序占用串口
- 在macOS上，串口通常是`/dev/cu.usbserial-*`

### 4. 端口问题
如果自动检测的端口不正确，可以在platformio.ini中指定：

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_port = /dev/cu.usbserial-14220
```

## 项目文件说明

- **platformio.ini**: 项目配置文件，定义平台、板子和框架
- **main.ino**: 主程序文件（Arduino格式）
- **.pio**: PlatformIO生成的构建文件目录
- **.venv**: Python虚拟环境目录

## 下一步

现在您的ESP32项目已经设置完成！您可以：

1. 修改main.ino中的代码
2. 添加更多源文件到src目录
3. 在platformio.ini中添加库依赖
4. 集成传感器、显示屏等硬件

如果遇到任何问题，请检查PlatformIO的官方文档：https://docs.platformio.org/