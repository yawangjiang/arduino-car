# 硬件连接与构建

各草图分别运行，不能直接把接线表合并。下表的左右均按小车前进方向确定；更换接线后先悬空检查电机方向。

## 整车程序引脚

| 程序 | ENA / ENB | IN1 / IN2 / IN3 / IN4 | 传感器 |
| --- | --- | --- | --- |
| 寻光 | D9 / D10 | D6 / D7 / D4 / D5 | 右 A0、左 A1 |
| PD 循迹 | D9 / D10 | D6 / D7 / D4 / D5 | ADS1115 AIN0/1/2 对应左/中/右；A4/SDA、A5/SCL |
| 超声波避障 | D5 / D6 | D11 / D10 / D8 / D9 | Trig D12、Echo D13、Servo D3 |
| 环境与惯性联动 | D9 / D10 | D6 / D7 / D4 / D5 | BME280 与 MPU6050 共用 A4/SDA、A5/SCL |

寻光、PD 与组合草图的逻辑左轮使用 ENA；避障附录中的逻辑左轮使用 ENB，前进电平也不同。基础循迹实验同样采用 ENB 对应左轮，但引脚和传感器接法另见[实验目录](../experiments/README.md)。

L298N 的 ENA/ENB 应接受 PWM，不能同时被使能跳线强制拉高。电机从匹配的电源与驱动器供电，不由 UNO I/O 供电，所有信号共地。电池电压与模块稳压跳线按实际模块标识确认，报告不足以指定具体配置。

## I2C 观察仪

| 外设 | 默认地址 | UNO 信号线 | 使用场景 |
| --- | --- | --- | --- |
| BME280 | `0x76` | A4/SDA、A5/SCL | 环境观察仪、组合草图 |
| MPU6050 | `0x68` | A4/SDA、A5/SCL | 六轴观察仪、组合草图 |
| ADS1115 | `0x48` | A4/SDA、A5/SCL | PD 循迹 |

先用 [I2C 扫描](../experiments/i2c/i2c_scan/i2c_scan.ino)确认地址。BME280 的另一常见地址为 `0x77`，MPU6050 的 AD0 设置可得到 `0x69`；应修改对应草图后重编译。模块供电、电平转换及上拉按实物额定要求连接，不能把裸传感器的电压要求与带转换的模块混用。

独立 BME280/MPU6050 观察仪不需要连接 L298N。组合草图已移除原附录中未使用的 ADS1115，因此不要求它在线。

## 依赖与串口

| 草图 | Arduino 库 | 波特率 |
| --- | --- | --- |
| `light_seeking` | 核心即可 | 9600 |
| `line_following_pd` | Adafruit ADS1X15 | 9600 |
| `obstacle_avoidance` | Servo | 115200 |
| `bme280_monitor` | Adafruit BME280 Library | 115200 |
| `mpu6050_monitor` | Adafruit MPU6050、Adafruit Unified Sensor | 115200 |
| `environment_imu` | Adafruit BME280 Library、Adafruit MPU6050、Adafruit Unified Sensor | 9600 |
| `experiments/` 下七个草图 | Arduino 核心，I2C 扫描使用核心 Wire | 见实验索引 |

库管理器还会按依赖安装 Adafruit BusIO 等库。每个文件夹只包含其自身的 `.ino`，在 IDE 中独立打开，避免多个 `setup()`/`loop()` 被编入同一个草图。

## 命令行构建

从 [Arduino 官方安装页](https://docs.arduino.cc/arduino-cli/installation)安装 Arduino CLI 后，在仓库根目录执行。仓库工具需要 Python 3.9 或以上，主机测试还需要支持 C++11 的 g++。下列版本对应本次成功编译的组合：

```sh
arduino-cli core update-index
arduino-cli core install arduino:avr@1.8.8
arduino-cli lib install "Adafruit ADS1X15@2.6.2" "Adafruit MPU6050@2.2.9" "Adafruit BME280 Library@2.3.0" "Servo@1.3.0"
python tools/compile_all.py
```

`compile_all.py` 编译 `firmware/` 与 `experiments/` 中全部 13 个草图，输出放在临时目录，不加入仓库。支持 `--cli` 指定可执行文件、`--config` 指定隔离配置。

只编译一个例子：

```sh
arduino-cli compile --fqbn arduino:avr:uno firmware/obstacle_avoidance
```

烧录应在确认实际接线和端口后执行。本次验证只编译，没有对连接设备烧录。

## 复测顺序

先验证独立采样与 I2C 地址，再悬空验证电机方向和动作，之后测试阈值及整车运动。对避障应包含无回波与侧向测距失败，对循迹应包含 `000` 丢线和 `011` 恢复，对惯性联动应记录原始 Z 轴在两个阈值附近的状态保持。

[返回项目首页](../README.md)
