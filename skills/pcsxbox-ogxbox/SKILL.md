---
name: pcsxbox-ogxbox
description: 编译、实机调试与优化初代 Xbox（ogXbox）上的 PCSXBox PS1 模拟器。用于用老版 Microsoft XDK 构建 1.4/1.5/1.6 三核心 XBE、排查黑屏/死机/存档设置异常、加 CHD 之类新功能、以及做性能 A/B 与打包测试目录；不用于 RetroArch 主机移植，也不处理 ROM/BIOS 分发。
---

# PCSXBox on the Original Xbox

项目是 2009 年 XPort 系 PCSXBox 的重建快照：**一个前端 + 三套 PSX 核心**，用 VC7.1 编译，
必须用 **老版官方 XDK 5849.17**（`D:\Tools\ogxbox_legacy\XDK\xbox`），不是 RxDK。
产物是三个 XBE，运行时互相 chain-load。

本地工程：`/mnt/e/Projects/PlayStation 1 - PCSXBox`（**不是 git 仓库**）；
已归档副本：`https://github.com/liujiny/pcsxbox-ogxbox`（`Common/` + `Projects/` 镜像 `E:\` 布局）。

先确认这次要做的是哪类工作，再读对应参考。**改动前先备份现有 XBE 和 `E:\SAVES\PCSXBOX`。**

- 编译、工具链、三个核心怎么出、产物怎么命名 → [构建](references/build.md)
- 搞清楚哪个目录属于哪套版本、XBE 跳转逻辑 → [源码结构](references/source-tree.md)
- 黑屏/死机/画面异常/存档被污染 → [调试](references/debugging.md)
- 提升帧率、做 A/B、profiler 读数怎么解释 → [性能优化](references/optimization.md)
- 每游戏设置（`.stg`）、R3 菜单、frameskip 在哪 → [配置与测试](references/config-files.md)

## 工作原则

三核心共用同一个前端 `pcsxbox.cpp`，靠 `CORE=` / 宏切换。改前端或 `src\psxcounters.c`
一类共享文件时，**三份 core 目录必须同步**（`src`、`src\good`、`src\1.6`），否则就是
"改了一个 XBE，实际跑的是另一个"。

**绝对不要混用不同构建的 XBE，也不要用别的构建写过的 `.stg`。** 历史黑屏的原因就是
跨构建混用导致 `PCSXBOX_LAUNCH_DATA` 字段错位 + 旧 `.stg` 被按新结构 `fread`。
换核心时必须三份整体替换，并清一次 `E:\SAVES\PCSXBOX`。

验证状态要分清：`用户实机确认` / `仅编译成功` / `失败已回退` / `待测试`。任何优化结论
都要有实机同场景的 FPS 或 profiler 数字支撑，不能只看编译通过。

不要往技能里放 ROM、BIOS、SDK 二进制、`.xbe` 或大型日志。
