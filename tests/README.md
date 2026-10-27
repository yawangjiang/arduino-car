# 主机侧控制逻辑测试

在仓库根目录运行 `python tools/test_host.py`，使用 g++ 编译并执行六个测试程序：寻光、PD 循迹、避障、环境与惯性联动、BME280 独立观察、MPU6050 独立观察。构建输出位于临时目录。

测试直接包含各自的真实 `.ino`，本目录的 Arduino、Servo、Wire 与传感器头文件只用于主机 API 桩；Arduino 目标编译使用真实库。这些桩不模拟实际声学、电气、传感器噪声或运动过程。

下方保留单独运行寻光测试的方式，其余用例覆盖和 UNO 编译见[验证说明](../docs/validation.md)。

测试直接包含 `firmware/light_seeking/light_seeking.ino`，使用本目录的 `Arduino.h` 替代实际硬件 API，输入模拟 ADC 读数并检查输出引脚与 PWM。

覆盖传感器偏差补偿、左右转向、正负死区边界、暗光阈值的严格比较、单侧遮挡、ADC 端点、PWM 限幅、初始化和串口输出。

在本项目目录使用支持 C++11 的 g++ 编译器执行：

```powershell
New-Item -ItemType Directory -Path .test-build -Force | Out-Null
g++ -std=c++11 -Wall -Wextra -Werror -I tests tests/light_seeking_test.cpp -o .test-build/light_seeking_test.exe
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed' }
& ./.test-build/light_seeking_test.exe
if ($LASTEXITCODE -ne 0) { throw 'Control tests failed' }
```

Linux / macOS 可使用相同 g++ 参数编译到自建的 `.test-build/` 目录，再执行生成的程序。

API 桩只能验证主机 C++ 编译与程序控制行为，不能验证 AVR 固件编译、PWM 电气输出、模拟输入噪声、电机极性或实车运动。Arduino 目标编译与实车步骤见[接线与标定](../docs/setup-and-calibration.md)。
