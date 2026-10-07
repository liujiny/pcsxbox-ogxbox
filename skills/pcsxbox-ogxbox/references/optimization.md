# 性能优化

基线是 `Release/PCSXBox_chd1`（只有 CHD，没有性能改动），已实机验收通过。
分析见工程根目录 `ANALYSIS_OPT1.md` / `ANALYSIS_OPT2.md`。

## 每帧结构（先知道时间花在哪）

* `src/win32/wndmain.c` 的 `while (Running)` 连续 140 次 `psxCpu->Execute()`；
  一次 `Execute()` 是一个 PSX 基本块（`src/ix86/ir3000a.c` 的 456-482 行），块间直接跳。
* 所有 vblank 工作都在 `src/psxcounters.c` 的 VSync End 分支：
  `GPU_updateLace()` → `updateDisplay()` → `xbox_put_image()` →
  `pcsxbox.cpp::render_to_texture()` → `Present()`。**Present 的 vsync 等待也在这条链里**。
* 每帧 = dynarec + 软件光栅 + 贴图上传 + Present 等待，四段互不重叠，可分别测量。
  FPS 上限恒为 60；三项之和贴着 16.67ms 就说明已撞 vsync 上限。

三份 core 的对应文件：`src/psxcounters.c`、`src/good/psxcounters.c`、`src/1.6/PsxCounters.c`
（注意大小写）；`src/ix86/ir3000a.c`、`src/ix86/good/ir3000a.c`、`src/1.6/ix86/iR3000A.c`。

## 已落地的两个改动

**A — 软件光栅器强制内联**（`src/gpu/src/soft.c`）
`soft.c` 自己写了 `#define __inline _inline`，在 VC7.1 下等于**从不内联**那 13 个逐像素
辅助函数（`Blit8`、`Blit16` …）。改成 `__forceinline`。对 2D 密集画面收益最大。

**B — dynarec 块出口内联**（三个 `ir3000a.c`）
基本块出口原来每个分支都 `CALL psxBranchTest`；现在把 `psxRegs.cycle` /
`psxNextsCounter` / `psxNextCounter` 比较和 `psxRegs.interrupt` 快路径内联，
只有命中才走 C 调用。逃生开关：`EXTRA_DEFINE=PCSXBOX_NO_FAST_BRANCHTEST`。

## 测试包关系（按这个顺序 A/B）

| 包 | 内容 |
|---|---|
| `Release/PCSXBox_chd1` | 对照基线（CONTROL），只有 CHD |
| `Release/PCSXBox_optA_softinline` | 基线 + 改动 A |
| `Release/PCSXBox_optB_branchtest` | 改动 A + 改动 B |
| `Release/PCSXBox_optB_prof` | 同 optB，加 cpu/rast/blit overlay（**诊断用，不发布**） |

差值含义：`chd1 → optA` = A 的净收益；`optA → optB` = B 的净收益。

## 怎么做测量

同一台机器、同一个游戏、同一存盘/同一关，挑**屏幕上元素最密集**的一段，每种构建
各停 30 秒。用 `optB_prof` 读 `cpu= / rast= / blit=`，然后：

* `cpu` 最大 → 继续做 dynarec；
* `rast` 最大 → 继续做 `src/gpu/src/soft.c` 精灵内层循环；
* `blit` 最大 → 做显示路径（删冗余 Clear / 贴图直写）。

## 已排除的误区（别再试）

* **`/G6` 是空操作**：VC7.1 里 `/O2` 与 `/O2 /G6` 目标文件逐字节相同（已实测）。
* **`/O2` 已隐含 `/Oi /Ob2 /Ot /Oy /Gs /GF /Gy`**；旧 build.sh 的 `/O2 /Ob1 /Os`
  是反向的，已修正为 `/O2 /Ob2 /Ot /Og`。
* **`iFlushRegs()` 不是开销点**，它几乎不生成指令。
* `memset(g_pDeltaBuff, 0xFF, m_pitch*512)`（1MB）只在 init / 换滤镜时发生，不是每帧。
* `BlitScreen16` 的 bit-shuffle 对 2D 游戏不是瓶颈（320x240 约 0.1-0.2ms）。

## 还没做、风险收益对照

低风险：`LockRect` 只锁实际 `w x h`（opt2 已放进代码，但**可能收益为零**，有回退分支）;
关混响（菜单已有开关，2D 游戏通常不需要，不建议改默认值）；`CD_BUF_SECTORS` 10 → 32/64
（省盘停顿，多占约 52KB）；FPS 字符串不要每帧 `sprintf`。
输入轮询节流（`XBInput_GetInput` 每帧做 2~4 次 `XGetDeviceChanges`）属于低风险可能可观，
但会改共享文件 `E:\Projects\common`。

**不要碰**：dynarec 跨块寄存器分配、块链接重写、光栅器重写 —— 没有逐游戏回归手段时
风险远大于收益。

## 硬规则

优化必须以 **core 为边界**：改 `pcsxbox.cpp` / `psxcounters.c` 一类共享文件时，
三份 core 目录同步改，否则测出来的差异不是你以为的那个改动。
