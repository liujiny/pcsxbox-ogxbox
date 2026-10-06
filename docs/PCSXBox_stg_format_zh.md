# PCSXBox `.stg` 每游戏设置文件格式说明

以 **Raiden Project**（PCSXbox V25「1470x Pack」里的那份）为例，逐项拆解 `*.stg` 的内容。

数据来源：`apps\PCSXbox\SAVES\PCSXBOX\Raiden Project\Raiden Project.stg`（310 字节），
并对照本项目源码 `pcsxbox.cpp` 的 `saveSettings()` / `loadSettings()`，以及同包
2082 个 `.stg` 的取值分布做交叉验证。

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
