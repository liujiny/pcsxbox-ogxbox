# 调试

完整过程见工程根目录 `ANALYSIS_BASELINE3.md`（含实机读数），这里是可复用的结论。

## 经典案例：主画面黑屏但 R3 缩略图正常

**症状**：游戏在跑（FPS ~60），按 R3 能出菜单，菜单里的游戏缩略图正常且会变，
就是全屏游戏画面是黑的。

**排查过且无效的方向（不要重复）**：换 dynarec（`src\ix86\good`、`src\1.5_orig`）、
强制 interpreter（速度掉到 0.x FPS，不能用来判断）、把 `CPanel::Render` 换成
`Sprite->Draw`、强制目标矩形 `640x480`、强制仿缩略图 `200x150`、去掉
`BeginScene`/`EndScene`、`BlockUntilNotBusy`、禁用 `xbox_clear_screen()`、
去掉 FPS/overlay 绘制。**全都是改绘制方式，而问题在参数/数据。**

**真正原因**：**跨构建混用**。

1. 测试包里放的是「自己编的 `default.xbe`」+「发布版原版 `default14.xbe`/`defaultr.xbe`」。
   1.5 前端跳到发布版 1.4 核心时，`PCSXBOX_LAUNCH_DATA`（头部 757 字节 + 168 字节扩展
   + reserved）的字段偏移与发布版二进制未必一致，核心读到错位的画面/参数块。
2. `E:\SAVES\PCSXBOX\*.stg` 里 `m_nScreen*` 等字段是发布版写的旧布局，被当前构建按
   新布局 `fread` → 进游戏后画面参数错乱。
3. "删掉其它 XBE 就死机" 也印证了确实发生过 `XLaunchNewImage` 到另一个 XBE。

**规则**：不要混用不同构建的 XBE，也不要用别的构建写过的 `.stg`。四核心整体替换，
换核心时清一次 `E:\SAVES\PCSXBOX`。

**顺手留下的保护网**（保留，它会自证）：

* `render_to_texture()` 里若 `m_nScreenMaxX/Y <= 0` 或 `m_nScreenX/Y < 0`，钳到
  `0,0,640,480`。
* 编译加 `EXTRA_DEFINE=PCSXBOX_SCREEN_DIAG` 会在画面前 ~5 秒打印
  `SCREEN x=.. y=.. w=.. h=.. src=..,..`，触发钳位时打印 `SCREENFIX`。
  实测基线正常输出 `SCREEN x=0 y=0 w=640 h=480 src=640,480` —— 说明钳位是保护网，
  不是修复点。

## 已知的死代码

`pcsxbox.cpp::init_texture()` 里 `if (Texture) { return 1; Texture->Release(); ... }`：
`return` 之后的释放代码永不可达，只是逻辑冗余，**不是**黑屏原因。

## 诊断手段

| 宏 | 作用 |
|---|---|
| `PCSXBOX_SCREEN_DIAG` | 打印全屏目标矩形与钳位触发 |
| `PCSXBOX_PROFILE` | 每帧显示 `cpu= / rast= / blit=` 毫秒（诊断包用，别当最终版发布） |
| `PCSXBOX_NO_FAST_BRANCHTEST` | 关掉 dynarec 块出口内联，回退到原 C 调用 |
| `OPT_SET=size` | 回到旧的 `/Ob1 /Os` 构建做 A/B |

## 定位方法

改任何共享文件前先 grep 三份 core 是否都有同名文件，再决定是否同步改：

```bash
cd "/mnt/e/Projects/PlayStation 1 - PCSXBox"
grep -rn "<符号>" src/psxcounters.c src/good/psxcounters.c "src/1.6/PsxCounters.c"
```

注意 1.4/1.5 用小写 `psxcounters.c`，1.6 用 `PsxCounters.c`，大小写不同是历史遗留。


## 案例：Captain Commando 第一关 BOSS 贴图错乱（CD-ROM SubQ skew）

**现象**：主画面/速度/菜单缩略图都正常，只有第一关 BOSS（及第三关部分敌人）贴图花掉。
不是本项目独有的 bug，是 PS1 模拟界著名问题。

**公开资料**（2026-10 复查）：
- MiSTer `PSX_MiSTer` issue #66 / #303（"First boss does not render"）；
- DuckStation 作者 stenzek 在 `src/core/cdrom.cpp` 直接写明机制，并加 `SUBQ_SECTOR_SKEW = 2`
  （设置项 `cdrom_subq_skew`，默认开）；
- PCSX-ReARMed 等价实现 `SUBQ_FORWARD_SECTORS 2u`（`libpcsxcore/cdrom.c:169`，
  `cdrReadInterrupt()` 里 `cdr.SubqForwardSectors` 1→2）；notaz 就是用它修掉这个 bug；
- 实机 + 直读也会有人遇到（可能叠加 dump 差异；redump 镜像干净）。

**机制**：BOSS 段的 BGM 音频扇区里重复铺了 tile/texture 数据，游戏靠狂刷 `GetlocP`
等一个"比真正想要的扇区**提前 2**"的时间码，等到就立刻 DMA 把数据抢出来；模拟器若不做
这个偏移，它永远等不到，就一直重读，BOSS 的 tile 永远上传不进去。

**本项目的问题**：Xbox 侧 `src/plugins.c:335 CDR__getBufferSub()` 恒返回 NULL，所以
`CdlGetlocP` 永远走 fallback 分支；而那个 fallback 只把秒字段 `-2`（其实是
"绝对 MSF → track-relative"的老做法），完全没有 +2 扇区偏移。

**补丁（2026-10-07 落地，4 核全覆盖）**：

| 文件 | 位置 | 改法 |
|---|---|---|
| `src/cdrom.c` | `CdlGetlocP` fallback | `cdr.Prev` BCD→二进制 MSF，+`PCSXBOX_SUBQ_SKEW`(=1)，再转回 BCD；track-relative 用 借位安全的 `-2s` |
| `src/good/cdrom.c` | 同上 | 同上 |
| `src/1.6/CdRom.c` | 同上（LF 行尾，文件名大写） | 同上 |
| `src/1.5 (Reloaded)/cdrom.c` | `Create_Fake_Subq()` 里 `fake_subq_real` 之前 | 给 `temp_cur` 加 `PCSXBOX_SUBQ_SKEW`(=2) |
| `src/1.5 (Reloaded)/cdrom.h` | `itob/btoi` 旁 | 定义 `PCSXBOX_SUBQ_SKEW` |

**相位（最容易搞错的地方，±1 就无效）**：上游语义是报告"**最后交付扇区 + 2**"。
- 老三代核：`cdrReadInterrupt()` 末尾才 `ReadTrack()`，`cdr.Prev = 已交付 + 1` → 加 **1**；
- Reloaded 核：`cdrReadInterrupt()` **开头**先 `ReadTrack(cdr.SetSector)`，`Prev = 已交付` → 加 **2**。
- `cdr.Prev` 存的是 **BCD 绝对 MSF**（`time2addrB()` 里 `-150`，00:02:00 = LBA 0）。

**回退**：`PCSXBOX_SUBQ_SKEW` 改 0 即恢复旧行为；各文件旁有 `*.pre_subq_skew` 备份。

**副作用**：会破坏 libcrypt 子通道校验（stenzek 原话）。PAL 防拷游戏异常时优先怀疑它。

**验收**：用 `.st0` 存读档直接跳到 BOSS 前最省事（槽号替换扩展名末位，slot 0 = `.st0`）。
