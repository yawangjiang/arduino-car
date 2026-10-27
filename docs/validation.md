# 构建与验证记录

2026 年 10 月 27 日，全部 13 个独立草图通过 `arduino:avr:uno` 目标编译。编译使用真实 UNO 核心和第三方库，主机控制测试另用 API 桩验证状态与边界。

## 编译环境

| 工具或库 | 版本 |
| --- | --- |
| Arduino CLI | 1.5.2-rc.1 |
| Arduino AVR Boards | 1.8.8 |
| avr-gcc | 7.3.0-atmel3.6.1-arduino7 |
| Adafruit ADS1X15 | 2.6.2 |
| Adafruit BME280 Library | 2.3.0 |
| Adafruit MPU6050 | 2.2.9 |
| Adafruit Unified Sensor | 1.1.15 |
| Adafruit BusIO | 1.17.4 |
| Servo | 1.3.0 |

以 `--warnings all` 编译；当前 AVR 核心 `new.cpp` 有未使用参数警告，编译成功不代表第三方代码零警告。构建产物与下载的工具、库没有加入仓库。

## 覆盖范围

| 验证 | 范围 |
| --- | --- |
| UNO 目标编译 | 六个应用、七个基础/诊断草图全部通过 |
| 寻光控制 | 原 20 个控制用例、初始化和串口输出 |
| PD 循迹 | ADC 初始化失败、黑白阈值、转向限幅、20 ms 后退、`011` 恢复和 130 ms 过渡 |
| 避障状态机 | 启动停车、双向扫描、阈值停车、左右观察、后退时序、随机分支、恢复、无回波与测距失败 |
| 惯性联动 | -8/-15 严格边界、区间状态保持、初始化/读取失败、非有限读数停车和压力单位 |
| 独立观察仪 | 周期、CSV 字段、模长、压力换算和无效读数 |
| 文档与数据 | 本地链接、代码围栏、图片哈希、草图名称、数据数量与范围 |

主机 API 桩不模拟电机惯性、声学反射、真实 I2C 失败时序或传感器噪声。AVR 编译不等于烧录与运动验证。本次没有连接设备烧录，也没有对重建或修正的程序进行实车复测。

## 重现命令

```sh
python tools/compile_all.py
python tools/test_host.py
python tools/check_project.py
```

依赖安装见[构建说明](hardware-and-build.md)，测试细节见[测试目录](../tests/README.md)。

[返回项目首页](../README.md)
