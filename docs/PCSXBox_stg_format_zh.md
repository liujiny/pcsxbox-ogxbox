# PCSXBox `.stg` 每游戏设置文件格式说明

以 **Raiden Project**（PCSXbox V25「1470x Pack」里的那份）为例，逐项拆解 `*.stg` 的内容。

数据来源：`apps\PCSXbox\SAVES\PCSXBOX\Raiden Project\Raiden Project.stg`（310 字节），
并对照本项目源码 `pcsxbox.cpp` 的 `saveSettings()` / `loadSettings()`，以及同包
2082 个 `.stg` 的取值分布做交叉验证。

> 文末附有**全部 2082 个游戏的逐个设置表**（含每个文件的 core / GPU / SPU 与
> 非默认项），以及机器可读的 `docs/PCSXBox_stg_all_games.tsv`（43 列全量取值）。

---

## 1. `.stg` 是什么

* 每个游戏一份的**二进制设置快照**，不是文本 ini。
* 写入 / 读取实现：`pcsxbox.cpp` 的 `CXBoxSample::saveSettings()`（约 `:2249`）
  与 `CXBoxSample::loadSettings()`（约 `:2320`），内部就是一连串
  `fwrite(&var, sizeof(unsigned int), 1, setfile)`，顺序写出的结构体转储。
* 存放位置（实机）：`E:\SAVES\PCSXBOX\<游戏名>\<游戏名>.stg`。
  同目录下的 `.key`（文本手柄映射）、`.keh`（二进制手柄参数）、`.mcd`（记忆卡）
  是另外的文件，`.stg` 只负责「这个游戏该怎么跑」。
* 版本差异：
  * 本项目（v1.5 Plus 源码）的 `saveSettings()` 写 **43 个 dword = 172 字节**。
  * V25 那份是 **310 字节**，前 172 字节与本项目逐项一致，后面是 V25/Madmab
    界面的扩展区。同包里还有 306 / 270 / 266 字节的变体，差别同样只在扩展区。

## 2. 文件结构

| 区域 | 内容 |
| --- | --- |
| `0x00` – `0xAB`（43 × 4 = 172 字节） | 43 个 32 位**小端**整数，即游戏配置页那一屏的选项。本项目源码与之一致 |
| `0xAC` – 文件尾 | V25/Madmab 界面扩展区：手柄预设名（对应 `PresetC\*.ini`）、一段手柄映射二进制，以及一批本项目源码里没有的选项 |

## 3. 字段表（Raiden Project 的实际取值）

置信度标记：

* **已验证** —— 源码字段名 + 全包分布 + 游戏语义三者互证。
* **高概率** —— 按 XPort/PCSXBox 结构体字段顺序对齐推断，含义可靠，
  个别项目的 UI 措辞可能略有出入。

| 偏移 | 字段 | 该游戏的值 | 作用 | 置信度 |
| --- | --- | --- | --- | --- |
| `0x00` | `vandalFix` | 0 | Vandal Hearts 类专用修复。全包仅 13 个游戏开启：Vandal Hearts I/II、Parasite Eve 1/2、Thousand Arms 1/2、Tomb Raider IV、WWF In Your House、Spin Jam、Defcon 5、Gunnm 等 | 已验证 |
| `0x04` | `Xa` | 0 | XA 音频修复。全包只有 Batman Forever 打开 | 高概率 |
| `0x08` | `Sio` | 0 | SIO 修复。全包 3 个：Battle Arena Toshinden 4、Cyberwar、Kyuutenkai | 高概率 |
| `0x0C` | `Mdec` | 0 | MDEC / FMV 修复。全包 2082 个文件全部为 0（没人用） | 高概率 |
| `0x10` | `Cdda` | 0 | CDDA 音轨修复。全包 244 个游戏开启，是这个包里最常用的一项修复 | 高概率 |
| `0x14` | `Cpu` | 0 | CPU 修复。全包 4 个：Discworld Noir、UPP、Viewpoint、Wheel of Fortune | 高概率 |
| `0x18` | `SpuIrq` | 0 | SPU IRQ 修复。全包 34 个 | 高概率 |
| `0x1C` | `VSyncWA` | 0 | VSync / 等待 CPU 类修复。全包无人开启 | 高概率 |
| `0x20` | `graphicsFixes` | **8** | 图形修复**位掩码**（全包取值 0 / 8 / 64 / 128 / 136 / 192 …，典型的 bit 组合） | 已验证 |
| `0x24` | `frameskip` | **1** | 帧跳过开关。**想关掉就改成 0** | 高概率 |
| `0x28` | `framelimit` | 1 | 帧率限制开关（全包 1368 开 / 714 关） | 高概率 |
| `0x2C` | `bitDepth` | **16** | 色深，全包恒为 16bpp | 已验证 |
| `0x30` | `biosMode` | 1 | 1 = 使用真实 BIOS，0 = HLE 模拟 BIOS（全包 2059 用真实 BIOS） | 高概率 |
| `0x34` | `xboxSFilter` | 0 | Xbox 软件滤镜编号，全包恒 0 | 高概率 |
| `0x38` | `ff9` | 0 | 「FF9 类」修复。全包 4 个：FFT、FFT - TLW、SaGa Frontier 2、Discworld Noir | 高概率 |
| `0x3C` | `memcardnum1` | 0 | PSX 1 号记忆卡槽用哪块卡（0 = 卡 1） | 已验证 |
| `0x40` | `memcardnum2` | 1 | PSX 2 号记忆卡槽用哪块卡（1 = 卡 2）——与目录里的 `Raiden Projec0.mcd` / `Raiden Projec1.mcd` 两张卡完全对应 | 已验证 |
| `0x44` | `SpuIrq2` | 0 | 第二类 SPU IRQ 修复。全包 59 个，含 Gran Turismo、Einhander、Koudelka | 高概率 |
| `0x48` | `spu_usexa` | 1 | 启用 XA 音频（全包 2078 个为 1，与源码默认值 1 一致） | 已验证 |
| `0x4C` | `spu_xaspeed` | 0 | XA 速度修复，全包仅 1 个开启 | 高概率 |
| `0x50` | `spu_reverb` | 1 | 混响等级（全包取值 0 / 1 / 2） | 已验证 |
| `0x54` | `spu_interpolation` | 2 | 插值等级。2 是全包默认（1975 / 2082），对应 Gaussian/Good | 已验证 |
| `0x58` | `spu_dbuf` | 0 | SPU 双缓冲，基本全关（12 个开启） | 高概率 |
| `0x5C` | `gpu_version` | **1** | GPU 插件版本。V25 的 GPU 列表为 6 项（UnaiGpuOld / UnaiGPU / 1.12 / 1.15 / 1.16 / 1.18），而文件里正好只出现 0–5 六种取值；本游戏 = 1 → UnaiGPU | 高概率 |
| `0x60` | `spu_version` | 0 | SPU 插件版本（全包取值 0–3） | 高概率 |
| `0x64` | `FixFF` | 1 | 快进（FF）相关修复开关 | 高概率 |
| `0x68` | `core_version` | **0** | PSX 核心版本。**全包 2082 个文件都是 0** | 高概率 |
| `0x6C` | `screenX` | 0 | 自定义画面左上角 X | 高概率 |
| `0x70` | `screenY` | 0 | 自定义画面左上角 Y | 高概率 |
| `0x74` | `screenMaxX` | 0 | 自定义画面右下角 X | 高概率 |
| `0x78` | `screenMaxY` | 0 | 自定义画面右下角 Y（这 4 项全包全为 0，即都走自动尺寸） | 高概率 |
| `0x7C` | `xbox_oldstate` | 0 | 旧版即时存档格式开关 | 高概率 |
| `0x80` | `memcard_state` | 1 | 存档时一并保存记忆卡数据（全包恒 1，等于源码默认值） | 已验证 |
| `0x84` | `soundtimer` | 2 | 声音定时器方法（2 = Threaded，V25 默认；0 = Original） | 高概率 |
| `0x88` | `controller1` | 0 | 1P 手柄类型。V25 自己的调试字符串就叫 `controller1_type` | 已验证 |
| `0x8C` | `controller2` | 0 | 2P 手柄类型 | 已验证 |
| `0x90` | `controller3` | 0 | 3P 手柄类型 | 已验证 |
| `0x94` | `controller4` | 0 | 4P 手柄类型（这四项在全包里永远同进同退；0 = Standard，2 = DualShock） | 已验证 |
| `0x98` | `multitap` | 0 | 多分插。全包 46 个为 1，且全是 Micro Machines V3、Crash Bash、Gauntlet Legends、Poy Poy、NBA Jam 这类 4 人游戏 | 已验证 |
| `0x9C` | `rumble_enabled1` | 1 | 1P 震动开关 | 已验证 |
| `0xA0` | `rumble_enabled2` | 1 | 2P 震动开关 | 已验证 |
| `0xA4` | `rumble1` | 0 | 1P 震动强度（取值 0 或 20） | 已验证 |
| `0xA8` | `rumble2` | 0 | 2P 震动强度（取值 0 或 20） | 已验证 |

> 关于 Rumble：V25 的 XBE 里能找到 `P1 Rumble Increase` / `P2 Rumble Increase`
> 字符串，而 `0xA4` / `0xA8` 的取值恰好只有 0 和 20 两种（力度增量），
> 因此这两项对应「震动增强量」。

## 4. `0xAC` 之后的扩展区

* `0xAC`：一个 4 字节的 0。
* 紧接着是**手柄预设配置名**，对应 `PresetC\*.ini`：
  * Raiden Project 里是 `standard.ini`（写法有点脏，实际内容像 `Standard.ini`
    少了首字母 `S` 和 `.`）；
  * 其它游戏能看到 `Dualshock new reversed`、`Forsaken.ini`、`Dualshock new.ini`
    等，与 `PresetC\` 目录里的预设文件名吻合。
* 再往后是一段二进制手柄映射数据。
* V25 还有一批**本项目源码里完全没有**的每游戏选项，它们也在这一段里：
  * UnaiGPU 相关：`ilace_lowres`、`use_dithering`、`use_blending`、`fast_lighting`、
    `enable_lighting`、`skip_render_lines`、`skip_render_pixels`、
    `frameskip_unai`、`framelimit_unai`
  * CD 相关：`use_new_cd_code`、`xa_attenuation`、`clock_speed`、
    `auto_framelimit_on_mdec`、`disable_memslot2`、`xbox_bias`、`speed_bump`
  * SPU 相关：`spu_buff_size`、`xa_interpolation`

  这批字段名来自 V25 `default.xbe` 内的调试输出字符串；由于没有 V25 源码，
  这一段的具体字节排布只能给出大致判断，不像前 43 项那样可以逐项打包票。

## 5. 结论是怎么验证的

1. **结构体顺序**：直接用本项目 `saveSettings()` 的字段顺序去套，
   全包 2082 个 `.stg` 的取值分布全部落在合理区间。
2. **值域特征**：`0x2C` 恒为 16（色深 16bpp）；`0x88`–`0x94` 四项分布完全相同
   （同一类字段、四个玩家）；`0x7C` 恒 0、`0x80` 恒 1、`0x48` 恒 1、`0x54` 恒 2
   —— 正好等于源码里的默认值。
3. **语义互证**：
   * `0x00 = 1` 的 13 个游戏里包含 Vandal Hearts I 和 II → `vandalFix`。
   * `0x98 = 1` 的 46 个游戏全是 4 人同屏游戏 → `multitap`。
   * `0x3C / 0x40` 与每个游戏目录里恰好两张 `.mcd` 一一对应 → 记忆卡槽位。
   * V25 XBE 字符串里存在 `controller1_type` … `controller4_type`、`multitap`、
     `core_version`、`Preset_Controller_Config` 等，与字段位置吻合。

## 6. 实用操作

* **关掉帧跳过**：把 `0x24` 的 `01 00 00 00` 改成 `00 00 00 00`（小端）。
  `0x28` 是帧率限制，同理。也可以直接进游戏按 R3 菜单改，退出时模拟器会回写。
* **重建设置**：删掉该游戏的 `.stg`，模拟器会生成一套默认值。
* **不要跨版本混用 `.stg`**：V25 的 `.stg` 是 310 字节、由 V25 界面（或 PC 端
  批量生成工具）写出，本项目只写 172 字节。前 43 项可以直接读进来，但要注意：
  * `0x68 core_version = 0` 在本项目里意味着 **1.4 核心**，会链式启动
    `default14.xbe`。从 V25 包拷配置过来后，记得在菜单里把它改成 1.5 再保存。
  * 屏幕那 4 项（`0x6C`–`0x78`）为 0 不必担心：本项目源码里有兜底
    （`pcsxbox.cpp:4726` 与 `:4819`），数值非法时回退 640×480，不会黑屏。
  * 扩展区里那批 V25 选项（UnaiGPU / CD / SPU 细节）会被本项目忽略。

---

## 附：Raiden Project 这份 `.stg` 的一句话总结

真实 BIOS、16bpp、帧跳过开、帧率限制开、图形修复位掩码 8、UnaiGPU（版本索引 1）、
SPU 插值 Gaussian/Good、混响等级 1、声音定时器 Threaded、两张记忆卡分别放在
PSX 1/2 号槽、手柄类型 Standard、震动开但增强量 0、核心索引 0、画面尺寸走自动。

---

## 全游戏设置表（2082 个 `.stg`）

下面是这个 V25 包里**每一个** `.stg` 的实际取值。

**怎么读**

* `core` / `gpu` / `spu` 三列是版本索引，已经换算成本项目菜单里显示的名字。
  `⚠n` 表示该文件里的索引超出本项目支持的范围（见下面「版本索引对照」）。
* `差异项` 列只写**和全包众数不同**的字段——没列出来的字段就是众数值。
  众数在下一张表里给出；字段含义见第 3 节字段表。
* `大小` 是该 `.stg` 的字节数（310 / 306 / 270 / 266 四种，差别只在 `0xAC`
  之后的 V25 扩展区，前 43 项结构和本项目一致）。

> 需要原始 43 列取值做批量比对时，用同目录的 `PCSXBox_stg_all_games.tsv`，
> 里面是 43 列全量数据，字段名就是表头。

### 参照基准：全包 2082 个文件的众数

| 字段 | 众数 | 众数出现次数 | 说明 |
| --- | --- | --- | --- |
| `vandalFix` | 0 | 2069 | Vandal Hearts 类修复 |
| `psxfix_Xa` | 0 | 2081 | XA 音频修复 |
| `psxfix_Sio` | 0 | 2079 | SIO 修复 |
| `psxfix_Mdec` | 0 | 2082 | MDEC/FMV 修复 |
| `psxfix_Cdda` | 0 | 1838 | CDDA 音轨修复 |
| `psxfix_Cpu` | 0 | 2078 | CPU 修复 |
| `psxfix_SpuIrq` | 0 | 2048 | SPU IRQ 修复 |
| `psxfix_VSyncWA` | 0 | 2082 | VSync/等待修复 |
| `graphicsFixes` | 0 | 924 | 图形修复位掩码 |
| `frameskip` | 0 | 1170 | 帧跳过 |
| `framelimit` | 1 | 1368 | 帧率限制 |
| `bitDepth` | 16 | 2082 | 色深 |
| `biosMode` | 1 | 2059 | 1=真实 BIOS / 0=HLE |
| `xboxSFilter` | 0 | 2082 | Xbox 软件滤镜 |
| `ff9` | 0 | 2078 | FF9 类修复 |
| `memcardnum1` | 0 | 2081 | 1 号槽用哪块记忆卡 |
| `memcardnum2` | 1 | 2081 | 2 号槽用哪块记忆卡 |
| `psxfix_SpuIrq2` | 0 | 2023 | 第二类 SPU IRQ 修复 |
| `spu_usexa` | 1 | 2078 | 启用 XA |
| `spu_xaspeed` | 0 | 2081 | XA 速度修复 |
| `spu_reverb` | 1 | 1144 | 混响等级 |
| `spu_interpolation` | 2 | 1975 | SPU 插值等级 |
| `spu_dbuf` | 0 | 2070 | SPU 双缓冲 |
| `gpu_version` | 4 | 698 | GPU 插件索引 |
| `spu_version` | 1 | 985 | SPU 插件索引 |
| `FixFF` | 1 | 1572 | 快进方式 |
| `core_version` | 0 | 2082 | PSX 核心索引 |
| `screenX` | 0 | 2082 | 自定义左上 X |
| `screenY` | 0 | 2082 | 自定义左上 Y |
| `screenMaxX` | 0 | 2082 | 自定义右下 X |
| `screenMaxY` | 0 | 2082 | 自定义右下 Y |
| `xbox_oldstate` | 0 | 2082 | 旧版即时存档 |
| `memcard_state` | 1 | 2082 | 存档时保存记忆卡 |
| `soundtimer` | 2 | 1710 | 声音定时器 |
| `controller1` | 0 | 1107 | 1P 手柄类型 |
| `controller2` | 0 | 1108 | 2P 手柄类型 |
| `controller3` | 0 | 1108 | 3P 手柄类型 |
| `controller4` | 0 | 1108 | 4P 手柄类型 |
| `multitap` | 0 | 2036 | 多分插 |
| `rumble_enabled1` | 0 | 1075 | 1P 震动开关 |
| `rumble_enabled2` | 0 | 1075 | 2P 震动开关 |
| `rumble1` | 0 | 2080 | 1P 震动强度 |
| `rumble2` | 0 | 2080 | 2P 震动强度 |

### 版本索引对照（重要）

`core_version` / `gpu_version` / `spu_version` 三个索引不是同一套编号空间，
跨版本拷贝 `.stg` 时必须换算。

| 索引 | 本项目 `core` | 本项目 `gpu` | 本项目 `spu` | V25 菜单里的名字 |
| --- | --- | --- | --- | --- |
| 0 | 1.4（启动 `default14.xbe`） | 1.12 | 1.6 | Unai Gpu Old |
| 1 | 1.5（本 XBE，默认） | 1.15 | 1.9（默认） | Unai GPU |
| 2 | 1.6（启动 `default16.xbe`） | 1.16（默认） | 超出范围→当 1.9 处理 | (1.18) |
| 3 | — | 超出范围→`switch` 无分支 | 同上 | 1.16 |
| 4 | — | 超出范围→`switch` 无分支 | 同上 | 1.15 |
| 5 | — | 超出范围 | 同上 | 1.12 |

依据：本项目 `pcsxbox.cpp:1775-1777` 的三个数组（`cores[]`/`gpus[]`/`spus[]`）、
`src/gpu/src/gpu.c:507` 的 `switch( xbox_get_gpu_version() )`、
`src/plugins.c:673` 的 `xbox_get_spu_version()`。
V25 那一列来自 V25 `default.xbe` 里的字符串块 （`Unai Gpu Old` / `Unai GPU` / `(1.18)` / `1.16` / `1.15` / `1.12` 连续排布）。

> **两个坑**：
>
> 1. 本包里 **2082 个文件全部 `core_version = 0`**，在本项目里就是 **1.4 核心**，
>    会去链式启动 `D:\default14.xbe`。直接从 V25 包拷 `.stg` 过来，
>    记得进 R3 菜单把 Core Version 改成 1.5 再退出保存。
> 2. `gpu_version` 全包取值 0–5，而本项目只认识 0–2。
>    取值 ≥3 的文件（本包共 **1138** 个：3→335、4→698、5→105）进来后
>    `gpus[]` 会越界，菜单里那一行显示的字符串不可信；
>    按一次 GPU 选项让 `%3` 归位即可。
> 3. `spu_version` 全包取值 0–3，本项目只分 0（1.6）/ 其余（1.9）两支，
>    所以本包 189 个取值 2 或 3 的文件会被当成 1.9 跑，功能上没问题。

### 逐游戏设置


#### 0-9（5）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| 360 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| 3D Baseball | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0 |
| 3D Lemmings | 310 | 1.4 | 1.12 | 1.6 | framelimit=0, spu_interpolation=1 |
| 40 Winks | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| 4x4 World Trophy | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, rumble_enabled=1 |

#### A（135）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| A.IV - Evolution Global | 306 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Ace Combat 2 | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, biosMode=0, controller=2, rumble_enabled=1 |
| Ace Combat 3 (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=16, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ace Combat 3 (Disc 2) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ace Combat 3 - Electrosphere | 270 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Aces of the Air | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_interpolation=1 |
| Acid | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Aconcagua (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Aconcagua (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Action Bass | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Activision Classics | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| Actua Golf | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Actua Golf 2 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Actua Pool | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192, framelimit=0, spu_reverb=2 |
| Actua Soccer - Club Edition | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Addie's Present - To Moze From Addie | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Advan Racing | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, framelimit=0, spu_reverb=0, FixFF=0 |
| Advanced D&D - Iron & Blood | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Advanced Variable Geo | 310 | 1.4 | 1.15 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Advanced Variable Geo 2 | 310 | 1.4 | 1.16 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Adventure of Little Ralph | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Adventures of Lomax | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Aeon Flux (Prototype) | 310 | 1.4 | 1.12 | 1.9 | psxfix_Cdda=1, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Afraid Gear | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Afro-ken - The Puzzle | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Agent Armstrong | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, soundtimer=0 |
| Agile Warrior - F-111X | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Air Combat | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Air Hockey | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Air Management 96 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192 |
| Air Race | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Airgrave | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Aironauts | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Akuji - The Heartless | 270 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Akuji the Heartless | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, frameskip=1, spu_interpolation=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Aladdin - Nasira's Revenge | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Alex Ferguson's Player Manager 2001 | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Alexi Lalas International Soccer | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Alfred Chicken | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Alien Resurrection | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Alien Resurrection (Prototype) | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Alien Trilogy | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| All-Star Action (Disc 1) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| All-Star Action (Disc 2) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| All-Star Baseball 1997 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| All-Star Slammin' D-Ball | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0 |
| Allied General | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0 |
| Alone in the Dark (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Alone in the Dark (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Alone in the Dark - Jack is Back | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Alundra | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Alundra 2 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Amazing Virtual Sea-Monkeys | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| American Pool | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Amerzone (Disc 1) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=136, framelimit=0, soundtimer=0 |
| Amerzone (Disc 2) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=72, framelimit=0, soundtimer=0 |
| Andretti Racing | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Animaniacs - Ten Pin Alley | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0 |
| Animorphs | 270 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Animorphs - Shattered Reality | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=24, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Anna Kournikova - Smash Court Tennis | 306 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Another World | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, FixFF=0, soundtimer=0 |
| Ape Escape | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2 |
| Apocalypse | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Apocalypse Zero | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Aqua GT | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Aquanaut's Holiday | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Arc the Lad | 310 | 1.4 | ⚠3 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Arc the Lad - Arc Arena | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Arc the Lad II | 310 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Arc the Lad III (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Arc the Lad III (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Arcade Party Pak | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Arcade's G. Hits - Atari 1 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| Arcade's G. Hits - Atari 2 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Arcade's G. Hits - Midway 1 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| Arcade's G. Hits - Midway 2 | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Archer Maclean's 3D Pool | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Area 51 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128 |
| Ark of Time | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Arkanoid R 2000 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Arkanoid Returns | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Armed Fighter | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=1 |
| Armored Core | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=1 |
| Armored Core - MoA (Disc 1) | 310 | 1.4 | ⚠5 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Armored Core - MoA (Disc 2) | 310 | 1.4 | ⚠5 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Armored Core - Project Phantasma | 310 | 1.4 | ⚠5 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Armored Trooper Votoms | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Armorines - Project SWARM | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Army Men - Air Attack | 310 | 1.4 | 1.12 | ⚠3 | graphicsFixes=8, controller=2, rumble_enabled=1 |
| Army Men - Air Attack 2 | 306 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, framelimit=0, spu_reverb=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men - Green Rogue | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Army Men - Land Sea & Air | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men - Land Sea Air | 306 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=208, frameskip=1, framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men - Lock 'n' Load | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Army Men - Omega Soldier | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=208, frameskip=1, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men - Operation Meltdown | 270 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=208, framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men - Sarge's Heroes | 306 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=208, frameskip=1, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men - Team Assault | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men - World War | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Army Men - World War FF | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Army Men - World War LSA | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Army Men - World War TA | 310 | 1.4 | 1.12 | ⚠3 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Army Men 3D | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=208, frameskip=1, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Army Men World War - Land Sea Air | 270 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=208, framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Art Camion | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Art Truck Battle | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, framelimit=0 |
| Art Truck Battle 2 | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Arthur to Astaroth | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Assault | 310 | 1.4 | 1.15 | ⚠2 | graphicsFixes=200, FixFF=0 |
| Assault - Retribution | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=192, spu_reverb=0, controller=2, rumble_enabled=1 |
| Assault Rigs | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Assault Suits Valken 2 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Asteroids | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Asuka 120 Excellent - Burning Fest | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, psxfix_SpuIrq2=1, spu_dbuf=1 |
| Asuka 120 Excellent - Burning Remixes | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, psxfix_SpuIrq2=1, spu_dbuf=1 |
| Asuka 120 Final - Burning Fest | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, psxfix_SpuIrq2=1, spu_dbuf=1, controller=2, rumble_enabled=1 |
| Asuka 120 Special - Burning Fest | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, psxfix_SpuIrq2=1, spu_dbuf=1 |
| Asuka 120 Special - Burning Remixes | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, psxfix_SpuIrq2=1, spu_dbuf=1 |
| Asuncia | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Atari Anniversary Redux | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Atlantis - Lost Tales (Disc 1) | 310 | 1.4 | 1.16 | 1.6 | frameskip=1 |
| Atlantis - Lost Tales (Disc 2) | 310 | 1.4 | 1.16 | 1.6 | frameskip=1 |
| Atlantis - Lost Tales (Disc 3) | 310 | 1.4 | 1.16 | 1.6 | frameskip=1 |
| Attack of the Saucerman | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, controller=2 |
| ATV - Quad Power Racing | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| ATV Mania | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| ATV Racers | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Austin Powers Pinball | 310 | 1.4 | 1.16 | ⚠3 | graphicsFixes=128, spu_reverb=2, controller=2, rumble_enabled=1 |
| Auto Destruct | 310 | 1.4 | 1.15 | ⚠3 | psxfix_Cdda=1, graphicsFixes=192, spu_reverb=0 |
| Autobahn Raser II | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ayrton Senna Kart Duel | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Ayrton Senna Kart Duel 2 | 310 | 1.4 | 1.15 | ⚠3 | graphicsFixes=128, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Aztec | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, FixFF=0 |
| Azure Dreams | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128 |

#### B（122）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| B-Movie | 270 | 1.4 | ⚠3 | 1.9 | spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| B.L.U.E. - Legend of Water | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Baby Felix Tennis | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Backstreet Billiards | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=0, soundtimer=0 |
| Backyard Soccer | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Bakusou Kyoudai - Eternal Wings | 310 | 1.4 | 1.12 | ⚠3 | graphicsFixes=8, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Baldies | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Baldur's Gate (Disc 1) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Baldur's Gate (Disc 2) | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Baldur's Gate (Disc 3) | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Ball Breakers | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ballblazer Champions | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=24, frameskip=1 |
| Ballerburg | 310 | 1.4 | 1.15 | ⚠3 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2 |
| Ballerburg - Castle Chaos | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ballistic | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Baroque | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Bases Loaded '96 - Double Header | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Bass Landing | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Bass Rise | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Batman & Robin | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Batman - Gotham City Racer | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Batman Beyond - Return of Joker | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Batman Forever | 310 | 1.4 | 1.16 | 1.6 | psxfix_Xa=1, psxfix_Cdda=1, frameskip=1, framelimit=0, rumble_enabled=1 |
| Battle Arena Nitoushinden | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, FixFF=0, soundtimer=0 |
| Battle Arena Toshinden | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128 |
| Battle Arena Toshinden 2 Plus | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Battle Arena Toshinden 3 | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=64, frameskip=1, controller=2, rumble_enabled=1 |
| Battle Arena Toshinden 4 | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, spu_reverb=0, spu_interpolation=0, FixFF=0, controller=2, rumble_enabled=1 |
| Battle Arena Toshinden 4 - Subaru | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Sio=1, psxfix_Cdda=1, frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Battle Hunter | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Battle Qix | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Battle Stations | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Battlesport | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| BattleTanx - Global Assault | 270 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=192, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Beast Wars - Transformers | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, FixFF=0 |
| Beast Wars - Transmetals | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Beatmania | 306 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=192, framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Beavis & Butt-Head - Virtual Stupidity | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1 |
| Bedlam | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0 |
| Beyblade | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Beyond the Beyond | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Big Air | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Big Challenge Golf - Tokyo CC | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0 |
| Big Challenge Golf - Tokyo YCC | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Big Strike Bowling | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Billiards | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Bio FREAKS | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Bishi Bashi Special | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Black Bass with Blue Marlin | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Black Dawn | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0 |
| Blade | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Blam - Machine Head | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Blast Chamber | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, frameskip=1, FixFF=0 |
| Blast Lacrosse | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Blast Radius | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=64, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Blaster Master | 270 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2 |
| Blaster Master - Blasting Again | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_interpolation=1 |
| Blasto | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=200, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Blaze and Blade - Eternal Quest | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1 |
| Blazing Dragons | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Block & Switch | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Block Buster | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, spu_reverb=0, rumble_enabled=1 |
| Block Kuzushi - Deden no Gyakushuu | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, soundtimer=0 |
| Block Kuzushi - Kowashite Help! | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Block Kuzushi 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Block Wars | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Blockids | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Bloody Roar | 310 | 1.4 | 1.16 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Blue Breaker | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| Blue Breaker Burst - Egao | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Blue Breaker Burst - Hohoemi | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Bokan Desu Yo | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, rumble_enabled=1 |
| Bokan GoGoGo | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Bokan to Ippatsu! Doronboo | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0, rumble_enabled=1 |
| Bomberman | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0 |
| Bomberman Fantasy Race | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0 |
| Bomberman Wars | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128 |
| Bomberman World | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Bombing Islands | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Bomboat | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| BoomBots | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Brahma Force | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| BrainDead 13 (Disc 1) | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| BrainDead 13 (Disc 2) | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Brave Fencer Musashi | 310 | 1.4 | ⚠5 | ⚠3 | psxfix_Cdda=1, graphicsFixes=16, frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Brave Prove | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Bravo Air Race | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Break Out | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Break Point | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Break Thru! | 310 | 1.4 | ⚠3 | ⚠3 | graphicsFixes=192, spu_reverb=2, soundtimer=0 |
| Break Volley | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Breath of Fire III | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Breath of Fire IV | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, rumble_enabled=1 |
| Breed Master | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Brian Lara Cricket | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Brigandine - Grand Cross (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=24, frameskip=1, controller=2, rumble_enabled=1 |
| Brigandine - Grand Cross (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=24, frameskip=1, controller=2, rumble_enabled=1 |
| Brightis | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Broken Helix | 310 | 1.4 | 1.12 | ⚠2 | graphicsFixes=128, rumble_enabled=1 |
| Broken Sword | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, rumble_enabled=1 |
| Broken Sword II The Smoking Mirror | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, rumble_enabled=1 |
| Brunswick Circuit Pro Bowling | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Brunswick Circuit Pro Bowling 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Bubble Bobble & Rainbow Islands | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Bubsy 3D - Furbitten Planet | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0 |
| Buckle Up! | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0 |
| Buggy | 306 | 1.4 | 1.12 | 1.6 | framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Bugriders | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Bugs & Taz - Time Busters | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, spu_reverb=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Bugs Bunny & Taz - Time Busters | 310 | 1.4 | 1.12 | ⚠3 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Bugs Bunny - Lost in Time | 310 | 1.4 | ⚠5 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Building Crush! | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Burning Road | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, spu_reverb=0 |
| BursTrick - Wake Boarding | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Bushido Blade | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=192, spu_interpolation=1, FixFF=0, soundtimer=0 |
| Bushido Blade 2 | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Bust-A-Groove | 310 | 1.4 | 1.12 | 1.6 | spu_reverb=0 |
| Bust-A-Move 2 | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Bust-A-Move 3 DX | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Bust-A-Move 4 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Buster Bros. Collection | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, biosMode=0, spu_reverb=0 |
| Buttsubushi | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, FixFF=0, multitap=1, controller=2, rumble_enabled=1 |

#### C（149）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| C - The Contra Adventure | 310 | 1.4 | ⚠4 | 1.6 | psxfix_SpuIrq=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| C-12 - Final Resistance | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=88, frameskip=1, framelimit=0, biosMode=0, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| C1 - Circuit | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Cabela's Big Game Hunter | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Cabela's Ultimate Deer Hunt | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Caesars Palace 2000 - MGE | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Calcolo! | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| California Watersports | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Call of Cthulhu - Prisoner of Ice | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Capcom G. (Blazing Guns) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Capcom G. (Chronicles of Arthur) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Capcom G. (First Generation) | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Capcom G. (Street Fighter) | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Capcom G. (Wings of Destiny) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Capcom vs. SNK - Millennium Fight | 306 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Capcom vs. SNK Pro | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Captain Commando | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Card Captor Sakura (Disc 1) | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0 |
| Card Captor Sakura (Disc 2) | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0 |
| Card Games | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Cardinal Syn | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Carnage Heart | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0 |
| Carnage Heart (Briefing - Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Carnage Heart (Missions - Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=200, spu_reverb=0 |
| Carom Shot | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Carom Shot 2 | 310 | 1.4 | ⚠4 | ⚠2 | frameskip=1, spu_interpolation=1, controller=2, rumble_enabled=1 |
| CART World Series | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=8, frameskip=1, framelimit=0, spu_reverb=0 |
| Casper | 310 | 1.4 | ⚠5 | 1.6 | psxfix_SpuIrq=1, framelimit=0, biosMode=0, psxfix_SpuIrq2=1, spu_reverb=0 |
| Castlevania Chronicles | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Castlevania Rondo of the Night | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Castlevania Symphony of the Night | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Castrol Honda Superbike | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Castrol Honda VTR | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Cave Story | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Celebrity Deathmatch | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Centipede | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Champion Wrestler | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| Championship Manager Quiz | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Chaos Break | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Chaos Control | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, soundtimer=0 |
| Chase the Express (Disc 1) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Checkmate | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Checkmate II | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Cheesy | 310 | 1.4 | ⚠4 | 1.9 | spu_reverb=0 |
| Chessmaster 3-D | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128 |
| Chessmaster II | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0 |
| Chicken Run | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Chill | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| China - Forbidden City | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, FixFF=0 |
| Chocobo Racing | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Chocobo's Dungeon | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, biosMode=0, FixFF=0, soundtimer=0 |
| Chocobo's Dungeon 2 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Choro Q | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Choro Q - Wonderful! | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Choro Q 2 | 306 | 1.4 | ⚠3 | ⚠3 | graphicsFixes=192, spu_interpolation=1, FixFF=0 |
| Choro Q Jet - Rainbow Wings | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Chou Aniki | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Chris Kamara's Street Soccer | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=8, spu_reverb=0, controller=2, rumble_enabled=1 |
| Chronicles of the Sword (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Chrono Cross (Disc 1) | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Chrono Cross (Disc 2) | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Chrono Trigger | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=192, spu_reverb=0 |
| Circuit Breakers | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, multitap=1, controller=2, rumble_enabled=1 |
| City of Lost Children | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0, rumble_enabled=1 |
| Civilization | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Civilization 2 | 310 | 1.4 | 1.16 | ⚠2 | graphicsFixes=128, controller=1 |
| Cleopatra Fortune | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, FixFF=0 |
| Cleopatra's Fortune | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Clock Tower | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, rumble_enabled=1 |
| Clock Tower - First Fear | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Clock Tower II - The Struggle Within | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=216, soundtimer=0 |
| Club Solaris | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=88, frameskip=1, framelimit=0, spu_interpolation=1 |
| Codename - Tenka | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136, spu_reverb=0 |
| Colin McRae Rally | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192, frameskip=1, framelimit=0 |
| Colin McRae Rally 2.0 | 310 | 1.4 | ⚠5 | ⚠3 | psxfix_Cdda=1, graphicsFixes=72, spu_reverb=0, controller=2, rumble_enabled=1 |
| College Slam | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Colony Wars | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Colony Wars (Disc 1) | 310 | 1.4 | 1.12 | ⚠3 | psxfix_Cdda=1, frameskip=1, controller=2, rumble_enabled=1 |
| Colony Wars (Disc 2) | 310 | 1.4 | 1.12 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Colony Wars - Vengeance | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Colony Wars 3 - Red Sun | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Command & Conquer (GDI) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0 |
| Command & Conquer (NOD) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0 |
| Command & Conquer Commando | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, soundtimer=0 |
| Command & Conquer Red Alert (Allies) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0 |
| Command & Conquer Red Alert (Soviet) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0 |
| Command & Conquer Retaliation (Allies) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, rumble_enabled=1 |
| Command & Conquer Retaliation (Soviet) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, rumble_enabled=1 |
| Community Pom | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Constructor | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Contender | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Contender 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Contra - Legacy of War | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192 |
| Cool Boarders 2001 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Coolboarders | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Coolboarders 2 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0 |
| Coolboarders 2001 | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Coolboarders 3 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=64, framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Coolboarders 4 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_interpolation=1, FixFF=0, multitap=1, controller=2 |
| Cosmowarrior Zero | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Cotton - Fantastic Night Dreams | 266 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, spu_interpolation=1 |
| Countdown Vampires (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Countdown Vampires (Disc 2) | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Courier Crisis | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Covert Ops - Nuclear Dawn (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Covert Ops - Nuclear Dawn (Disc 2) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Cowboy Bebop | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=136, spu_reverb=0, controller=2, rumble_enabled=1 |
| Crash Bandicoot | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=160, spu_reverb=0 |
| Crash Bandicoot (Prototype) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0, spu_reverb=0 |
| Crash Bandicoot 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| Crash Bandicoot 3 - Warped | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=160, spu_reverb=0, controller=2, rumble_enabled=1 |
| Crash Bash | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, multitap=1, controller=2, rumble_enabled=1 |
| Crash Team Racing | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=152, spu_reverb=0, spu_dbuf=1, multitap=1, controller=2, rumble_enabled=1 |
| Crazy Balloon 2000 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Crazy Climber | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, spu_reverb=0 |
| Crazy Climber 2000 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Creature Shock (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Creature Shock (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Creatures | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Creatures - Raised in Space | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Crime Crackers | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| Crime Killer | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Crisis Beat | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Crisis City | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Critical Blow | 310 | 1.4 | 1.12 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Critical Depth | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, rumble_enabled=1 |
| Criticom | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Croc | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| Croc 2 | 306 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, controller=2 |
| Crossroad Crisis | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=192, spu_reverb=0, FixFF=0 |
| Crusader - No Remorse | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=136, spu_reverb=0 |
| Crusaders of Might and Magic | 310 | 1.4 | ⚠4 | 1.6 | spu_reverb=0 |
| Crypt Killer | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| CT Special Forces | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| CT Special Forces 2 | 310 | 1.4 | ⚠4 | 1.9 | — |
| CT Special Forces 3 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Cu-On-Pa | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Cubix Robots - Race 'n Robots | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Cyber Org | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Cyberbots - Fullmetal Madness | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Cyberia | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Cybernetic Empire (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Cybernetic Empire (Disc 2) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| CyberSled | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| CyberSpeed | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, soundtimer=0 |
| CyberTiger | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Cyberwar (Disc 1) | 310 | 1.4 | 1.12 | 1.6 | psxfix_Sio=1, psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0, biosMode=0 |
| Cyborg 009 - Block Kuzushi | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Cyborg Kuro-chan Returns | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |

#### D（109）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| D (Disc 1) | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| D (Disc 2) | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| D (Disc 3) | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Dakar 97 | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| DamDam Stompland | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Dance Dance Revolution | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Dangan | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128 |
| Danger Girl | 310 | 1.4 | ⚠3 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Darius Gaiden | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=24, frameskip=1, framelimit=0, biosMode=0, spu_reverb=0 |
| Dark Hunter - Strange Dimension School | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Dark Hunter 2 - Demon Forest | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Dark Tales - From the Lost Soul | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Darklight Conflict | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Darkstalkers | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Darkstalkers 3 | 306 | 1.4 | ⚠5 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Darkstalkers 3 (Uncensored) | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Darkstone | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Dave Mirra BMX - Maximum Remix | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| David Beckham Soccer | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, frameskip=1, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Davis Cup Complete Tennis | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| Dawn of the Dead (Disc 1) | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_reverb=0 |
| DBZ - Dead Ball Zone | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Dead in the Water | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192, framelimit=0, controller=2, rumble_enabled=1 |
| Dead or Alive | 310 | 1.4 | ⚠5 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Deadheat Road | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=192, FixFF=0 |
| Death Wing | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, framelimit=0 |
| Deathtrap Dungeon | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, rumble_enabled=1 |
| Deception | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0 |
| Deception II | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Deception III - Dark Delusion | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=88, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Deep Freeze | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0 |
| Defcon 5 - Peace Has a Price | 310 | 1.4 | ⚠3 | 1.9 | vandalFix=1, graphicsFixes=256, frameskip=1 |
| Delta Force - Urban Warfare | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Delta Force Urban Warfare | 306 | 1.4 | ⚠4 | ⚠2 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Demolition Racer | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, spu_interpolation=0, FixFF=0, controller=2, rumble_enabled=1 |
| Descent | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, soundtimer=0 |
| Descent Maximum | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=128, frameskip=1, spu_reverb=0, rumble_enabled=1 |
| Destrega | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Destruction Derby | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Destruction Derby 2 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Destruction Derby Raw | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, rumble_enabled=1 |
| Destructo 2 | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Detana Twinbee Yahoo! | 310 | 1.4 | ⚠4 | ⚠2 | psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0, biosMode=0, psxfix_SpuIrq2=1 |
| Devil Dice | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Devil Man | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Dexter's Laboratory - Mandark's Lab | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Dezaemon Kids! | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=24, frameskip=1, spu_reverb=0 |
| Dezaemon Kids! - Select 100 | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=24, frameskip=1, spu_reverb=0 |
| Dezaemon Plus | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=24, frameskip=1 |
| Diablo | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=192, spu_reverb=0 |
| Die Hard Trilogy | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=152, framelimit=0, spu_reverb=0 |
| Die Hard Trilogy 2 | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Digimon Rumble Arena | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=136, spu_reverb=0, controller=2, rumble_enabled=1 |
| Digimon World | 310 | 1.4 | ⚠3 | ⚠2 | frameskip=1 |
| Digimon World - Maeson | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128 |
| Digimon World 2 | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128 |
| Digimon World 2 - Alternative | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128 |
| Digimon World 2 - Enhanced | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128 |
| Digimon World 3 | 306 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Dino Crisis | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_reverb=0 |
| Dino Crisis 2 | 306 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=200, spu_reverb=0, controller=2, rumble_enabled=1 |
| Dinosaur | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Dirt Jockey | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Discworld | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, soundtimer=0 |
| Discworld II | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, soundtimer=0 |
| Discworld Noir | 310 | 1.4 | 1.16 | 1.9 | psxfix_Cpu=1, psxfix_SpuIrq=1, graphicsFixes=152, ff9=1, psxfix_SpuIrq2=1, spu_reverb=0, spu_dbuf=1 |
| Disruptor | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Diver's Dream | 310 | 1.4 | 1.12 | ⚠3 | frameskip=1, framelimit=0, spu_reverb=2, soundtimer=0 |
| Divide - Enemies Within | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192 |
| Dodgem Arena | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| DoDonPachi | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, soundtimer=0, rumble_enabled=1 |
| Donald Duck - Quack Attack | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| DonPachi | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, rumble_enabled=1 |
| Doom | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0 |
| Doraemon | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Doraemon - Nobita to Fukkatsu | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Doraemon 2 - SOS! Otogi no Kuni | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Double Dragon | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Downhill Snow | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Dr. Slump | 310 | 1.4 | ⚠4 | 1.9 | soundtimer=0, controller=2 |
| Dracula - The Last Sanctuary (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Dracula - The Last Sanctuary (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Dracula - The Resurrection (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Dracula - The Resurrection (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Dragon Ball - Final Bout | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, framelimit=0 |
| Dragon Ball Z - The Legend | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| Dragon Ball Z - Ultimate Battle 22 | 306 | 1.4 | ⚠5 | ⚠3 | graphicsFixes=192, FixFF=0 |
| Dragon Seeds | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Dragon Tales - Dragonseek | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Dragon Valor (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Dragon Valor (Disc 2) | 306 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Dragon Warrior VII (Disc 1) | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Dragon Warrior VII (Disc 2) | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Dragonbeat - Legend of Pinball | 310 | 1.4 | ⚠5 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| DragonHeart - Fire & Steel | 306 | 1.4 | ⚠4 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Dragstars | 310 | 1.4 | 1.12 | 1.6 | framelimit=0, spu_reverb=0, FixFF=0 |
| Driver | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Driver (Prototype) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_usexa=0, spu_reverb=0, controller=2 |
| Driver 2 (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Driver 2 (Disc 2) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, framelimit=0, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Duke Nukem | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Duke Nukem - Land of the Babes | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Duke Nukem - Planet of the Babes | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=200, spu_reverb=0, controller=2, rumble_enabled=1 |
| Duke Nukem - Time to Kill | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, memcardnum1=1, memcardnum2=2, spu_reverb=0, controller=2, rumble_enabled=1 |
| Duke Nukem - Time to Kill (Prototype) | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Duke Nukem - Total Meltdown | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Dukes of Hazzard | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=152, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Dune 2000 | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=192, spu_reverb=0, controller=2, rumble_enabled=1 |
| Dynasty Warriors | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=24, frameskip=1, framelimit=0, spu_reverb=0 |

#### E（51）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| E.T. - Interplanetary Mission | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Eagle One - Harrier Attack | 306 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, spu_interpolation=1, FixFF=0 |
| Earthworm Jim 2 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, rumble_enabled=1 |
| Echo Night | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, rumble_enabled=1 |
| Echo Night 2 | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Ecsaform (Disc 1) | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Ecsaform (Disc 2) | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| ECW Anarchy Rulz | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, multitap=1, controller=2, rumble_enabled=1 |
| ECW Hardcore Revolution | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, multitap=1, controller=2, rumble_enabled=1 |
| Eggs of Steel | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128 |
| Egypt - Tomb of Pharaoh | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, FixFF=0 |
| Egypt II - Heliopolis Prophecy | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, FixFF=0 |
| Einhander | 310 | 1.4 | 1.15 | ⚠3 | psxfix_Cdda=1, psxfix_SpuIrq=1, graphicsFixes=216, frameskip=1, psxfix_SpuIrq2=1, spu_dbuf=1 |
| Elemental Gearbolt | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| Elemental Pinball | 306 | 1.4 | ⚠3 | 1.6 | frameskip=1 |
| Eliminator | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Emit Value Pack (Vol. 1) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, biosMode=0 |
| Engacho! | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| EOS - Edge of Skyhigh | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Epidemic | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Equestriad 2001 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Equestrian Showcase | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| ESPN Extreme Games | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| ESPN Extreme Games 2 | 310 | 1.4 | ⚠4 | ⚠2 | graphicsFixes=128, rumble_enabled=1 |
| ESPN Extreme Games 3 | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| ESPN MLS Gamenight | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Eternal Eyes | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Europe Racer | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| European Super League | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Everybody's Golf | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Everybody's Golf 2 | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, controller=2, rumble_enabled=1 |
| Evil Dead - Hail to the King (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Evil Dead - Hail to the King (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Evil Zone | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Evo's Space Adventures | 306 | 1.4 | 1.12 | ⚠3 | spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Exalegiuse | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128 |
| Excalibur 2555 A.D | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128 |
| Excalibur 2555 A.D. | 270 | 1.4 | ⚠3 | 1.6 | frameskip=1 |
| Exector | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Exhumed | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, rumble_enabled=1 |
| Exodus Guilty | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Expendable | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Expert | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128 |
| Explosive Racing | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, FixFF=0, soundtimer=0 |
| Extra Bright | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=72, frameskip=1, spu_reverb=0 |
| Extreme 500 | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, spu_interpolation=1 |
| Extreme Ghostbusters | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Extreme Go-Kart Racing | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Extreme Pinball | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0 |
| Extreme Power | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Extreme Snow Break | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |

#### F（100）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Fade to Black | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Family Diamond | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Family Feud | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, psxfix_SpuIrq=1, graphicsFixes=24, frameskip=1, controller=2, rumble_enabled=1 |
| Family Game Pack | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Fantastic Four | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, multitap=1 |
| Fantastic Night Dreams - Cotton | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Fantastic Pinball | 310 | 1.4 | ⚠5 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, soundtimer=0 |
| Fatal Fury | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=2, soundtimer=0 |
| Fatal Fury - Wild Ambition | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=64, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Fear Effect (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fear Effect (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fear Effect (Disc 3) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fear Effect (Disc 4) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fear Effect (Prototype) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192, FixFF=0, controller=2, rumble_enabled=1 |
| Fear Effect 2 - Retro Helix (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fear Effect 2 - Retro Helix (Disc 2) | 310 | 1.4 | 1.12 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fear Effect 2 - Retro Helix (Disc 3) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fear Effect 2 - Retro Helix (Disc 4) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Felony 11-79 | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| FF Chronicles - Chrono Trigger | 266 | 1.4 | ⚠3 | ⚠2 | framelimit=0, spu_reverb=2 |
| FIFA 1996 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| FIFA 1997 | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| FIFA 1998 | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| FIFA 1998 - World Cup | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0 |
| FIFA 2002 - World Cup | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| FIFA 96 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| FIFA World Cup 1998 | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=200, framelimit=0, spu_reverb=0, spu_interpolation=1 |
| Fifth Element | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Fighter Maker | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0 |
| Fighters' Impact | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, spu_interpolation=0 |
| Fighting Force | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0 |
| Fighting Force 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Fighting Network Rings | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Final Doom | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, rumble_enabled=1 |
| Final Fantasy IV | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=192, FixFF=0 |
| Final Fantasy IX (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Final Fantasy IX (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Final Fantasy IX (Disc 3) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Final Fantasy IX (Disc 4) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Final Fantasy Origins - FF | 306 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Final Fantasy Origins - FF II | 306 | 1.4 | 1.15 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Final Fantasy Tactics | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, ff9=1 |
| Final Fantasy Tactics - Prime | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128 |
| Final Fantasy Tactics - TLW | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, ff9=1, FixFF=0 |
| Final Fantasy V | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Final Fantasy VI | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Final Fantasy VII (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192 |
| Final Fantasy VII (Disc 2) | 306 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192 |
| Final Fantasy VII (Disc 3) | 306 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192 |
| Final Fantasy VIII (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=72, frameskip=1, controller=2, rumble_enabled=1 |
| Final Fantasy VIII (Disc 2) | 306 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Final Fantasy VIII (Disc 3) | 306 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Final Fantasy VIII (Disc 4) | 306 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Finger Flashing | 306 | 1.4 | 1.12 | 1.6 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Fire Pro Wrestling - Iron Slam '96 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Fire Pro Wrestling G | 310 | 1.4 | 1.12 | 1.9 | frameskip=1 |
| Firebugs | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Firestorm - Thunderhawk 2 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Firo & Klawd | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128 |
| Fisherman's Bait | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fisherman's Bait 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Fist | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Fist of The North Star | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Floating Runner | 306 | 1.4 | ⚠4 | 1.9 | FixFF=0, soundtimer=0 |
| Fluid | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| Flying Squadron | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Football Madness | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Football Manager 2000 | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Football Manager 2001 | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ford Racing | 310 | 1.4 | 1.12 | 1.6 | framelimit=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Ford Truck Mania | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Formula 1 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Formula 1 - 1997 CE | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Formula 1 - Arcade | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Formula 1 - CE | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Formula 1 - GP Nippon | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Formula 1 - World Grand Prix | 310 | 1.4 | 1.16 | 1.6 | psxfix_Cdda=1, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Formula Circus | 306 | 1.4 | ⚠4 | 1.9 | FixFF=0, soundtimer=0 |
| Formula Karts - Special Edition | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Formula Nippon | 310 | 1.4 | ⚠5 | 1.6 | framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Formula One | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Forsaken | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0 |
| Fox Hunt (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, framelimit=0 |
| Fox Hunt (Disc 2) | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, framelimit=0 |
| Fox Hunt (Disc 3) | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, framelimit=0 |
| Freestyle Boardin' '99 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Freestyle Motocross | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Frenzy! | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Friday Night Funkin' | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Frisky Tom | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Frogger | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=88, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Frogger 2 - Swampy's Revenge | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=24, frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Front Mission 1st | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Front Mission 2 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, frameskip=1 |
| Front Mission 3 | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, rumble_enabled=1 |
| Front Mission Alternative | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0 |
| Fun! Fun! Pingu | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=136, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Future Cop LAPD | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Future Racer | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Fuuun Gokuu Ninden | 306 | 1.4 | 1.15 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |

#### G（91）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| G-Police | 310 | 1.4 | 1.12 | ⚠2 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_reverb=0, spu_interpolation=0 |
| G-Police (Disc 1) | 310 | 1.4 | 1.16 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_reverb=0 |
| G-Police (Disc 2) | 306 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192 |
| G-Police - Weapons of Justice | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| G-Police 2 - Weapons of Justice | 306 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| G. Darius | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Gaiaseed - Project Seed Trap | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Galaga - Destination Earth | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Galaxian 3 | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, multitap=1, controller=2 |
| Galaxy Fight | 310 | 1.4 | 1.12 | 1.6 | frameskip=1 |
| Galaxy Fraulein Yuna - Final Edition | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Gale Gunner | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Galerians (Disc 1) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Galerians (Disc 2) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Galerians (Disc 3) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Gallop Racer | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Gamera 2000 | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Ganbare Goemon - Kaizoku Akogingu | 270 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Ganbare Goemon - Kuru Nara Koi! | 310 | 1.4 | 1.16 | 1.6 | psxfix_Cdda=1, graphicsFixes=72, frameskip=1, spu_reverb=0 |
| Ganbare Goemon - Oedo Daikaiten | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ganbare Goemon - Space Pirate | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Gauntlet Legends | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, multitap=1, controller=2, rumble_enabled=1 |
| Gear Fighter Dendoh | 306 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| GeGeGe no Kitarou - Gyakushuu! | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Gekido - Urban Fighters | 310 | 1.4 | ⚠4 | 1.9 | multitap=1, controller=2, rumble_enabled=1 |
| Gekioh - Shooting King | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Gekisha Boy | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Gekisou Tomarunner | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Genei Tougi - Shadow Struggle | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Geom Cube | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0, FixFF=0, soundtimer=0 |
| Geppy-X (Disc 1) | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Geppy-X (Disc 2) | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Geppy-X (Disc 3) | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Geppy-X (Disc 4) | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Gex | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Gex 2 - Enter the Gecko | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, psxfix_SpuIrq=1, graphicsFixes=8, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Gex 3 - Deep Cover Gecko | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, psxfix_SpuIrq=1, graphicsFixes=72, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Gex 3D | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Gex 3D - Enter the Gecko | 306 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Ghost in the Shell | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=192, spu_interpolation=1, soundtimer=0 |
| Ghoul Panic | 310 | 1.4 | ⚠4 | 1.6 | spu_reverb=0, controller=2, rumble_enabled=1 |
| Global Domination | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Goemon - New Generation | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Goiken Muyou II | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=8, frameskip=1, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0 |
| Gold & Glory - Road to El Dorado | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Golden Nugget (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Golden Nugget (Disc 2) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Golgo 13 (Vol. 1) | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Golgo 13 (Vol. 2) | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Golgo 13 Vol. 1 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Golgo 13 Vol. 2 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Goofy's Fun House | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Gradius Deluxe Pack | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Gradius Gaiden | 310 | 1.4 | ⚠4 | 1.9 | FixFF=0 |
| Gran Turismo | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=216, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Gran Turismo 2 (Arcade Mode) | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Grand Slam | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Grand Theft Auto | 310 | 1.4 | 1.15 | 1.9 | framelimit=0 |
| Grand Theft Auto - London | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Grand Theft Auto 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=152, spu_reverb=0, controller=2 |
| Grand Theft Auto London | 270 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, spu_reverb=2, controller=2, rumble_enabled=1 |
| Grand Tour Racing '98 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Grandia (Disc 1) | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Grandia (Disc 2) | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Grandia ReDux (Disc 1) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Granstream Saga | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Grid Run | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Grind Session | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Groove Jigoku V - SweepStation | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0 |
| Grudge Warriors | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| GT - Straight Victory | 310 | 1.4 | ⚠4 | ⚠3 | framelimit=0, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| GT Kai - All Japan Touring Car | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0 |
| Guardian of Darkness | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Guardian's Crusade | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Gubble | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Guilty Gear | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, frameskip=1, framelimit=0, FixFF=0, soundtimer=0 |
| Gunbare! Game Tengoku - GP 2 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=16, frameskip=1, spu_reverb=0 |
| Gundam 0079 - War for Earth (Disc 1) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Gundam 0079 - War for Earth (Disc 2) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Gundam Battle Assault | 310 | 1.4 | 1.15 | ⚠2 | graphicsFixes=128, spu_reverb=2 |
| Gundam Battle Assault 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Gundress | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Gunfighter - Jesse James | 306 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Gunfighter - Legend of Jesse James | 306 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=72, frameskip=1, spu_reverb=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Gungage | 306 | 1.4 | ⚠5 | 1.9 | graphicsFixes=192, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Gunners Heaven | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Gunnm - Martian Memory | 310 | 1.4 | ⚠3 | 1.6 | vandalFix=1, psxfix_Cdda=1, graphicsFixes=8, frameskip=1, controller=2, rumble_enabled=1 |
| Gunpey | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Gunship | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128 |
| Guntu - Western Front 1944 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Gussun Oyoyo | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2 |

#### H（41）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Hakaiou - King of Crusher | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Happy Jogging in Hawaii | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Hard Boiled | 310 | 1.4 | ⚠4 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=24, frameskip=1, framelimit=0 |
| Hard Edge | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, controller=2, rumble_enabled=1 |
| Hard Rock Cab | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| HardBall 5 | 310 | 1.4 | 1.16 | 1.9 | psxfix_SpuIrq=1, graphicsFixes=128, psxfix_SpuIrq2=1 |
| Hardcore 4x4 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Harmful Park | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0 |
| Harry Potter - Chamber of Secrets | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Harry Potter - Philosopher's Stone | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Harvest Moon - Back to Nature | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Hashiriya | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0 |
| Heart of Darkness (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0 |
| Heart of Darkness (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0 |
| Hebereke's Popoitto | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Hellboy - Asylum Seeker | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Hellnight | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Hello Kitty - Cube Frenzy | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Herc's Adventures | 306 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0 |
| Hercules | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| Hermie Hopperhead - Scrap Panic | 310 | 1.4 | ⚠4 | 1.9 | psxfix_SpuIrq=1, graphicsFixes=64, frameskip=1, spu_reverb=0 |
| Hexen | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0 |
| Hi-Octane | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0 |
| Hidden & Dangerous | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Hit Back | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Hogs of War | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Hooters - Road Trip | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Hoshigami | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Hot Wheels - Extreme Racing | 310 | 1.4 | 1.12 | 1.6 | framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Hot Wheels - Turbo Racing | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Hugo | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Hugo - Black Diamond Fever | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Hyakujuu Sentai GaoRanger | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Hybrid | 306 | 1.4 | ⚠4 | 1.9 | spu_reverb=2, FixFF=0, soundtimer=0 |
| Hydro Thunder | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Hyper Crazy Climber | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Hyper Final Match Tennis | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| Hyper Speed GranDoll | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Hyper Speed GranDoll (Disc 1) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Hyper Speed GranDoll (Disc 2) | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128 |
| Hyper-Rally | 310 | 1.4 | 1.12 | 1.6 | framelimit=0, spu_reverb=0, FixFF=0 |

#### I（31）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Iblard - City of Hatching Laputa | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0 |
| IHRA Drag Racing | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, framelimit=0, FixFF=0 |
| ImageFight & XMultiply | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, framelimit=0, spu_interpolation=1 |
| Impact Racing | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| In Cold Blood (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| In Cold Blood (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| In the Hunt | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Incredible Crisis | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Incredible Hulk - The Pantheon Saga | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Independence Day | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0 |
| Infestation | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0 |
| Initial D | 310 | 1.4 | ⚠5 | ⚠3 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Inspector Gadget - Crazy Maze | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Intellivision Classic Games | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| International Karate Plus | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=2 |
| International Rally Racing (Prototype) | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| International Track & Field | 306 | 1.4 | 1.12 | 1.6 | graphicsFixes=64, frameskip=1, FixFF=0, multitap=1 |
| Inuyasha - A Feudal Fairy Tale | 306 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Invasion | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Invasion from Beyond | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Irem Arcade Classics | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Iron Man & X-O Manowar | 306 | 1.4 | 1.12 | 1.6 | spu_reverb=2 |
| Iron Soldier 3 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Irritating Stick | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=2, controller=2, rumble_enabled=1 |
| iS - Internal Section | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| ISS Deluxe | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| ISS Pro 1998 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| ISS Pro Evolution | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| ISS Pro Evolution 2 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Italian Job | 310 | 1.4 | ⚠5 | 1.6 | framelimit=0, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Iznogoud | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |

#### J（36）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| J's Racin' | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0 |
| Jackie Chan Stuntmaster | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=16, frameskip=1, framelimit=0, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Jade Cocoon | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Jaleco Collection Vol. 1 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| James Pond II | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Jarrett & Labonte Racing | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, spu_interpolation=1, soundtimer=0, controller=2, rumble_enabled=1 |
| Jeremy McGrath Supercross 2000 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Jeremy McGrath Supercross 98 | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128 |
| Jersey Devil | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=200 |
| Jet Ace | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Jet Moto | 270 | 1.4 | 1.12 | 1.6 | spu_reverb=2 |
| Jet Moto 2 - Championship Edition | 270 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, controller=2 |
| Jet Moto 2124 (Prototype) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Jet Rider | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=64, frameskip=1, framelimit=0, spu_reverb=2 |
| Jet Rider 2 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0 |
| Jet Rider 3 (Prototype) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Jewels of the Oracle | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| JGTC - All Japan Grand Touring Car | 306 | 1.4 | ⚠4 | 1.9 | framelimit=0, FixFF=0 |
| Jigsaw Madness | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Jikkyou Oshaberi Parodius | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_usexa=0, spu_reverb=0, soundtimer=0 |
| Jimmy White's 2 - Cueball | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Jinx | 306 | 1.4 | ⚠3 | ⚠2 | psxfix_Cdda=1, frameskip=1 |
| Johnny Bazookatone | 306 | 1.4 | 1.15 | ⚠2 | graphicsFixes=128, spu_reverb=2 |
| JoJo's Bizarre Adventure | 306 | 1.4 | 1.15 | 1.9 | graphicsFixes=8, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Jonah Lomu Rugby | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| Judge Dredd | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=200, spu_reverb=0, rumble_enabled=1 |
| Juggernaut (Disc 1) | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Juggernaut (Disc 2) | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Juggernaut (Disc 3) | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Jumping Flash! | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| Jumping Flash! 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| Jumping Flash! 2 - Rap-la-MuuMuu | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Jumping Flash! 3 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Jupiter Strike | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Jurassic Park - The Lost World | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Jurassic Park - Warpath | 306 | 1.4 | 1.12 | ⚠3 | psxfix_Cdda=1, graphicsFixes=72, framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |

#### K（63）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| K-1 Arena Fighters | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| K-1 Grand Prix | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| K-1 Revenge | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, FixFF=0 |
| Kagaku Ninja tai Gatchaman | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=72, frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Kahen Soukou Gunbike | 306 | 1.4 | 1.16 | 1.6 | frameskip=1, FixFF=0 |
| Kaisoku Tenshi - The Rapid Angel | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Kakuge Yarou - Fighting Creator | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Kamen Rider | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kamen Rider - The Bike Race | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kamen Rider Agito | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kamen Rider Kuga | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kamen Rider Ryuki | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kamen Rider V3 | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kattobi Tune | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Kensei - Sacred Fist | 306 | 1.4 | ⚠5 | ⚠3 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Kero Kero King | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kick Off World | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Kickboxing | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Kickboxing Knockout | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, framelimit=0, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Kid Clown - Crazy Chase 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Kidou Butouden G Gundam - The Battle | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Kileak | 270 | 1.4 | ⚠3 | ⚠2 | spu_reverb=2 |
| Kileak - The Blood | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, FixFF=0 |
| Kileak - The DNA Imperative | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Killer Loop | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Killing Time (Prototype) | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, spu_reverb=2 |
| Killing Zone | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| King of Bowling 2 | 310 | 1.4 | ⚠5 | 1.6 | spu_reverb=0 |
| King of Fighters 1995 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=0 |
| King of Fighters 95 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| King of Fighters 96 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, soundtimer=0 |
| King of Fighters 97 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, framelimit=0 |
| King's Field | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=0, rumble_enabled=1 |
| King's Field II | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=128, spu_reverb=0, rumble_enabled=1 |
| King's Field III | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, FixFF=0, soundtimer=0 |
| Kingsley's Adventure | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Kirikou | 306 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| KISS Pinball | 310 | 1.4 | 1.16 | 1.6 | spu_reverb=0, controller=2, rumble_enabled=1 |
| Kitchen Panic | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| KKND Krossfire | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Klaymen Gun-Hockey | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Klaymen Klaymen | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, FixFF=0, soundtimer=0, rumble_enabled=1 |
| Klonoa - Beach Volleyball | 306 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, spu_interpolation=1, FixFF=0, multitap=1, controller=2 |
| Klonoa - Door to Phantomile | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=24, frameskip=1, spu_reverb=0 |
| Knockout Kings | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0 |
| Konami Arcade Classics | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Konami MSX Collection (Vol. 1) | 310 | 1.4 | 1.16 | 1.6 | frameskip=1 |
| Konami MSX Collection (Vol. 2) | 310 | 1.4 | 1.16 | 1.6 | frameskip=1 |
| Konami MSX Collection (Vol. 3) | 310 | 1.4 | 1.16 | 1.6 | frameskip=1 |
| Konami Open Golf | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| KoroKoro Post nin | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Kotobuki Grand Prix | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Koudelka (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | psxfix_SpuIrq=1, graphicsFixes=200, framelimit=0, biosMode=0, spu_reverb=0, controller=2 |
| Koutetsu Reiiki - Steeldom | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0 |
| Kowai-Shashin | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Krazy Ivan | 310 | 1.4 | ⚠5 | 1.6 | psxfix_SpuIrq=1, frameskip=1, biosMode=0, psxfix_SpuIrq2=1, spu_reverb=0, spu_dbuf=1 |
| Kula World | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, frameskip=1, spu_reverb=2, controller=2, rumble_enabled=1 |
| Kurt Warner's Arena Football Unleashed | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Kurushi Final | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Kyoro-chan no Prikura Daisakusen | 310 | 1.4 | ⚠4 | ⚠3 | spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Kyoutei Wars - Makuru 6 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Kyuiin | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, spu_reverb=0 |
| Kyuutenkai | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Sio=1, psxfix_SpuIrq=1, graphicsFixes=152, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, spu_dbuf=1 |

#### L（53）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Land Maker | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Langrisser IV | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=200, spu_reverb=0, controller=2, rumble_enabled=1 |
| Largo Winch - Commando SAR | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Lattice - 200EC7 | 306 | 1.4 | ⚠3 | 1.9 | spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Le Mans 24 Hours | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Legacy of Kain - Blood Omen | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Legacy of Kain - Soul Reaver | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=136, spu_reverb=0, controller=2, rumble_enabled=1 |
| Legacy of Kain - Soul Reaver (Alpha) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Legacy of Kain - Soul Reaver uS | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=208, spu_reverb=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Legacy of Kain Blood Omen | 270 | 1.4 | ⚠3 | ⚠2 | framelimit=0, rumble_enabled=1 |
| Legacy of Kain Soul Reaver | 270 | 1.4 | 1.16 | 1.9 | psxfix_Cdda=1, frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Legend | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0 |
| Legend of Dragoon (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Legend of Dragoon (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Legend of Dragoon (Disc 3) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Legend of Dragoon (Disc 4) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Legend of Kartia | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Legend of Legaia | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Legend of Mana | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| LEGO Island 2 | 310 | 1.4 | 1.12 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| LEGO Racers | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| LEGO Rock Raiders | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Lemmings | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Lemmings & Oh No! More Lemmings | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, soundtimer=0, controller=2, rumble_enabled=1 |
| Lethal Enforcers I & II | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| LiberoGrande | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Lightning Legend | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Little Big Adventure | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_SpuIrq=1, frameskip=1, psxfix_SpuIrq2=1 |
| Live Wire! | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, multitap=1, controller=2, rumble_enabled=1 |
| LMA Manager | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Loaded | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| Lode Runner | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| London Racer | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| London Racer II | 306 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Lone Soldier | 310 | 1.4 | ⚠5 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Lone Soldier (DE-Cyborg) | 310 | 1.4 | ⚠5 | 1.9 | frameskip=1, soundtimer=0 |
| Looney Tunes Racing | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Lost Vikings 2 - Norse by Norsewest | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, soundtimer=0 |
| Louvre - The Final Curse (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Louvre - The Final Curse (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Love Truck 2 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| LSD - Dream Emulator | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Lucifer Ring | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Lucky Luke | 310 | 1.4 | ⚠4 | 1.9 | FixFF=0, soundtimer=0 |
| Lucky Luke - Western Fever | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=64, frameskip=1, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Lunar - Silver Star (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=132, spu_reverb=0 |
| Lunar - Silver Star (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=132, spu_reverb=0 |
| Lunar 2 - Eternal Blue (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=132, spu_reverb=0, controller=2, rumble_enabled=1 |
| Lunar 2 - Eternal Blue (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=132, spu_reverb=0, controller=2, rumble_enabled=1 |
| Lunar 2 - Eternal Blue (Disc 3) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=132, spu_reverb=0, controller=2, rumble_enabled=1 |
| Lunatik (Prototype) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Lupin the Third | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Lupupu Cube - Lup Salad | 310 | 1.4 | 1.12 | 1.9 | frameskip=1 |

#### M（143）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| M&M's - Shell Shocked | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Machine Hunter | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, framelimit=0 |
| Macross (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, controller=2, rumble_enabled=1 |
| Macross (Disc 2) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Macross - Digital Mission VF-X | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Macross Plus - Game Edition | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Macross VF-X2 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Mad Panic Coaster | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Mad Stalker | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Madden NFL 97 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Magic Beast Warriors | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0 |
| Magic Carpet | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0 |
| Magic Castle | 310 | 1.4 | 1.12 | 1.9 | frameskip=1 |
| Magic The Gathering - BM | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Magical Drop | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Magical Drop F | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Magical Drop III (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0 |
| Magical Drop III (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Magical Hoppers | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Magical Tetris Challenge | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, soundtimer=0, controller=2, rumble_enabled=1 |
| Makeruna! Makendou 2 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Marble Master | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Marchen Adventure - Cotton 100% | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Martian Gothic | 270 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2 |
| Martian Gothic - Unification | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Marvel Super Heroes | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0 |
| Marvel Super Heroes vs. Street Fighter | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0 |
| Marvel vs. Capcom | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0 |
| Mary King's Riding Star | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Mass Destruction | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0 |
| Master of Monsters | 270 | 1.4 | ⚠3 | ⚠2 | frameskip=1 |
| Master of Monsters - Disciples of Gaia | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Masters | 310 | 1.4 | ⚠4 | 1.9 | psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1 |
| Mat Hoffman's Pro BMX | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Max Power Racing | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Maximum Force | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, rumble_enabled=1 |
| MaxRacer | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, soundtimer=0 |
| MDK | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| MechWarrior 2 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Medal of Honor | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=64, framelimit=0, controller=2, rumble_enabled=1, rumble=20 |
| Medal of Honor (Beta) | 306 | 1.4 | ⚠4 | 1.9 | framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Medal of Honor - Underground | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=64, framelimit=0, controller=2, rumble_enabled=1, rumble=20 |
| Medarot R | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=64, frameskip=1, spu_reverb=0 |
| MediEvil | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_reverb=0, controller=2 |
| MediEvil 2 | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=64, frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Mega Man - Battle & Chase | 270 | 1.4 | ⚠3 | ⚠2 | frameskip=1, spu_reverb=2, soundtimer=0 |
| Mega Man 8 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Mega Man Legends | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Mega Man Legends 2 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Mega Man X3 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Mega Man X3 (Enhanced) | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, FixFF=0 |
| Mega Man X4 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Mega Man X5 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Mega Man X5 (Enhanced) | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Mega Man X6 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Mega Man X6 (Enhanced) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Megaman - Battle & Chase | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, FixFF=0, soundtimer=0 |
| Megatudo 2096 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Meisha Retsuden - Greatest 70's | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Men in Black | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Men in Black - Crashdown | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=200, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Meta-Ph-List Mu.X.2297 (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Meta-Ph-List Mu.X.2297 (Disc 2) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Metal Gear Solid (Disc 1) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, controller=2, rumble_enabled=1 |
| Metal Gear Solid (Disc 2) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, controller=2, rumble_enabled=1 |
| Metal Gear Solid - Special Missions | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Metal Gear Solid - VR Missions | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, controller=2, rumble_enabled=1 |
| Metal Jacket | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Metal Slug | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| Metal Slug X | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0 |
| Mezase! Senkyuu-ou | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0 |
| MGG - Manic Game Girl | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Michael Owen's World League 1999 | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Michael Schumacher - WRK 2002 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Mickey's Wild Adventure | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Micro Machines V3 | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, multitap=1 |
| Micro Maniacs | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Midnight in Vegas | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Midnight Run - Road Fighter 2 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Mighty Hits Special | 306 | 1.4 | 1.12 | 1.6 | framelimit=0, FixFF=0 |
| Mille Miglia | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, soundtimer=0, controller=2, rumble_enabled=1 |
| Mini Bakusou Kyoudai - WGP Hyper Heat | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Minnya de Ghost Hunter | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Misadventures of Tron Bonne | 310 | 1.4 | 1.16 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=208, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Missile Command | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Mission Impossible | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=216, spu_interpolation=1 |
| Missland | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Missland 2 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Mizzurna Falls | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Mobile Armor | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Mobile Light Force | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2 |
| Mobile Police Patlabor | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Mobile Police Patlabor (Taikenban 1.0) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Mobile Police Patlabor (Taikenban 2.0) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Mobile Suit Gundam - C's Counterattack | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Mobile Suit Gundam - POYW | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, soundtimer=0 |
| Mobile Suit Gundam - Version 2.0 LTD | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128 |
| MoHo | 306 | 1.4 | 1.12 | ⚠3 | spu_interpolation=1, controller=2, rumble_enabled=1 |
| Monaco Grand Prix | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Monkey Magic | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=24, frameskip=1, framelimit=0, spu_reverb=0 |
| Monster Bass | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Monster Racer | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Monster Rancher | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Monster Rancher 2 | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Monster Rancher Hop-A-Bout | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Monster Trucks | 310 | 1.4 | ⚠4 | ⚠2 | psxfix_Cdda=1, psxfix_SpuIrq=1, frameskip=1, psxfix_SpuIrq2=1 |
| Monsters Inc. - Scream Team | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| MonsterSeed | 306 | 1.4 | ⚠3 | 1.6 | frameskip=1, spu_reverb=2 |
| Moon | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Moon Cresta | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Moorhen 3 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Moorhuhn 2 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Moorhuhn Kart | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Moorhuhn X | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Mort the Chicken | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Mortal Kombat - Special Forces | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Mortal Kombat 2 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Mortal Kombat 3 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Mortal Kombat 4 | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, soundtimer=0 |
| Mortal Kombat Mythologies | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Mortal Kombat Mythologies - Sub-Zero | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Mortal Kombat Trilogy - TE | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Mortal Kombat Trilogy_ | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, soundtimer=0 |
| Moto Racer | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Moto Racer 2 | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Moto Racer World Tour | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Motocross Mania | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Motor Mash | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0, multitap=1 |
| Motor Toon Grand Prix | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Motorhead | 306 | 1.4 | ⚠5 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Mr. Driller | 310 | 1.4 | ⚠4 | ⚠2 | psxfix_Cdda=1, frameskip=1, framelimit=0, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Mr. Driller G | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, framelimit=0, spu_reverb=0 |
| Mr. Prospector | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Ms. Pac-Man - Maze Madness | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| MTB Dirt Cross | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| MTV Sports - Pure Ride | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| MTV Sports - Skateboarding | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| MTV Sports - Snowboarding | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Muppet Monster Adventure | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=88, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Music | 310 | 1.4 | ⚠4 | 1.9 | — |
| Music 2000 | 306 | 1.4 | 1.12 | 1.9 | controller=2 |
| Myst | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Mystic Dragoons | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |

#### N（69）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| N-Gen Racing | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=72, framelimit=0, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| N2O - Nitrous Oxide | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Nagano Winter Olympics 1998 | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0 |
| Namco Anthology 1 | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0 |
| Namco Anthology 2 | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0 |
| Namco Museum Vol. 1 | 310 | 1.4 | 1.16 | 1.9 | framelimit=0, soundtimer=0 |
| Namco Museum Vol. 2 | 310 | 1.4 | 1.16 | 1.9 | framelimit=0, soundtimer=0 |
| Namco Museum Vol. 3 | 310 | 1.4 | 1.16 | 1.9 | framelimit=0, soundtimer=0 |
| Namco Museum Vol. 4 | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Namco Museum Vol. 5 | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, soundtimer=0 |
| Namco Museum Vol. 6 - Encore | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, soundtimer=0 |
| Naniwa Wangan Battle | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Nanotek Warrior | 310 | 1.4 | 1.12 | 1.6 | framelimit=0, spu_reverb=2, soundtimer=0 |
| NASCAR 2000 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| NASCAR 2001 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| NASCAR 98 - Collector's Edition | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| NASCAR 99 - Legacy (CE) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| NASCAR Heat | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| NASCAR Racing | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| NASCAR Rumble | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=16, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| NBA Hangtime | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=1, FixFF=0, multitap=1 |
| NBA Hoopz | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| NBA Jam - Extreme | 306 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, multitap=1 |
| NBA Jam - Tournament Edition | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, multitap=1 |
| NBA Jam - Tournament Edition '21 | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, multitap=1 |
| NBA Jam - Tournament Edition '22 | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, multitap=1 |
| NBA Shoot Out | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=64, frameskip=1, framelimit=0, spu_reverb=0 |
| NBA Showtime - NBA on NBC | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, spu_reverb=2, FixFF=0, multitap=1, controller=2, rumble_enabled=1 |
| NCAA Basketball Final Four 97 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Necronomicon (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Necronomicon (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Nectaris - Military Madness | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Need for Speed II | 266 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, rumble_enabled=1 |
| Nekketsu Oyako | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, framelimit=0, spu_reverb=0 |
| Net Yaroze Collection | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=8, frameskip=1, spu_reverb=0 |
| Net Yaroze Collection (2007) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1 |
| Newman Haas Racing | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Next Tetris | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0 |
| NFL Blitz | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| NFL Full Contact | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| NFL GameDay | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| NFL GameDay 1997 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| NFL Xtreme | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0 |
| NHL Face Off | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1 |
| NHL Face Off 1997_ | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| NHL Face Off 1998 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0 |
| NHL Face Off 1999 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| NHL Face Off 2000 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| NHL Powerplay 1996 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| NHL Rock the Rink | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Nichibutsu Arcade Classics | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Nicktoons Racing | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Night Raid | 310 | 1.4 | 1.15 | ⚠3 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_xaspeed=1, spu_reverb=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Night Striker | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Nightmare Creatures | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Nightmare Creatures II | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Ninja - Shadow of Darkness | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0 |
| Ninja Hayate | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Ninja Jajamaru | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128 |
| Ninpuu Sentai Hurricaneger | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| No Fear - Downhill Mountain Bike | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| No Fear Downhill Mountain Biking | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| No One Can Stop Mr. Domino | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=64, soundtimer=0 |
| Notam of Wind | 310 | 1.4 | ⚠5 | 1.9 | graphicsFixes=128, framelimit=0 |
| Novastorm | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, FixFF=0 |
| Novastorm (Disc 1) | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=136, framelimit=0 |
| Novastorm (Disc 2) | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=136, framelimit=0 |
| Nuclear Strike | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0 |
| Nyan Cat | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |

#### O（28）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| O.D.T | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| O.D.T. | 270 | 1.4 | 1.16 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Oddworld - Abe's Exoddus (Disc 1) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=8, frameskip=1, soundtimer=0, controller=2, rumble_enabled=1 |
| Oddworld - Abe's Exoddus (Disc 2) | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=8, frameskip=1, soundtimer=0, controller=2, rumble_enabled=1 |
| Oddworld - Abe's Oddysee | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=8, frameskip=1, soundtimer=0 |
| Odo Odo Oddity | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| Off-World Interceptor Extreme | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| Ogre Battle - Limited Edition | 310 | 1.4 | 1.15 | ⚠2 | frameskip=1, FixFF=0 |
| Oh No! | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Ohlins Hyper-Rally | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Olympic Summer Games | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Omega Assault | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Omega Boost | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| One | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_interpolation=1, controller=2, rumble_enabled=1 |
| One Piece - Grand Battle | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=192, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| One Piece Mansion | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Onside Complete Soccer | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Open Golf - History of Turnberry | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Option - Tuning Car Battle | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Option - Tuning Car Battle 2 | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Option - Tuning Car Battle Spec R | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Option - Tuning Car Battle Spec-R | 310 | 1.4 | ⚠5 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Over Drivin' - Skyline Memorial | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, FixFF=0 |
| Over Drivin' DX - Rally Edition | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, FixFF=0 |
| OverBlood | 310 | 1.4 | 1.12 | 1.9 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, FixFF=0 |
| OverBlood 2 (Disc 1) | 306 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=72, frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| OverBlood 2 (Disc 2) | 306 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=72, frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Overboard! | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, framelimit=0 |

#### P（114）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Pac-Man World | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| PacaPaca Passion | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0 |
| Pachi-Slot Kanzen - Cranky Pro | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, soundtimer=0 |
| Pandemonium! 2 | 306 | 1.4 | 1.12 | 1.9 | graphicsFixes=136, spu_reverb=2 |
| Pandemonium!_ | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, rumble_enabled=1 |
| Panzer Bandit | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0 |
| Panzer Front | 270 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=2 |
| Panzer Front bis | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Panzer General | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Paperboy (Prototype) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Paradise Casino | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Paranoia Scape | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| PaRappa the Rapper | 306 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0 |
| Parasite Eve (Disc 1) | 310 | 1.4 | 1.12 | 1.6 | vandalFix=1, frameskip=1 |
| Parasite Eve (Disc 2) | 310 | 1.4 | 1.12 | 1.6 | vandalFix=1, frameskip=1 |
| Parasite Eve II (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | vandalFix=1, psxfix_Cdda=1, graphicsFixes=128, controller=2, rumble_enabled=1 |
| Parasite Eve II (Disc 2) | 310 | 1.4 | ⚠4 | 1.6 | vandalFix=1, psxfix_Cdda=1, frameskip=1, controller=2, rumble_enabled=1 |
| Parodius | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Pastel Muses | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Patriotic Pinball | 306 | 1.4 | ⚠5 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Pax Corpus | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| PD Ultraman Invader | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Peak Performance | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, framelimit=0, spu_reverb=0 |
| Pebble Beach no Hatou Plus | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Penny Racers | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0 |
| Pepsi-Man | 266 | 1.4 | 1.16 | ⚠2 | graphicsFixes=128, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Pepsiman | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Perfect Assassin | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Perfect Performer - Yellow Monkey | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Perfect Weapon | 310 | 1.4 | 1.12 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=216 |
| Persona | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, rumble_enabled=1 |
| Persona 2 Eternal Punishment | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Persona 2 Eternal Punishment (Bonus) | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Persona 2 Innocent Sin | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Pet in TV | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Peter Jacobsen's Golden Tee Golf | 310 | 1.4 | 1.15 | 1.6 | psxfix_SpuIrq=1, frameskip=1, psxfix_SpuIrq2=1, controller=2, rumble_enabled=1 |
| Peter Pan - Never Land | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| PGA Tour 1996 | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| PGA Tour 1997 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Philosoma | 310 | 1.4 | ⚠5 | ⚠3 | graphicsFixes=200, FixFF=0 |
| Phix - The Adventure | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Pinball Fantasies - Deluxe | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0 |
| Pinball Power | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Pink Panther - Pinkadelic Pursuit | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Pinobee | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Pipe Dreams 3D | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=72, frameskip=1, framelimit=0, spu_reverb=0 |
| Pipe Mania 3D | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=0 |
| Pitball | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192 |
| Pitfall 3D - Beyond the Jungle | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0 |
| Plane Crazy | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Planet Dob | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Planet Laika | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Planet of the Apes | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Player Manager | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Plue no Daibouken | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0 |
| PO'ed | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Pocket Fighter | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Point Blank | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Point Blank 2 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Point Blank 3 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Policenauts (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=192, framelimit=0, rumble_enabled=1 |
| Policenauts (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=192, framelimit=0, rumble_enabled=1 |
| Pong | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Pool Academy | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, soundtimer=0, controller=2, rumble_enabled=1 |
| Pool Hustler | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Pop'n Tanks! | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=1 |
| PoPoLoCrois Monogatari | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| PoPoLoCrois Monogatari II (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| PoPoLoCrois Monogatari II (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| PoPoLoCrois Monogatari II (Disc 3) | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Populous - The Beginning | 310 | 1.4 | 1.16 | 1.6 | psxfix_Cdda=1, graphicsFixes=72, frameskip=1, spu_reverb=0 |
| Porsche Challenge | 310 | 1.4 | ⚠4 | ⚠2 | psxfix_Cdda=1, graphicsFixes=24, framelimit=0, spu_reverb=0 |
| Power Instinct 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Power Move Pro Wrestling | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Power Rangers - Lightspeed Rescue | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Power Rangers - Time Force | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Power Rangers Pinball | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0 |
| Power Serve 3D Tennis | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Power Shovel | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Power Spike - Pro Beach Volleyball | 310 | 1.4 | 1.16 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, multitap=1, controller=2, rumble_enabled=1 |
| Powerpuff Girls - Chemical X-Traction | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Powerslave | 306 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, FixFF=0 |
| Poy Poy | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, multitap=1 |
| Poy Poy 2 | 306 | 1.4 | ⚠3 | ⚠3 | graphicsFixes=192, spu_interpolation=1, FixFF=0, multitap=1, controller=2 |
| Premier Manager 1998 | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, spu_reverb=0 |
| Premier Manager 1999 | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Premier Manager 2000 | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Primal Rage | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Prince Naseem Boxing | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Prism Land Story | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Pro 18 - World Tour Golf | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Pro Pinball - Big Race USA | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Pro Yakyuu Nettou Puzzle Stadium | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Professional Underground League | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0 |
| Project - Horned Owl | 306 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=192, framelimit=0 |
| Project Gaiaray | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, controller=1 |
| Project Horned Owl | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=64, frameskip=1, spu_reverb=0 |
| Project Overkill | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, rumble_enabled=1 |
| Psychic Detective (Disc 1) | 310 | 1.4 | ⚠3 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, spu_dbuf=1 |
| Psychic Force | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Psychic Force - Puzzle Taisen | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Psychic Force 2 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| PU - League of Pain | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, psxfix_SpuIrq2=1 |
| Pu-Li-Ru-La | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Puchi Carat | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Puffy - P.S. I Love You | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Punch the Monkey! | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=152, framelimit=0, spu_reverb=2, controller=2, rumble_enabled=1 |
| Punky Skunk | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Putter Golf | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Puyo Puyo - n | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0 |
| Puyo Puyo Sun | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Puyo Puyo Tsuu - Ketteiban | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0 |
| Puzzle Arena Toshinden | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| Puzznic | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |

#### Q（4）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Q-bert | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=136, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Qix 2000 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Qix Neo | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Quake II | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |

#### R（140）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| R - Rock'n Riders | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| R - Rock'n Riders (Bonus Disc) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| R-Type Delta | 306 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=128, frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| R-Types | 310 | 1.4 | 1.16 | ⚠3 | frameskip=1, spu_reverb=0 |
| Race Drivin' a Go! Go! | 310 | 1.4 | ⚠3 | 1.9 | spu_reverb=0, FixFF=0 |
| Racing Lagoon | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=88, frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Racingroovy VS | 310 | 1.4 | 1.12 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Radikal Bikers | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=136, spu_reverb=0, spu_interpolation=0, FixFF=0, controller=2, rumble_enabled=1 |
| Rage Racer | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0 |
| Rageball | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Raging Skies | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Raiden | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Raiden DX | 306 | 1.4 | 1.12 | 1.9 | framelimit=0, FixFF=0, soundtimer=0, rumble_enabled=1 |
| Raiden Project | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=8, frameskip=1, rumble_enabled=1 |
| Railroad Tycoon II | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Rakugaki Showtime | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0 |
| Rally Championship | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Rally Cross | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, multitap=1, controller=2 |
| Rally Cross 2 | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Rally de Africa | 306 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, frameskip=1, FixFF=0, controller=2 |
| Rally de Europe | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rally Masters - Race of Champions | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rampage - Through Time | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, multitap=1, controller=2, rumble_enabled=1 |
| Rampage - World Tour | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Rampage 2 - Universal Tour | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, multitap=1, controller=2, rumble_enabled=1 |
| Ranma 1l2 - Battle Renaissance | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Rap-la-MuuMuu from Jumping Flash! 2 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Rapid Racer | 310 | 1.4 | ⚠5 | ⚠3 | graphicsFixes=192, spu_reverb=2, FixFF=0, soundtimer=0 |
| Rapid Reload | 306 | 1.4 | ⚠5 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Rascal | 310 | 1.4 | ⚠5 | 1.6 | framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Rat Attack! | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=24, frameskip=1, spu_reverb=0 |
| Raven Project (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Ray Tracers | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| RayCrisis | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=64, frameskip=1, spu_interpolation=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Rayman | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, spu_reverb=2, FixFF=0, soundtimer=0 |
| Rayman 2 | 306 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, framelimit=0, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Rayman Rush | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, controller=2, rumble_enabled=1 |
| RayStorm | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, spu_reverb=0, spu_interpolation=1 |
| RC de GO! | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, biosMode=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| RC Helicopter | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| RC Revenge | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| RC Stunt Copter | 310 | 1.4 | ⚠4 | 1.6 | spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Re-Loaded | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Ready 2 Rumble Boxing | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ready 2 Rumble Boxing - Round 2 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Real Bout Fatal Fury | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, soundtimer=0 |
| Real Bout Special - Dominated Mind | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Real Robots - Final Attack | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| ReBoot | 306 | 1.4 | 1.12 | 1.6 | graphicsFixes=64, framelimit=0, spu_reverb=2, FixFF=0 |
| Red Asphalt | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Red Assault | 270 | 1.4 | 1.15 | 1.9 | frameskip=1, framelimit=0, spu_reverb=2, spu_interpolation=1 |
| Reel Fishing | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Reel Fishing II | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Remote Control Dandy | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2 |
| Renegade Racers | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rescue Copter | 306 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rescue Heroes - Molten Menace | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=136, framelimit=0, controller=2, rumble_enabled=1 |
| Rescue Shot | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Resident Evil | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Resident Evil (1995 Alpha) | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, rumble_enabled=1 |
| Resident Evil (1995 Beta) | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128 |
| Resident Evil (Director's Cut) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Resident Evil (Remaster Cut) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Resident Evil (True Director's Cut) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Resident Evil (Ultimate Cut Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Resident Evil (Ultimate Cut Disc 2) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Resident Evil - Anonymous Survivors | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, FixFF=0, controller=2, rumble_enabled=1 |
| Resident Evil - AS | 270 | 1.4 | ⚠3 | ⚠2 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Resident Evil - Battle Coliseum | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Resident Evil - Deep Freeze | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Resident Evil - Survivor | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128 |
| Resident Evil - Survivor TP | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Resident Evil - True Director's Cut | 270 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=192, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Resident Evil 0 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=136, spu_usexa=0, controller=2, rumble_enabled=1 |
| Resident Evil 2 (Beta) | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128 |
| Resident Evil 2 (Disc 1) (Leon) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Resident Evil 2 (Disc 2) (Claire) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Resident Evil 2 (Prototype - GB) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Resident Evil 2 (Prototype) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=2 |
| Resident Evil 2 (Trial Edition) | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, spu_reverb=2 |
| Resident Evil 2 - Battle Coliseum | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Resident Evil 2 - Darkness | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Resident Evil 2 - Forgotten Soldier | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Resident Evil 3 - Dark Infection | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=24, frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Resident Evil 3 - Nemesis | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Retro Arcade Pack | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, soundtimer=0 |
| Retro Force | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=64, frameskip=1 |
| Return Fire | 306 | 1.4 | ⚠4 | 1.6 | spu_reverb=2 |
| Reverthion | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0, soundtimer=0 |
| Revolution X | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Rhapsody | 310 | 1.4 | ⚠5 | 1.9 | psxfix_Cdda=1, psxfix_SpuIrq=1, graphicsFixes=8, frameskip=1, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ridegear Guybrave | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Ridge Racer | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0 |
| Ridge Racer (Turbo Mode Disc) | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ridge Racer Revolution | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0 |
| Ridge Racer Type 4 | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Riot | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, framelimit=0 |
| Rise 2 - Resurrection | 310 | 1.4 | ⚠5 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Rising Zan | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Risk | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Rival Schools (Arcade Disc) | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rival Schools (Evolution Disc) | 306 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Riven (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=144, biosMode=0 |
| Riven (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=144, biosMode=0 |
| Riven (Disc 3) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=144, biosMode=0 |
| Riven (Disc 4) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=144, biosMode=0 |
| Riven (Disc 5) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=144, biosMode=0, soundtimer=0 |
| RMJ - Mystery Hospital (Disc 1) | 310 | 1.4 | ⚠5 | ⚠2 | frameskip=1 |
| RMJ - Mystery Hospital (Disc 2) | 310 | 1.4 | ⚠5 | ⚠2 | frameskip=1 |
| Road Blaster | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| Road Rash | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=72, psxfix_SpuIrq2=1, spu_reverb=0 |
| Road Rash - Jailbreak | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=136, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Road Rash 3D | 310 | 1.4 | 1.12 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Roadsters | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Robin Hood - The Siege | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Robo Pit | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0, soundtimer=0 |
| Robo Pit 2 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Robotron X | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0 |
| Rock & Roll Racing 2 | 270 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=1 |
| Rock 'Em Sock 'Em Robots Arena | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Rock-Climbing | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Rogue Trip - Vacation 2012 | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rollcage | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rollcage Stage II | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Romance of Three Kingdoms IV | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Romance of Three Kingdoms VI | 310 | 1.4 | 1.15 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=136 |
| Ronin Blade | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Rosco McQueen | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Rosco McQueen - Firefighter Extreme | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Roswell Conspiracies | 310 | 1.4 | ⚠4 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=8, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rox | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| RPG Maker | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Rugrats - Search for Reptar | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0 |
| Runabout 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=88, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Running High | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Running Wild | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Rurouni Kenshin | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=152, frameskip=1, spu_reverb=0 |
| Rurouni Kenshin - Ishin Gekitou-hen | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0 |
| Rush Hour | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0, biosMode=0, spu_interpolation=1, FixFF=0 |
| Rushdown | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, FixFF=0 |

#### S（206）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Saber Marionette J | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Sabrina Teenage Witch - Twitch in Time | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136, framelimit=0, controller=2, rumble_enabled=1 |
| SaGa Frontier | 306 | 1.4 | ⚠3 | 1.9 | spu_reverb=2 |
| SaGa Frontier 2 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, ff9=1, controller=2, rumble_enabled=1 |
| Sailor Moon SuperS | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Saiyuki - Journey West | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Sakigake Otokojuku - Dodgeball | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Salamander Deluxe Pack Plus | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Saltwater Sportfishing | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Samurai Deeper Kyo | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Samurai Shodown - Warriors Rage | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=24, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Samurai Shodown III - Blades of Blood | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Samurai Spirits - Amakusa Special | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Samurai Spirits - Bushidou Retsuden | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Samurai Spirits - KS Pack | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Samurai Spirits - Shinshou | 310 | 1.4 | ⚠5 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Samurai Spirits - Zankurou Musouken | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0 |
| San Francisco Rush - Extreme Racing | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Santa Claus Saves the Earth | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| SCARS | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Scooby-Doo - Cyber Chase | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, spu_reverb=0, controller=2, rumble_enabled=1 |
| Scooby-Doo - Night of 100 Frights | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| SD Gundam - Over Galaxian | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, framelimit=0, spu_reverb=2, soundtimer=0 |
| SD Gundam Eiyuuden | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Sea-Doo Hydro Cross | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| SeaBass Fishing | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Sentient | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Sentinel Returns | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Sentou Mecha Xabungle | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Sexy Parodius | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Shadow Gunner | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, soundtimer=0, controller=2, rumble_enabled=1 |
| Shadow Madness | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, FixFF=0 |
| Shadow Man | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Shadow Master | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0 |
| Shadow Tower | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Shake Kids | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0 |
| Shaolin | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_interpolation=1, soundtimer=0, controller=2, rumble_enabled=1 |
| Sheep | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Sheep Dog 'n' Wolf | 306 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Shellshock | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128 |
| Shienryuu | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1 |
| Shigeru Mizuki's Yokai Battles | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Shin Kidou Senki Gundam W - The Battle | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Shin Megami Tensei | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Shin Nihon Pro Wrestling | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=136, spu_reverb=0 |
| Shin Nihon Pro Wrestling 2 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Shin Senki Van-Gale | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Shipwreckers! | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Shockwave Assault (Disc 1) | 306 | 1.4 | 1.15 | 1.9 | graphicsFixes=192, spu_reverb=0, FixFF=0 |
| Shockwave Assault (Disc 2) | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Shooter - Space Shot | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Short Time (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Shura no Mon | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, soundtimer=0 |
| Shutokou Battle R | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, spu_reverb=0, FixFF=0 |
| Side by Side - Special 2000 | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=136, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Side Pocket 3 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Sidewinder 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Silent Bomber | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Silent Hill | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, graphicsFixes=80, framelimit=0, spu_reverb=0 |
| Silent Iron | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2 |
| Silhouette Mirage | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Silverload | 310 | 1.4 | 1.15 | 1.9 | psxfix_SpuIrq=1, graphicsFixes=152, biosMode=0, spu_reverb=0 |
| SimCity 2000 | 310 | 1.4 | ⚠5 | ⚠3 | graphicsFixes=128, FixFF=0 |
| Simpsons Wrestling | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Sitting Ducks | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Skeleton Warriors | 310 | 1.4 | ⚠3 | ⚠2 | frameskip=1 |
| Skullmonkeys | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, soundtimer=0, rumble_enabled=1 |
| Skydiving Extreme | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Slam 'n Jam '96 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Slam Dragon | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Slap Happy Rhythm Busters | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=88, framelimit=0, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Sled Storm | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, spu_interpolation=0, FixFF=0, controller=2, rumble_enabled=1 |
| Small Soldiers | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Smash Court | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Smash Court 2 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Smash Court 3 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Snatcher | 310 | 1.4 | 1.12 | 1.9 | frameskip=1 |
| SnoCross Championship Racing | 306 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Snow Racer 98 | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Soccer Kid | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, framelimit=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Sol Divide | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Sonic the Hedgehog (Demo) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Sonic Wings Special | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Sorcerer's Maze | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Soukyuu Gurentai | 306 | 1.4 | ⚠3 | ⚠3 | frameskip=1, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| Soukyuu Gurentai - Oubushutsugeki | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Soul Blade | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=24, frameskip=1, spu_reverb=0, spu_interpolation=1 |
| Soul of the Samurai | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Sound Qube | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| South Park | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| South Park - Chef's Luv Shack | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, multitap=1 |
| South Park Rally | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Soviet Strike | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, psxfix_SpuIrq2=1, spu_reverb=0 |
| Space Adventure Cobra | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Space Chaser 2000 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Space Griffon VF-9 | 306 | 1.4 | 1.15 | 1.9 | framelimit=0, FixFF=0 |
| Space Hulk | 310 | 1.4 | 1.15 | ⚠2 | frameskip=1 |
| Space Invaders | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, frameskip=1, controller=2, rumble_enabled=1 |
| Space Invaders 1500 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Space Invaders 2000 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Space Jam | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| Space Rider | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Spawn - The Eternal | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Spec Ops - Airborne Commando | 306 | 1.4 | 1.12 | 1.9 | graphicsFixes=192, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Spec Ops - Covert Assault | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Spec Ops - Ranger Elite | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Spec Ops - Stealth Patrol | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Speed Freaks | 306 | 1.4 | 1.12 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Speed King | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0 |
| Speed Power Gunbike | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=0 |
| Speed Racer | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Speedball 2100 | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0, controller=2, rumble_enabled=1 |
| Speedster | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0, soundtimer=0 |
| Spice World | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1 |
| Spider - The Video Game | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0 |
| Spider-Man | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, framelimit=0, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Spider-Man 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, framelimit=0, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Spin Jam | 310 | 1.4 | 1.16 | 1.6 | vandalFix=1, psxfix_SpuIrq=1, graphicsFixes=136, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Spirit Master (Prototype) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| SpongeBob SquarePants - SuperSponge | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=152, spu_reverb=0, controller=2, rumble_enabled=1 |
| Sports Car GT | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Sports Superbike | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Sports Superbike 2 | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Spot Goes to Hollywood | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, framelimit=0 |
| Spriggan - Lunar Verse | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Spyro 2 - Ripto's Rage! | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Spyro 3 - Year of the Dragon | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Spyro 3.5 - Forgotten Realms | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Spyro the Dragon | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, spu_reverb=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Stahlfeder | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0 |
| Stakes Winner | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Stakes Winner 2 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Star Fighter | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Star Gladiator | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0 |
| Star Gladiator - Final Crusade | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Star Ixiom | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Star Ocean - The 2nd Story (Disc 1) | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Star Ocean - The 2nd Story (Disc 2) | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=200, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Star Trek - Invasion | 310 | 1.4 | 1.12 | ⚠3 | FixFF=0, controller=2, rumble_enabled=1 |
| Star Wars - Dark Forces | 306 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=256, frameskip=1 |
| Star Wars - Demolition | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, framelimit=0, spu_reverb=0, FixFF=0, soundtimer=0, controller=2,0,0,0, rumble_enabled=1 |
| Star Wars - Jedi Power Battles | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=88, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Star Wars - Masters of Teras Kasi | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, framelimit=0 |
| Star Wars - Rebel Assault II (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=160, spu_reverb=0 |
| Star Wars - The Phantom Menace | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192, framelimit=0, spu_reverb=2, controller=2, rumble_enabled=1 |
| Starblade Alpha | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128 |
| Starborders | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| StarCon (Prototype) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Starfighter Sanvein | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=152, spu_reverb=0, controller=2, rumble_enabled=1 |
| StarSweep | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Starwinder - Ultimate Space Race | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0 |
| Steel Harbinger | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128 |
| Steel Reign | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Streak Hoverboard Racing | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Street Fighter - The Movie | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Street Fighter Alpha | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Street Fighter Alpha 2 | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_interpolation=1, FixFF=0 |
| Street Fighter Collection (Disc 1) | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Street Fighter Collection (Disc 2) | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Street Racer | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0, multitap=1 |
| Street Racquetball | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Street Scooters | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Street Sk8er | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Street Sk8er 2 | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| Strider | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0 |
| Strider 2 | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=128, spu_reverb=0, spu_interpolation=0 |
| Strike Force Hydra | 310 | 1.4 | 1.12 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Strikepoint - The Hex Missions | 306 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, spu_interpolation=1, FixFF=0, soundtimer=0 |
| Striker 1996 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Strikers 1945 | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Strikers 1945 II | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Submarine Commander | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Suiko Enbu | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Suikoden | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128 |
| Suikoden II | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=0 |
| Suikogaiden (Vol. 1) | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=8, spu_reverb=0, spu_interpolation=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Suikogaiden (Vol. 2) | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=8, spu_reverb=0, spu_interpolation=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Suikogaiden Vol. 1 | 306 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| Super Adventure Rockman (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| Super Adventure Rockman (Disc 2) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, soundtimer=0 |
| Super Adventure Rockman (Disc 3) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Super Bubble Pop | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Super Dropzone | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Super Pang Collection | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Super Puzzle Fighter II Turbo | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Super Robot Wars - Alpha Gaiden | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Super Shot Soccer | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Super Street Fighter - Remix 2009 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Superbike 2000 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Supercross | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Supercross 2000 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Supercross Circuit | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Superman (Beta) | 270 | 1.4 | ⚠3 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Superman (Prototype) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Supersonic Racers | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, FixFF=0, soundtimer=0 |
| Superstar Dance Club - 1 Hits!!! | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Surf Riders | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Suzuki Bakuhatsu | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Sven-Goeran Eriksson's World Challenge | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Sven-Goeran Eriksson's World Manager | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Swagman | 310 | 1.4 | ⚠3 | ⚠2 | frameskip=1, framelimit=0 |
| Swing | 310 | 1.4 | 1.12 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Sydney Summer Olympics 2000 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Syndicate Wars | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, multitap=1 |
| Syphon Filter | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=200, spu_reverb=0, controller=2, rumble_enabled=1 |
| Syphon Filter 2 (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |

#### T（210）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| T'ai Fu - Wrath of the Tiger | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=136, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| T.J. Lavin's Ultimate BMX | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tactical Armor Custom | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Tactical Armor Custom Gasaraki | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Tactics Ogre | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Tail Concerto | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=24, frameskip=1, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Tail of the Sun | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| Tales of Destiny | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128 |
| Tales of Destiny II (Disc 1) | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Tales of Destiny II (Disc 2) | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tales of Destiny II (Disc 3) | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tales of Phantasia | 310 | 1.4 | ⚠3 | ⚠2 | frameskip=1 |
| Tall - Infinity | 306 | 1.4 | 1.12 | 1.6 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Tall - Twins Tower | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128 |
| Tama - Adventurous Ball | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Tamago de Puzzle | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Tank Racer | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Tarzan | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tatsunoko Fight | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Taxi 2 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Team Buddies | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tear Ring Saga | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Techno BB | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, framelimit=0 |
| TechnoMage | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Tecmo Stackers | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Tecmo World Golf - Japan | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| Tekken | 310 | 1.4 | 1.15 | ⚠2 | frameskip=1, framelimit=0 |
| Tekken 2 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=64, frameskip=1, soundtimer=0 |
| Tekken 3 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tempest X3 | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=136 |
| Ten made Jack | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0 |
| Ten Pin Alley | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| Tenchu - Shinobi Gaisen | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tenchu - Stealth Assassins | 310 | 1.4 | ⚠5 | 1.9 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tenchu 2 - Birth of the Assassins | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1, controller=2, rumble_enabled=1 |
| Tennis Arena | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Terracon | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Test Drive 4 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Test Drive 5 | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Test Drive 6 | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Test Drive Le Mans | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cdda=1, psxfix_SpuIrq=1, graphicsFixes=152, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, spu_dbuf=1, controller=2, rumble_enabled=1 |
| Test Drive Off-Road | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| Test Drive Off-Road 2 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Test Drive Off-Road 3 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=192, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tetris | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Tetris - Card Captor Sakura | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Tetris Plus | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Tetris X | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Tetsuya Komuro - Gaball Screen | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1 |
| The Astronaut | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Bike Race | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| The Blue Marlin | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| The Bombing Islands | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Crow - City of Angels | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0, soundtimer=0 |
| The Curling | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| The Darts | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| The Devouring of Heaven & Earth II | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| The Dungeon | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Firemen 2 - Pete & Danny | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1 |
| The Game of Life | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, multitap=1, controller=2 |
| The Gateball | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| The Gun Shooting | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0 |
| The Gun Shooting 2 | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, frameskip=1, soundtimer=0 |
| The Hive (Disc 1) | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| The Hive (Disc 2) | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| The Hunter | 310 | 1.4 | 1.16 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, controller=2, rumble_enabled=1 |
| The Italian Job | 310 | 1.4 | ⚠4 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=208, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Kendo - Ken no Hanamichi | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| The King of Fighters 1995 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, framelimit=0, spu_reverb=0 |
| The King of Fighters 1999 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Last Blade | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0 |
| The Last Report | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| The Lion King - Mighty Adventure | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Little Mermaid II | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Master's Fighter | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| The Maze | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Mummy | 310 | 1.4 | ⚠4 | ⚠2 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| The Need for Speed | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0 |
| The Need for Speed - Porsche | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| The Need for Speed II | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| The Need for Speed III - Hot Pursuit | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136, framelimit=0 |
| The Need for Speed IV | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Need for Speed IV - Complete | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=144, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Need for Speed IV - Road Challenge | 270 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=216, framelimit=0, spu_interpolation=1, FixFF=0, controller=2, rumble_enabled=1 |
| The Need for Speed V - Porsche | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Next Tetris | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| The Note | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1 |
| The Pinball 3D | 310 | 1.4 | 1.15 | 1.6 | frameskip=1, framelimit=0, controller=2, rumble_enabled=1 |
| The Shooting | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| The Smurfs | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| The Sniper | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=8, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Suiei | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Sumo | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| The Table Hockey | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Takkyuu | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| The Three Stooges | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| The Unholy War | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=192, FixFF=0, controller=2, rumble_enabled=1 |
| The War of the Worlds | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=192, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| The Weakest Link | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=72, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Theme Hospital | 310 | 1.4 | 1.15 | ⚠2 | frameskip=1 |
| Theme Park | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Theme Park World | 310 | 1.4 | ⚠3 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| They're Here! | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Thousand Arms (Disc 1) | 310 | 1.4 | 1.15 | ⚠2 | vandalFix=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1 |
| Thousand Arms (Disc 2) | 310 | 1.4 | 1.15 | ⚠2 | vandalFix=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1 |
| Thrasher - Skate and Destroy | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Threads of Fate | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Thrill Kill | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, multitap=1, controller=2, rumble_enabled=1 |
| Thunder Force V - Perfect System | 306 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Thunder Storm | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, framelimit=0 |
| Thunder Truck Rally | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, frameskip=1 |
| Tiger Woods PGA Golf | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, soundtimer=0 |
| Tigershark | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Tilt! | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Time Commando | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0 |
| Time Crisis | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, framelimit=0 |
| Time Crisis - Project Titan | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Time Gal | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Tintin - Destination Adventure | 306 | 1.4 | ⚠3 | 1.9 | psxfix_Cdda=1, graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Tiny Bullets | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Tiny Tank | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=24, frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Tiny Toon Adventures - Great Beanstalk | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, spu_reverb=2, soundtimer=0 |
| Tiny Toon Adventures - PBA | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tiny Toon Adventures - Toonenstein | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Titan A.E. (Demo) | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Titan Wars | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| TNN Motorsports HardCore TR | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Toaplan Shooting Battle 1 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Tobal 2 | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=64, frameskip=1, framelimit=0, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Tobal No. 1 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, framelimit=0, FixFF=0, soundtimer=0 |
| TOCA 2 Touring Cars | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| TOCA 3 World Touring Cars | 310 | 1.4 | 1.12 | 1.6 | framelimit=0, spu_reverb=0, FixFF=0 |
| TOCA Touring Cars | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Tokimeki Memorial | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Tokimeki Memorial - Puzzle Dama | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Toko Toko Trouble - Chikyuu Itadaki | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Tokyo 23-ku - Seifuku-Wars | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Tokyo Dungeon | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0 |
| Tokyo Highway Battle | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192 |
| Tom and Jerry in House Trap | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tom Clancy's Rainbow Six | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Tom Clancy's Rainbow Six LW | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Tom Clancy's Rainbow Six RS | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Tomarunner vs L'Arc-en-Ciel | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Tomb Raider | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, rumble_enabled=1 |
| Tomb Raider - Prototype (E3 1996) | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, spu_interpolation=0 |
| Tomb Raider - Unfinished Business | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Tomb Raider II | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, rumble_enabled=1 |
| Tomb Raider II - Golden Mask | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0 |
| Tomb Raider II - The Golden Mask | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Tomb Raider III | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Tomb Raider III (Beta Demo) | 270 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=192, spu_interpolation=1, controller=2, rumble_enabled=1 |
| Tomb Raider IV | 310 | 1.4 | ⚠5 | 1.6 | psxfix_Cdda=1, graphicsFixes=24, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tomb Raider IV - The Times | 310 | 1.4 | 1.15 | 1.6 | vandalFix=1, psxfix_Cdda=1, graphicsFixes=8, frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Tomb Raider V - Chronicles | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Tomba! | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tommi Maekinen Rally | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=128, frameskip=1, framelimit=0, spu_reverb=2 |
| Tommi Makinen Rally | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Tonde! Tonde! Diet | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Tonka Space Station | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tony Hawk's Pro Skater | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tony Hawk's Pro Skater 2 | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Tony Hawk's Pro Skater 3 | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Tony Hawk's Pro Skater 4 | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Tony Hawk's Skateboarding | 306 | 1.4 | ⚠5 | 1.9 | psxfix_Cdda=1, framelimit=0, biosMode=0, spu_reverb=2, spu_interpolation=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Tonzura-kun | 310 | 1.4 | 1.15 | 1.6 | frameskip=1 |
| Top Gun - Fire at Will | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Top Shop | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, multitap=1 |
| ToPoLo | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Torc - Legend of the Ogre King | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128 |
| Torneko - The Last Hope | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Total Drivin | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=192, frameskip=1, framelimit=0, spu_reverb=0 |
| Total Eclipse Turbo | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| Touki Denshou - Angel Eyes | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, FixFF=0, soundtimer=0 |
| Tournament Leader | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Transport Tycoon | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0 |
| Trap Gunner | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Trash It | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, multitap=1 |
| Treasure Planet | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Treasures of the Deep | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=136, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tribal (Prototype) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, multitap=1, controller=2 |
| Trick'n Snowboarder | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Trickshot | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Triple Play Baseball | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| Triple Play Baseball 1997 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1 |
| Triple Play Baseball 1998 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Triple Play Baseball 2001 | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Tripuzz | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| TRL - The Rail Loaders | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Truck Rally | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| True Pinball | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, soundtimer=0 |
| Tsumu | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| Tsumu Light | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128 |
| TsunTsun-gumi 3 | 310 | 1.4 | 1.12 | 1.9 | frameskip=1 |
| Tsuukai - Slot Shooting | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Tunguska - Legend of Faith | 306 | 1.4 | ⚠3 | ⚠2 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Tunnel B1 | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Turnabout | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Twin Goddesses | 310 | 1.4 | ⚠4 | ⚠2 | psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0 |
| Twinbee Deluxe Pack | 310 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| Twisted Metal | 310 | 1.4 | ⚠4 | 1.9 | framelimit=0, rumble_enabled=1 |
| Twisted Metal (Prototype) | 310 | 1.4 | ⚠4 | ⚠2 | psxfix_Cdda=1, graphicsFixes=128 |
| Twisted Metal - Small Brawl | 310 | 1.4 | ⚠4 | ⚠3 | spu_reverb=0, controller=2, rumble_enabled=1 |
| Twisted Metal - World Tour | 310 | 1.4 | 1.15 | 1.9 | framelimit=0 |
| Twisted Metal 2 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Twisted Metal 2 (Beta) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Twisted Metal 3 | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Twisted Metal 4 | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, multitap=1, controller=2, rumble_enabled=1 |
| Two-Tenkaku | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| Tyco RC - Assault with a Battery | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, framelimit=0, FixFF=0 |

#### U（15）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Ubik | 310 | 1.4 | ⚠3 | ⚠2 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| UEFA Challenge | 310 | 1.4 | 1.16 | 1.6 | frameskip=1, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| UEFA Champions League 1998-99 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| UEFA Striker | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Ultima Underworld - Stygian Abyss | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Ultimate Fighting Championship | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Ultraman - Fighting Evolution | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| Ultraman - Tiga & Dyna | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0 |
| Um Jammer Lammy | 310 | 1.4 | 1.15 | 1.6 | psxfix_Cdda=1, graphicsFixes=136, controller=2, rumble_enabled=1 |
| Umihara Kawase Shun (Second Edition) | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Unstack | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| UPP | 310 | 1.4 | ⚠4 | 1.9 | psxfix_Cpu=1, frameskip=1, biosMode=0 |
| Uprising X | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, frameskip=1, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Urban Chaos | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=152, spu_reverb=0, controller=2, rumble_enabled=1 |
| US Racer | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |

#### V（40）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| V-Ball - Beach Volley Heroes | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| V-Rally | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=152, framelimit=0, soundtimer=0 |
| V-Rally - 97 Championship Edition | 270 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0 |
| V-Rally 2 | 310 | 1.4 | ⚠4 | ⚠3 | framelimit=0, spu_reverb=2, soundtimer=0, multitap=1, controller=2, rumble_enabled=1 |
| V-Tennis | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128 |
| V2000 | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0, FixFF=0, controller=2, rumble_enabled=1 |
| Vagrant Story | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=64, frameskip=1 |
| Valkyrie Profile (Disc 1) | 310 | 1.4 | 1.12 | ⚠2 | graphicsFixes=72, frameskip=1, framelimit=0, psxfix_SpuIrq2=1, controller=2, rumble_enabled=1 |
| Valkyrie Profile (Disc 2) | 310 | 1.4 | 1.12 | ⚠2 | graphicsFixes=72, frameskip=1, framelimit=0, psxfix_SpuIrq2=1, controller=2, rumble_enabled=1 |
| Vampir - Tales of Bloodsuckers | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Vampire - Bloody Bride | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Vampire Hunter D | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Vanark | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Vandal Hearts | 310 | 1.4 | ⚠4 | 1.6 | vandalFix=1, frameskip=1, rumble_enabled=1 |
| Vandal Hearts II | 310 | 1.4 | 1.16 | ⚠2 | vandalFix=1, frameskip=1 |
| Vanguard Bandits | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Vanishing Point | 310 | 1.4 | ⚠4 | 1.6 | spu_reverb=0, controller=2, rumble_enabled=1 |
| Vermin Kids | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Versailles - Game of Intrigue | 310 | 1.4 | 1.16 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=128, spu_reverb=0 |
| Vib-Ribbon | 306 | 1.4 | ⚠4 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Viewpoint | 310 | 1.4 | 1.15 | 1.9 | psxfix_Cpu=1, psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0 |
| Vigilante 8 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Vigilante 8 - 2nd Offense | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| VIP | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Viper | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=192, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Virtual Golf | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, soundtimer=0 |
| Virtual Hiryuu no Ken | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, framelimit=0 |
| Virtual Kasparov | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, soundtimer=0 |
| Virtual Pool | 310 | 1.4 | ⚠4 | 1.9 | spu_reverb=0 |
| Virtual Pool 3 | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Virtual Pro Wrestling | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Virus - It Is Aware | 310 | 1.4 | ⚠4 | 1.9 | graphicsFixes=64, frameskip=1, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Virus - The Battle Field | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| Viva Soccer | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| VMX Racing | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0 |
| Voltage Fighter Gowcaizer | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| VR Baseball '97 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, FixFF=0 |
| VR Baseball '99 | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1 |
| VR Sports Powerboat Racing | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=136, framelimit=0, spu_reverb=0 |
| Vs | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0 |

#### W（79）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Wacky Races | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=64, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Walt Disney World Quest - MRT | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Wangan Trial (Disc 1) | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Wangan Trial (Disc 2) | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| Wanted | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| War Gods | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| WarGames - Defcon 1 | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=256, controller=2, rumble_enabled=1 |
| Warhammer - Dark Omen | 306 | 1.4 | 1.15 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Warhammer - SHR | 306 | 1.4 | ⚠3 | 1.9 | framelimit=0, spu_reverb=2, FixFF=0 |
| Warhawk | 306 | 1.4 | ⚠4 | 1.9 | spu_reverb=2 |
| Warm Up! | 306 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, FixFF=0, controller=2, rumble_enabled=1 |
| Warriors of Might & Magic | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Warriors of Might and Magic | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Warzone 2100 | 310 | 1.4 | 1.15 | 1.9 | frameskip=1, controller=2, rumble_enabled=1 |
| Wayne Gretzky's 3D Hockey '98 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0 |
| WCW Backstage Assault | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_interpolation=1, controller=2, rumble_enabled=1 |
| WCW Mayhem | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0, controller=2, rumble_enabled=1 |
| WCW Nitro | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| WCW vs The World | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0 |
| WCW-nWo Thunder | 310 | 1.4 | 1.16 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| WDL - Thunder Tanks | 310 | 1.4 | ⚠5 | ⚠3 | psxfix_Cdda=1, graphicsFixes=200, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| WDL - Thunder Tanks_ | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| WDL - WarJetz | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Welcome House | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Wheel of Fortune | 310 | 1.4 | 1.12 | 1.6 | psxfix_Cdda=1, psxfix_Cpu=1, graphicsFixes=80, frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Whizz | 306 | 1.4 | 1.15 | ⚠2 | frameskip=1 |
| Who Wants to Be a Millionaire (AUS) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| Who Wants to Be a Millionaire (IRE) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Who Wants to Be a Millionaire (UK) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Who Wants to Be a Millionaire 2 (UK) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Who Wants to Be a Millionaire 2 (US) | 310 | 1.4 | ⚠3 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, controller=2, rumble_enabled=1 |
| Who Wants to Be a Millionaire 3 (US) | 310 | 1.4 | ⚠3 | 1.6 | psxfix_Cdda=1, graphicsFixes=8, frameskip=1, controller=2, rumble_enabled=1 |
| Who Wants to Be a Millionaire Jr (UK) | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Wild 9 | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Wild Arms | 306 | 1.4 | 1.15 | ⚠2 | frameskip=1 |
| Wild Arms 2 (Disc 1) | 306 | 1.4 | 1.12 | ⚠2 | psxfix_Cdda=1, frameskip=1, spu_reverb=2 |
| Wild Arms 2 (Disc 2) | 306 | 1.4 | 1.12 | ⚠2 | psxfix_Cdda=1, frameskip=1, spu_reverb=2 |
| Wild Boater | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, controller=2, rumble_enabled=1 |
| Wild Rapids | 306 | 1.4 | ⚠4 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Wing Commander III (Disc 1) | 306 | 1.4 | 1.15 | 1.6 | spu_reverb=2 |
| Wing Commander III (Disc 2) | 266 | 1.4 | 1.15 | 1.6 | spu_reverb=2 |
| Wing Commander III (Disc 3) | 266 | 1.4 | 1.15 | 1.6 | spu_reverb=2 |
| Wing Commander III (Disc 4) | 266 | 1.4 | 1.15 | 1.6 | spu_reverb=2 |
| Wing Commander IV (Disc 1) | 306 | 1.4 | 1.15 | 1.6 | spu_reverb=2 |
| Wing Commander IV (Disc 2) | 266 | 1.4 | 1.15 | 1.6 | spu_reverb=2 |
| Wing Commander IV (Disc 3) | 266 | 1.4 | 1.15 | 1.6 | spu_reverb=2 |
| Wing Commander IV (Disc 4) | 266 | 1.4 | 1.15 | 1.6 | spu_reverb=2, controller=2 |
| Wing Over | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| Wing Over 2 | 306 | 1.4 | ⚠3 | 1.9 | FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Wipeout | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, rumble_enabled=1 |
| Wipeout 2097 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, spu_reverb=0, rumble_enabled=1 |
| WipEout 3 - Special Edition | 310 | 1.4 | ⚠4 | ⚠3 | graphicsFixes=216, framelimit=0, spu_reverb=2 |
| Wipeout XL | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128 |
| Wizardry - Llylgamyn Saga | 310 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| Wizardry - New Age of Llylgamyn | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| Wolf Fang - Kuuga 2001 | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, frameskip=1, spu_reverb=0, FixFF=0, soundtimer=0 |
| Wonder 3 | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0 |
| Wonder Trek | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Woody Woodpecker Racing | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| World Championship Snooker | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| World Cup Golf - Professional Edition | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, FixFF=0 |
| World League Soccer 1998 | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0, spu_reverb=0 |
| World Tennis Stars | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| World's Scariest Police Chases | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, graphicsFixes=64, spu_reverb=0, controller=2, rumble_enabled=1 |
| Worms | 306 | 1.4 | 1.16 | 1.9 | framelimit=0 |
| Worms Armageddon | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| Worms Pinball | 310 | 1.4 | ⚠5 | 1.6 | psxfix_SpuIrq=1, graphicsFixes=128, framelimit=0, psxfix_SpuIrq2=1, spu_reverb=0 |
| Worms World Party | 310 | 1.4 | 1.12 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| WRC - Arcade | 306 | 1.4 | 1.12 | 1.6 | graphicsFixes=192, framelimit=0, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Wreckin Crew | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=2, FixFF=0, controller=2 |
| Wu-Tang - Shaolin Style | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| Wu-Tang - Taste the Pain | 310 | 1.4 | 1.12 | 1.6 | graphicsFixes=128, framelimit=0, biosMode=0, spu_reverb=0, spu_interpolation=0, controller=2, rumble_enabled=1 |
| WWF Attitude | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, multitap=1, controller=2, rumble_enabled=1 |
| WWF In Your House | 310 | 1.4 | 1.16 | 1.6 | vandalFix=1, graphicsFixes=392, framelimit=0, spu_reverb=0, multitap=1 |
| WWF SmackDown | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=0, spu_interpolation=0, multitap=1, controller=2, rumble_enabled=1 |
| WWF SmackDown 2 | 310 | 1.4 | 1.12 | ⚠3 | frameskip=1, spu_reverb=0, spu_interpolation=0, multitap=1, controller=2, rumble_enabled=1 |
| WWF SmackDown 2 - Inspired | 310 | 1.4 | 1.12 | 1.6 | frameskip=1, spu_reverb=0, multitap=1, controller=2, rumble_enabled=1 |
| WWF War Zone | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, multitap=1 |
| WWF WrestleMania | 306 | 1.4 | ⚠4 | 1.6 | frameskip=1 |

#### X（21）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| X - Selection of Fate | 310 | 1.4 | 1.16 | 1.9 | frameskip=1 |
| X'treme Roller | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=128, framelimit=0, spu_reverb=0, spu_interpolation=1, controller=2, rumble_enabled=1 |
| X-Bladez - Inline Skater | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| X-COM - Enemy Unknown | 306 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| X-COM - Terror from the Deep | 306 | 1.4 | 1.15 | 1.6 | graphicsFixes=128 |
| X-Files (Disc 1) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136 |
| X-Files (Disc 2) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136 |
| X-Files (Disc 3) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136 |
| X-Files (Disc 4) | 310 | 1.4 | 1.15 | 1.6 | graphicsFixes=136 |
| X-Men - Children of the Atom | 310 | 1.4 | ⚠3 | 1.9 | graphicsFixes=128, framelimit=0, FixFF=0, soundtimer=0 |
| X-Men - Mutant Academy | 310 | 1.4 | ⚠4 | 1.6 | graphicsFixes=64, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| X-Men - Mutant Academy 2 | 306 | 1.4 | ⚠3 | ⚠3 | graphicsFixes=64, framelimit=0, spu_reverb=2, FixFF=0, soundtimer=0, controller=2, rumble_enabled=1 |
| X-Men vs. Street Fighter - EX Edition | 306 | 1.4 | 1.15 | 1.9 | frameskip=1 |
| X2 - No Relief | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Xena - Warrior Princess | 310 | 1.4 | ⚠5 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Xenocracy - Ultimate Solar War | 306 | 1.4 | 1.12 | 1.6 | graphicsFixes=64, frameskip=1, controller=2, rumble_enabled=1 |
| Xenogears (Disc 1) | 310 | 1.4 | ⚠3 | 1.6 | framelimit=0, spu_reverb=0 |
| Xenogears (Disc 2) | 310 | 1.4 | ⚠3 | 1.6 | framelimit=0, spu_reverb=0 |
| Xevious 3D-G Plus | 306 | 1.4 | 1.15 | ⚠3 | frameskip=1, FixFF=0, soundtimer=0 |
| XS Airboat Racing | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, spu_reverb=0, controller=2, rumble_enabled=1 |
| XS Junior League Dodgeball | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |

#### Y（13）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Yacht Racing 1999 - Nippon Challenge | 310 | 1.4 | ⚠4 | 1.6 | psxfix_Cdda=1, graphicsFixes=128, spu_reverb=0, controller=2, rumble_enabled=1 |
| Yakiniku Bugyou | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0 |
| Yaroze Games | 310 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_usexa=0, spu_reverb=0, spu_interpolation=0, rumble_enabled=1 |
| Yeh Yeh Tennis | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, spu_reverb=0, controller=2, rumble_enabled=1 |
| Yellow Brick Road | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Yetisports Deluxe | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| Yetisports World Tour | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| You Don't Know Jack (Disc 1) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, multitap=1, controller=2, rumble_enabled=1 |
| You Don't Know Jack (Disc 2) | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, multitap=1, controller=2, rumble_enabled=1 |
| You Don't Know Jack - Mock 2 | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, controller=2, rumble_enabled=1 |
| YoYo's Puzzle Park | 306 | 1.4 | 1.15 | 1.9 | frameskip=1, FixFF=0, controller=2, rumble_enabled=1 |
| Yu-Gi-Oh! Forbidden Memories | 310 | 1.4 | 1.15 | ⚠3 | frameskip=1, controller=2, rumble_enabled=1 |
| Yusha - Heaven's Gate | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0, soundtimer=0 |

#### Z（14）

| 游戏 | 大小 | core | gpu | spu | 差异项 |
| --- | --- | --- | --- | --- | --- |
| Z | 310 | 1.4 | ⚠4 | 1.9 | frameskip=1 |
| Zanac X Zanac | 310 | 1.4 | ⚠4 | ⚠3 | psxfix_Cdda=1, frameskip=1, spu_reverb=0, FixFF=0, controller=2, rumble_enabled=1 |
| Zeiramzone | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128, framelimit=0 |
| Zen-Nippon Joshi Wrestling | 310 | 1.4 | ⚠5 | 1.6 | graphicsFixes=128, spu_reverb=0 |
| Zen-Nippon Pro Wrestling | 310 | 1.4 | 1.15 | 1.9 | graphicsFixes=128 |
| Zero 4 Champ | 310 | 1.4 | 1.12 | 1.9 | framelimit=0 |
| Zero Divide | 310 | 1.4 | ⚠5 | ⚠3 | frameskip=1, spu_reverb=0, spu_interpolation=0 |
| Zero Divide 2 | 310 | 1.4 | ⚠4 | ⚠3 | frameskip=1, spu_reverb=2, soundtimer=0 |
| Zero Pilot | 310 | 1.4 | ⚠4 | 1.6 | frameskip=1, framelimit=0 |
| Zero4 Champ DooZy-J | 310 | 1.4 | ⚠4 | 1.6 | framelimit=0, FixFF=0, soundtimer=0 |
| Zig Zag Ball | 310 | 1.4 | 1.12 | 1.9 | graphicsFixes=128, controller=2, rumble_enabled=1 |
| Zoku Gussun Oyoyo | 310 | 1.4 | ⚠3 | 1.6 | graphicsFixes=128, framelimit=0 |
| Zoop | 306 | 1.4 | ⚠3 | 1.9 | frameskip=1, spu_reverb=2, FixFF=0, soundtimer=0 |
| ZXE-D - Legend of Plasmatlite | 310 | 1.4 | 1.16 | 1.6 | graphicsFixes=128, framelimit=0, biosMode=0 |
