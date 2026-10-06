# PCSXBox OG Xbox -- 性能分析 Round 2 (源码级)

基线: `Release/PCSXBox_baseline3_final` (default.xbe 8F657CD9...) -- 实机已验收通过
(Captain Commando.bin: 画面/声音/操作正常, R3 菜单 + 缩略图 + 返回 + Exit Game 全过)。

本轮新增两个包:
- `Release/PCSXBox_opt2`      -- 优化改动, 无 profiler
- `Release/PCSXBox_opt2_prof` -- 同上 + `PCSXBOX_PROFILE` 计时overlay

---

## 1. 每帧结构 (已用源码证明, 不是猜测)

`src/win32/wndmain.c:119-379` 主循环体**只有 240 个 `psxCpu->Execute()`**
(139..378 行, 除了开头 121-137 的 schedulestate 判断, 没有任何其它语句)。
所以帧节奏完全来自:

```
PSX 块执行 -> psxRegs.cycle 跨过 vblank -> psxBranchTest -> psxRcntUpdate
  -> src/psxcounters.c:141  GPU_updateLace()
  -> src/gpu/src/gpu.c:942  GPUupdateLace
  -> src/gpu/src/gpu.c:680  updateDisplay
  -> src/gpu/src/draw.c:5670 xbox_put_image
  -> pcsxbox.cpp:4518       render_to_texture
  -> pcsxbox.cpp:4762       Present()   <-- vsync 等待在这里
```

`m_d3dpp` 由 `../../Common/Src/xbapp.cpp:57` `ZeroMemory` 初始化,
`FullScreen_PresentationInterval` 从未被赋值 => =0 = `D3DPRESENT_INTERVAL_DEFAULT`
=> **Present 每帧阻塞等待 vblank**。这就是 60fps 的来源, 也是为什么
"cpu+rast+blit" 只要 < 16.67ms 就永远是 60fps。

每帧时间 = CPU模拟 + 软件光栅 + 贴图上传 + Present等待, 四段不重叠, 可分别测量。

---

## 2. 每帧固定开销点 (逐条已定位)

### 2.1 dynarec 块出口 -- 单点收益最大

`src/ix86/ir3000a.c:255-270` (以及 169 / 200 的等价路径):

```c
iFlushRegs();                                  // 见下, 实际几乎不生成指令
MOV32ItoM((u32)&psxRegs.pc, branchPC);
CALLFunc((u32)psxBranchTest);                  // <<<< 每个基本块一次 C 调用
count = (pc - pcold)/4;
ADD32ItoM((u32)&psxRegs.cycle, count);
CMP32ItoM((u32)&psxRegs.pc, branchPC);  JE8 -> RET
MOV32MtoR(EAX, PC_REC(branchPC));  TEST; JNE8 -> RET
JMP32R(EAX);                                   // 链到下一块
```

`iFlushRegs()` (`ir3000a.c:90`) 循环 32 个寄存器, 但 `iFlushReg` (`ir3000a.c:83`)
只在 `IsConst(reg)`(编译期常量) 时才发射 store; 因为 `Load()` 在本版本里根本没实现
(寄存器从不映射到 x86 寄存器), 所以**生成的写回指令数通常是 0**。也就是说块出口的
开销几乎全部是 `CALLFunc(psxBranchTest)` 本身。

`psxBranchTest` 本体 (`src/r3000a.c:100`):

```c
if ((psxRegs.cycle - psxNextsCounter) >= psxNextCounter) psxRcntUpdate();
if (psxRegs.interrupt) { ... 5 个位测试 ... }
if (psxHu32(0x1070) & psxHu32(0x1074)) { if ((Status & 0x401)==0x401) psxException(...); }
```

在 Xbox 上 `SWAP32` 是恒等 (`src/psxmem.h:41`), 所以这就是
"2 次内存比较 + 3 次内存读 + 若干分支 + call/ret"。一个典型 PSX 基本块本身
可能只有 50~150 个 x86 周期, 因此这一次调用+返回 + 函数体大约占 **20%~40%**。

**做法**: 在生成代码里内联快路径, 只有命中才 CALL:

```
mov  eax, [psxRegs.cycle]
sub  eax, [psxNextsCounter]      ; SUB32MtoR  (ix86.h:222, 0x2B = sub r32,[mem])
cmp  eax, [psxNextCounter]       ; CMP32MtoR  (ix86.h:412, 0x3B = cmp r32,[mem])
JGE8 -> slow                     ; ix86.h:341
mov  eax, [psxRegs.interrupt]    ; TEST32RtoR / JNE8 -> slow
mov  eax, [psxH+0x1070]
and  eax, [psxH+0x1074]          ; JNE8 -> slow
... 继续链
slow: CALLFunc(psxBranchTest)
```

可用 emitter 已核对: `SUB32MtoR` / `CMP32MtoR` / `OR32MtoR` / `TEST32ItoR` /
`MOV32MtoR` 都在; **`JAE8`/`JB8` 不存在**, 用 `JGE8`(0x7D, 有符号) 即可,
因为这里比较的两个量差值很小, 有符号/无符号等价。
预期收益 5%~15% (CPU 重的 3D 游戏更高)。风险中等, 必须逐游戏回归。

### 2.2 显示/上传路径 (`pcsxbox.cpp:4518 render_to_texture`)

| 行 | 内容 | 评价 |
|---|---|---|
| 4594 / 4613 | `BlitScreen(pBits|g_pBlitBuff, blitx, blity)` | 见 2.3 |
| 4598 / 4616 | `Clear(..., D3DCLEAR_TARGET, 黑)` | **整屏 640x480 清屏, 随后立刻被覆盖** |
| 4599 / 4617 | `BeginScene()` | |
| 4693 | `m_pnlGameScreen.Render(..., 0,0,640,480)` | 全屏 quad, 覆盖整个 backbuffer |
| 4698 | `if (g_bShowFPS) sprintf(...)` | 每帧一次 sprintf + swprintf |
| 4750 | `EndScene()` | |
| 4762 | `Present()` | vsync 等待 |

因为 4693 的目标矩形就是 (0,0)-(640,480) 全屏, 4598/4616 的 `Clear` 是纯浪费。
Xbox 上整屏 fill 大约 0.1~0.3ms, 属于"零风险小收益", 但前提是确认
`m_nScreenMaxX/Y` 恒为 640x480 (现在 `InitializeEmuSpecific` 已经保证默认值)。

### 2.3 贴图上传 (`src/gpu/src/draw.c:2014 BlitScreen16`)

16bit 主路径每次处理 2 个像素 (一个 32-bit store):

```c
lu = *SRCPtr++;
*DSTPtr++ = ((lu<<11)&0xf800f800)|((lu<<1)&0x7e007e0)|((lu>>10)&0x1f001f);
```

320x240 = 38400 次迭代, 纯 bit-shuffle, 在 733MHz P3 上大约 0.1~0.2ms。
**对 2D 游戏这不是瓶颈**, 所以先别做 MMX/SSE 版本 —— 除非 profiler 显示
`blit` 占大头。

### 2.4 纹理 Lock (本轮 opt2 已改)

`init_texture()` 里原来是整面 1024x512 的 `LockRect` 量出 `m_pitch = 2048`
(`pcsxbox.cpp:1273`)。opt2 改成只锁 `w x h`, 并在 `d3dlr.Pitch != m_pitch` 时
**退回整面锁** (`pcsxbox.cpp:4549-4556`)。

必须诚实说明: 这个改动**可能收益为零**。
- `D3DFMT_LIN_R5G6B5` 是线性格式, 驱动大概率原地返回同一块内存, 则 Pitch 仍是 2048,
  省下来的是"驱动要搬运的字节数", 不是 Lock 调用本身的开销;
- 如果驱动为子矩形返回了不同的 Pitch, 代码直接回退, 等于没改。
它的真正价值是**为后续替换成 swizzle/DirectX 直写路径做准备**, 且它是安全的
(有回退分支)。是否需要保留, 由 opt2 实机 A/B 决定。

### 2.5 输入轮询 (`../../Common/Src/xbinput.cpp:91`)

`XBInput_GetInput()` **每次调用都做一次 `XGetDeviceChanges(GAMEPAD,...)`**
(内核调用, 枚举设备)。而它每帧至少被调 2 次:

- `src/gpu/src/gpu.c:993` `GPUupdateLace` 结尾 `xbox_read_input(0)`
- `src/PadSSSPSX.c:531/536` 游戏轮询手柄时 `xbox_read_input(0/1)`
  -> `pcsxbox.cpp:6684` -> `ReadJoypad` -> `pollXBoxControllers`
  (`../common/commonfuncs.cpp:2894`) -> `XBInput_GetInput`

热插拔检测只需要 ~2Hz, 现在却是每帧 2~4 次。把 `XGetDeviceChanges` 节流到
每 30 帧一次是**低风险、可能可观**的收益 (取决于该内核调用实际耗时, 需要
profiler 里加一个计数点来确认)。注意这会修改 `E:\Projects\common` 下的共享文件。

### 2.6 SPU / CD

- 混响默认开: `pcsxbox.cpp:1106 m_psxfix_spu_reverb = 2`。菜单里有 0/1/2 三档,
  2D 游戏关掉可省 SPU CPU。属于"用户可以自己选", 不建议改默认值。
- `CD_BUF_SECTORS 10` (`pcsxbox.cpp:173`): 每次 cache miss 读 2352*10=23.5KB。
  调到 32 会让内存多占 ~52KB, 对 HDD 路径收益小, 对真实光驱/网络路径收益明显。

---

## 3. 已经排除的误区 (不要再试)

- **`/G6` 不是优化**: VC7.1 里 `/O2` 与 `/O2 /G6` 生成的目标文件逐字节相同 (已实测)。
- **`/O2` 已隐含 `/Oi /Ob2 /Ot /Oy /Gs /GF /Gy`**。旧 build.sh 用 `/O2 /Ob1 /Os`
  与其说是"优化"不如说是"反向", Round 1 已修正为 `/O2 /Ob2 /Ot /Og`。
- **黑屏不是渲染 API 的问题**: 强制 `Sprite->Draw`、强制 640x480、强制 200x150、
  去掉 BeginScene/EndScene、BlockUntilNotBusy、禁用 ClearScreen 全都无效
  (见 Round 1 记录)。根因是 screen rect 被 launch path / 旧 .stg 写成 0。
  `InitializeEmuSpecific` 的默认值已经把这个坑堵上。
- **`iFlushRegs()` 不是开销点**: 它几乎不生成指令 (见 2.1)。
- **`memset(g_pDeltaBuff, 0xFF, m_pitch*512)` (1MB)** 只在 init / 换滤镜时发生,
  **不是**每帧。

---

## 4. 建议的优化顺序 (要按 profiler 数据走, 不要盲改)

1. **先量**: 用 `Release/PCSXBox_opt2_prof` 跑 Captain Commando, 记下屏幕上的
   `cpu= / rast= / blit= ms/frame`。
   - 三项之和 ≈ 16.67 -> 已经 vsync 限速, 这个游戏没有优化空间, 要换更重的 3D 游戏来测。
   - `cpu` 占大头 -> 做 2.1 (dynarec 块出口内联), 这是本工程最大的一个杠杆。
   - `rast` 占大头 -> 做光栅器 (`src/gpu/src/soft.c` span 循环, 例如
     `soft.c:2794-2820` 的纹理采样循环; `BlitScreen16` 的 MMX 化是次要的)。
   - `blit` 占大头 -> 先做 2.2 (删冗余 Clear) 再做 2.4 之后的直写方案。
2. **零风险清理**: 冗余 Clear (2.2)、`g_bShowFPS` 的 sprintf 改成只在数值变化时刷。
3. **低风险**: 2.5 输入轮询节流; 2.6 CD 缓存 (如果确实在读盘)。
4. **中等风险, 收益最大**: 2.1。
5. **别碰**: dynarec 跨块寄存器分配 / 块链接重写 / 光栅器重写。在当前没有
   逐游戏回归手段的前提下风险远大于收益。

---

## 5. 结论

当前 `default.xbe` 链路是健康的、可复现的基线。这套源码**不是**一条干净的发布源码,
而是 1.4 / 1.5 / 1.6(Reloaded) 三套 core 共用一个前端的开发工作区:
`pcsxbox.cpp` 是前端 + 1.5 core, `src\good` 是 1.4 core, `src\1.6` 是 1.6 core,
运行时通过 `XLaunchNewImage("default14.xbe" / "default16.xbe")` 切换。
所以**优化必须以 core 为边界**: `pcsxbox.cpp` / `src\psxcounters.c` 的改动要
三份同步 (`src\psxcounters.c`, `src\good\psxcounters.c`, `src\1.6\PsxCounters.c`)。

在没有 profiler 数据之前, 唯一确定值得做的是 2.1 (块出口内联) 和 2.2 (删冗余 Clear)。
