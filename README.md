# 实验一：矩阵键盘与串口通信的猜数字游戏

基于普中51开发板、STC89C52RC、11.0592MHz晶振的综合实验。最终版本同时通过 **LCD1602、八位数码管和UART串口** 输出，能够脱离电脑串口窗口进行游戏。

## 直接使用

- 最终工程：[`firmware/experiment1.uvproj`](firmware/experiment1.uvproj)。
- 最终下载文件：[`firmware/experiment1.hex`](firmware/experiment1.hex)。
- 操作说明：[`docs/显示版操作说明.md`](docs/显示版操作说明.md)。
- 下载工具：[`tools/PZ-ISP.exe`](tools/PZ-ISP.exe)，为用户提供的普中配套原始工具。

PZ-ISP选择 **STC89C5xx-RC Series、当前CH340串口（实测COM7）、9600、12T**，载入最终HEX，点击下载后将开发板电源关闭约2秒再打开。原有实测成功记录不代表每次连接的串口编号不变。

## 实验报告

- [实验一_矩阵键盘与串口通信的猜数字游戏_实验报告.docx](实验一_矩阵键盘与串口通信的猜数字游戏_实验报告.docx)
- [实验一_矩阵键盘与串口通信的猜数字游戏_实验报告.pdf](实验一_矩阵键盘与串口通信的猜数字游戏_实验报告.pdf)

## 在开发板上操作

1. 按 **K3** 开始，LCD显示 `IN:____ K4:OK`。
2. 矩阵键盘 **S1～S10对应0～9**，依次输入四位。S11～S16忽略。
3. 按 **K4** 确认。LCD显示 `LAST:0123`、`X:2 Y:1 NEXT:KEY` 一类信息；数码管显示 `0123-2-1`。
4. X是数字正确总数，Y是其中位置正确数。猜错后直接输入下一组；按K3可重新开局。
5. 猜对时显示 `ALL CORRECT!` 和 `PASS`，按K3开始新游戏。

K1/K2与串口P3.1/P3.0复用，故使用K3/K4。指导书硬件说明与功能步骤的按键命名存在不一致，本项目在说明中记录了适配。LCD1602采用英文提示，中文反馈保留在串口（9600、8N1、UTF-8）。

## 目录

|目录|内容|
|---|---|
|`firmware/`|最终LCD＋数码管版源码、Keil工程、HEX、构建脚本|
|`archive/serial_only/`|最初串口版，保留可复现基线|
|`docs/`|操作说明、实验原理、流程图、思考题及原版三轮实测摘录截图|
|`evidence/display/`|最终显示版的编译、下载与真实串口记录|
|`evidence/serial_only/`|原版三轮输入记录及验证记录|
|`evidence/*.jpg`|用户提供的开发板参考照片，拍摄于显示版烧录前|
|`references/manuals/`|指导书和普中开发攻略原PDF|
|`references/examples/`|开发过程中参照的普中原始示例ZIP|
|`tools/`|PZ-ISP、可选串口监视脚本与Python依赖|
|`tests/`|数字比较算法的独立频次法验证|

## 构建与验证

已安装Keil C51时，在PowerShell执行：

```powershell
.\firmware\build.ps1 -KeilC51Root 'D:/keil-C51/C51'
```

也可直接在Keil中打开工程。Keil编译器需自行安装，本项目不包含编译器。最终版本实际编译为 **0错误、0警告；code=2658字节，data=56.1字节**。

安装GCC后，可在仓库根目录运行已有算法测试：

```powershell
gcc tests/test_score.c firmware/game.c -o tests/test_score.exe
.\tests\test_score.exe
```

比较算法对8种目标各遍历0000～9999，共 **80000组**，与独立数字频次预期算法一致。

串口监视是可选功能，板载显示无需它：

```powershell
python -m pip install -r tools/requirements.txt
python tools/serial_monitor.py --port COM7
```

停止监视用Ctrl+C；不要让两个工具同时占用COM7。

## 实物验证与边界

- PZ-ISP已实际写入最终显示版，日志显示“文件下载完毕”，6.092s。
- 最终版输入0123后，真实串口返回“2个数字正确，1个位置正确”。用户确认LCD显示 `LAST:0123`、`X:2 Y:1 NEXT;KEY`，数码管显示 **0123-2-1**；三处输出一致。
- 原串口版另有0123、4567、8901三次独立开始后的猜测记录。三张截图是实际接收日志在本地查看器中的摘录展示，不是PZ-ISP窗口截图。
- 三次记录都是开始后的单次猜测，不是三次从开始到猜中通关。隐藏目标未公开，单凭这些日志不能逐位核验目标；算法另有80000组测试。
- “全部猜中”与再开始分支已实现，尚未实板触发验证。没有把烧录前的黑块屏照片当作显示成功照片。

## 参考资料与归属

详见 [`references/资料来源.md`](references/资料来源.md)。教材、原厂示例和PZ-ISP的权利属于原作者或提供方，随本实验资料一起归档，不对第三方文件另行授予许可。
