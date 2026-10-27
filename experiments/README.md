# 基础实验与调试工具

每个目录是独立草图，按硬件主题分类。电机与基础循迹来自实验报告，超声波单测从期末函数拆出，I2C 扫描为整理时新增工具。

| 类别 | 草图 | 来源与作用 | 串口 |
| --- | --- | --- | --- |
| LDR | [ldr_readings](ldr/ldr_readings/ldr_readings.ino) | Lab 1，A0 采样，`Y` 开始、`N` 停止，每次采样间隔 3 s | 9600 |
| 电机 | [four_wire_speed_test](motor/four_wire_speed_test/four_wire_speed_test.ino) | Lab 2，D2-D5 顺序软件 PWM，245/255 设置 | 无 |
| 电机 | [six_wire_speed_test](motor/six_wire_speed_test/six_wire_speed_test.ino) | Lab 2，方向 D2-D5、使能 D9/D10，249/255 设置 | 无 |
| 红外 | [ir_sensor_readings](infrared/ir_sensor_readings/ir_sensor_readings.ino) | Lab 3，左 A2、中 A1、右 A0 | 9600 |
| 基础循迹 | [basic_line_following](line_following/basic_line_following/basic_line_following.ino) | Lab 4，左 A5、中 A4、右 A3，阈值 500、PWM 上限 180 | 9600，无遥测打印 |
| 超声波 | [ultrasonic_distance](ultrasonic/ultrasonic_distance/ultrasonic_distance.ino) | 期末 3.3 拆分，Trig D12、Echo D13，输出距离及有效性 | 115200 |
| I2C | [i2c_scan](i2c/i2c_scan/i2c_scan.ino) | 新增诊断，A4/SDA、A5/SCL，列出应答地址 | 115200 |

## 复现要点

Lab 1 的 D4 是原程序控制输出，报告未说明外接负载；仅采样 A0 时可不连接 D4。3 s 阻塞等待期间不会立即响应停止命令。

四线软件 PWM 对两个电机顺序输出，不能把结果简单归结为线数差异。比较时应保存真实 PWM、负载、地面、电源与计时条件。

基础循迹保留 Lab 4 映射：D9/ENA 接逻辑右轮，D10/ENB 接逻辑左轮；与期末 PD 版不同。它占用 A4/A5，不能直接和 I2C 模块同时按该接线运行。被裁切的 `setMotor()` 声明已补全，右轮方向使用传入参数。

三个 IR 共有八种二进制组合，报告主要测试七种；基础和期末循迹均对 `101` 保留前次偏差。黑白读数方向应实测确认。

[数据](../data/README.md) · [硬件与构建](../docs/hardware-and-build.md) · [返回首页](../README.md)
