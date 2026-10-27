# Arduino 多功能小车

基于 Arduino UNO 的 Bus 造型双轮差速小车，记录从传感器采样、电机测试到寻光、循迹、避障和环境观测的课程设计过程。

**Yuanxi HAN** · Arduino UNO R3 · L298N · LDR / TCRT5000 / HC-SR04 / MPU6050 / BME280

![Bus 造型小车实物](media/light-seeking/car-side.jpg)

## 按功能浏览

| 分类 | 实现 | 文档 | 程序 |
| --- | --- | --- | --- |
| 光照感知与运动 | 双 LDR 寻光、偏差补偿、暗光停车 | [寻光小车](docs/light-seeking.md) | [light_seeking](firmware/light_seeking/light_seeking.ino) |
| 红外循迹 | 三路 TCRT5000、ADS1115、PD 差速与丢线恢复 | [循迹控制](docs/line-following.md) | [line_following_pd](firmware/line_following_pd/line_following_pd.ino) |
| 超声波避障 | HC-SR04 测距、SG90 扫描、退让与转向状态机 | [超声波避障](docs/obstacle-avoidance.md) | [obstacle_avoidance](firmware/obstacle_avoidance/obstacle_avoidance.ino) |
| 环境观测 | BME280 温度、相对湿度与气压，CSV 串口输出 | [温湿度气压观察仪](docs/environment-monitoring.md) | [bme280_monitor](firmware/environment/bme280_monitor/bme280_monitor.ino) |
| 惯性观测 | MPU6050 三轴加速度、三轴角速度与加速度模长 | [陀螺仪与加速度观测](docs/inertial-monitoring.md) | [mpu6050_monitor](firmware/inertial/mpu6050_monitor/mpu6050_monitor.ino) |
| 环境与运动联动 | 课程 Z 轴阈值启停逻辑，结合 BME280 数据 | [环境与惯性联动](docs/environment-imu.md) | [environment_imu](firmware/environment_imu/environment_imu.ino) |

每个草图单独编译、单独烧录。两个独立观察仪从报告的组合程序拆出，可单独采样而不驱动电机。整车程序沿用各自引脚，切换功能前按[接线总表](docs/hardware-and-build.md)核对，当前没有同时运行全部功能的统一固件。

## 演示

| 内容 | 报告提供的视频 |
| --- | --- |
| 寻光 | [Bilibili 演示](https://www.bilibili.com/video/BV1PkKU6hEKu/) |
| 黑线循迹 | [Bilibili 演示](https://www.bilibili.com/video/BV1pkKU68E97/) |
| 超声波避障 | [Bilibili 演示](https://www.bilibili.com/video/BV1jdKU6DEbN/) |
| 传感器与运动状态测试 | [Bilibili 演示](https://www.bilibili.com/video/BV1pCKU6fEtG/) |

视频对应课程阶段的实测版本。整理时的代码恢复、故障处理与独立拆分见[来源与修改记录](docs/module-source-notes.md)，这些改动尚未进行实车复测。

## 从基础实验到整车功能

```mermaid
flowchart LR
    A[LDR 特性与双路标定] --> B[寻光控制]
    C[四线 / 六线电机驱动] --> D[双轮差速与补偿]
    E[红外高度响应与三路采样] --> F[基础循迹]
    F --> G[ADS1115 与 PD 循迹]
    H[超声波单独测距] --> I[舵机扫描避障]
    J[BME280 环境观测] --> K[环境与惯性联动]
    L[MPU6050 六轴观测] --> K
    D --> B
    D --> G
    D --> I
```

[基础实验目录](experiments/README.md)保留电机测速、LDR 采样、红外采样、基础循迹、超声波测距和 I2C 诊断，共七个独立草图。[数据目录](data/README.md)收录可追溯的 LDR 原始读数、红外高度测试与电机计时数据。

## 快速开始

1. 在功能表中选择程序，阅读对应文档和接线表。
2. 安装 Arduino AVR Boards 平台及该程序需要的库，选择 Arduino UNO。
3. 在 Arduino IDE 中打开与文件夹同名的 `.ino`，编译并上传。
4. 按文档设置串口波特率；涉及运动的程序先悬空确认左右轮和前进方向，再落地标定。

[硬件与构建说明](docs/hardware-and-build.md)提供引脚、依赖、串口设置和批量编译命令。

## 验证与资料

全部 **13 个草图通过 UNO AVR 目标编译**。控制回归、版本与验证范围见[验证说明](docs/validation.md)。可运行[编译工具](tools/compile_all.py)、[主机测试](tests/README.md)和[文档与数据检查](tools/check_project.py)复查。

完整实验报告未上传。代码和图片以原报告为依据，正文与附录冲突时说明实际代码行为；恢复被裁切的内容和必要修正逐项注明。没有原始记录的平滑曲线只用于解释，不据此生成测量数据。

## 仓库结构

```text
arduino-car/
├── firmware/              # 六个独立应用草图
│   ├── light_seeking/
│   ├── line_following_pd/
│   ├── obstacle_avoidance/
│   ├── environment/bme280_monitor/
│   ├── inertial/mpu6050_monitor/
│   └── environment_imu/
├── experiments/           # 七个基础实验与诊断草图
├── docs/                  # 原理、接线、标定、来源与验证
├── media/                 # 按功能分类的报告原图
├── data/                  # 原始实验读数与计时数据
├── tools/                 # 批量编译与完整性检查
└── tests/                 # 主机侧控制回归测试
```
