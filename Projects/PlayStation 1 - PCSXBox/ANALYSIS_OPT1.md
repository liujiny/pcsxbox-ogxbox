# PCSXBox OG Xbox - 性能分析与优化候选 (Round 1)

基线: `Release/PCSXBox_baseline3_final` (default.xbe 8F657CD9...),已实机验收通过。

## 1. 每帧结构(先搞清楚时间花在哪)

- `src/win32/wndmain.c:117` 起的 `while (Running)` 里连续 140 次 `psxCpu->Execute()`。
  一次 `Execute()` = 一个 PSX 基本块(`src/ix86/ir3000a.c:456-482`),块之间用
  `JMP32R(EAX)` 直接跳(`ir3000a.c:174-282`),所以一次调用实际会跑很多块。
- 所有的 vblank 工作都发生在 `src/psxcounters.c:112` 的 VSync End 分支:
  `GPU_updateLace()` -> `updateDisplay()` -> `xbox_put_image()` ->
  `pcsxbox.cpp::render_to_texture()` -> `Present()`。**Present 的 vsync 等待也在
  这条链里**,所以它属于"每帧固定开销"。
- 因此每帧 = CPU 模拟(dynarec) + GPU 软件光栅 + 贴图上传 + Present 等待,
  四段互不重叠,可以分别测量。

## 2. 各段现状与热点

### 2.1 dynarec(`src/ix86/ir3000a.c`,8MB 缓存)
每个基本块结束时都会:
1. `iFlushRegs()` —— 把当前映射到 x86 寄存器的 PSX 寄存器**全部写回** `psxRegs`(`ir3000a.c:222-283`);
2. `CALLFunc(psxBranchTest)` —— **每个分支一次 C 函数调用**(`src/r3000a.c:100`),
   函数体是 `cycle - psxNextsCounter >= psxNextCounter` 加一串 `psxRegs.interrupt` 位测试;
3. `ADD32ItoM(&psxRegs.cycle, count)` —— 每块一次内存累加。
块入口再从内存按需加载。这是 PCSX 1.5 世代的经典结构,也是这里最大的
可优化点(见 4.e / 4.h)。

### 2.2 GPU 软件光栅器(`src/gpu/src/soft.c` 7422 行, `prim.c`)
逐像素写 16bit,`BlitScreen16/32`(`draw.c:1824/2014`)已经做到 2 像素/32bit 存储。

### 2.3 D3D 上传路径(`pcsxbox.cpp::render_to_texture`)
每帧: `Texture->GetLevelDesc()`(已删) -> `LockRect` 整个 **1024x512 R5G6B5(1MB)**
-> `BlitScreen`(只写 w x h, 例如 320x240) -> `Unlock` -> `Clear` 全屏 ->
`BeginScene` -> panel -> 覆盖层 -> `EndScene` -> `Present`。

### 2.4 SPU / CD
- 混响默认开(`pcsxbox.cpp:1092`, `m_psxfix_spu_reverb = 2`),菜单里可关。
- CD 缓存 `CD_BUF_SECTORS = 10`(`pcsxbox.cpp:159`),每次 cache miss 读 2352*10 = 23.5KB;
  HDD 路径影响小,真实光驱/网络路径影响明显。

## 3. 本轮已做(低风险)

1. **编译开关修正**。`pcsxbox.vcproj`(Release|Xbox) 写的是
   `FavorSizeOrSpeed="1"`(/Ot 偏速度)、`EnableIntrinsicFunctions="TRUE"`(/Oi),
   但 `build_oldxdk/build.sh` 用的是 `/O2 /Ob1 /Os` —— 后两个是**反向**的
   (favor size + 只内联 inline 标记的函数)。
   现在默认 `/O2 /Ob2 /Ot /Og`;旧行为保留: `OPT_SET=size bash build_oldxdk/build.sh`。
   XBE 1,511,424 -> 1,613,824 字节。
2. 死代码: `pcsxbox.cpp::init_texture()` 里 `return 1;` 之后的 Release 永不可达;
   `render_to_texture()` 每帧一次的 `Texture->GetLevelDesc()` 只被注释掉的代码使用。
3. 新增 `PCSXBOX_PROFILE`(每帧计时,屏幕显示 cpu/rast/blit)。
4. `build_oldxdk/build.sh` 支持 `OUTDIR=` / `EXTRA_DEFINE=` / `OPT_SET=` 覆盖。

## 4. 候选优化(按 收益/风险 排序)

### 低风险,先做
- **a. LockRect 只锁实际用到的 w x h**(现在锁满 1MB)。是否真省取决于 Xbox 运行时
  lock 的代价 —— 等 profiling 数据出来再动,不要盲改。
- **b. 关混响**。菜单已有开关,2D 游戏通常不需要,直接省 SPU CPU。
- **c. `CD_BUF_SECTORS` 10 -> 32/64**。减少读停顿,内存代价 ~150KB(64MB 机器上可接受)。
- **d. FPS 字符串不要每帧 sprintf**。微小,但零风险。

### 中风险,收益最大的一档
- **e. 内联 `psxBranchTest` 快路径**(推荐第一个做)。
  在生成代码里直接比较 `psxRegs.cycle` / `psxNextsCounter` / `psxNextCounter` 和
  `psxRegs.interrupt`,只有命中才 `CALL`。改动局限在 `ir3000a.c` 的
  `iRet/iJump/iBranch/SetBranch`,不碰模拟语义。这类改动在 PCSX 系通常有 5~15%。
- **f. `BlitScreen16` 用 MMX/SSE 一次搬 4 像素**(现在的 RGB555->RGB565 位运算
  本来就是 SSE 友好的)。
- **g. 帧变化检测**: `gpu.c:942` 已经有 `bDoVSyncUpdate`,基本够用,可复查边界情况。

### 大改动,收益高但风险高
- **h. dynarec 跨块寄存器分配**,块之间保持 x86 寄存器映射,只在必要时 flush。
  PCSX-ReARMed 的做法,10~25%,但兼容性风险大,必须逐游戏回归。
- **i. 块链接 + 更快的块查找**(现在目标块没编译就回 C dispatcher)。
- **j. GPU 光栅器重写**(边界检查内联、按 span 填充)。
- **k. LTCG `/GL`**。vcproj 里 `WholeProgramOptimization="TRUE"`,我们没开。
  需要先验证这套 XDK 的 `link.exe` + `imagebld.exe` 能吃 /GL 目标文件。

## 5. 已排除的误区

- **`/G6` 是空操作**。老文档里写的 `/O2 /G6`,在 VC7.1 里已经不再生成不同代码:
  实测 `/O2` 与 `/O2 /G6` 的目标文件**除嵌入文件名外逐字节相同**。
- `/O2` 本身已隐含 `/Oi /Ob2 /Ot /Oy /Gs /GF /Gy`,所以真正生效的差异只有
  `/Os` 和 `/Ob1` 这两个反向开关。

## 6. 下一步(需要实机数据)

用 `Release/PCSXBox_opt1_prof` 跑,屏幕上每秒刷新一行:
`cpu=.. rast=.. blit=.. ms/frame`(含义见该包 README_OPT1.txt)。
- `cpu+rast << 16.67ms` -> 该游戏被 vsync 限速,没有优化空间,要换 CPU 重的 3D 游戏;
- `cpu` 占大头 -> 做 4.e;
- `rast-blit` 占大头 -> 做 4.f/4.j;
- `blit` 占大头 -> 做 4.a。
