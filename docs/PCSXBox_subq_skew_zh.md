# PCSXBox ogXbox —— CD-ROM SubQ skew 修复说明（名将 / Captain Commando 第一关 BOSS 贴图错乱）

## 1. 现象

《名将》（Captain Commando，日版）第一关 BOSS 出现时，BOSS 的贴图错误 / 缺失，
而其它场景、其它游戏正常。原始 v1.5 Plus 成品 XBE 同样复现，因此这不是本次
移植引入的回归，而是这套老 PCSX 分支长期存在的兼容性问题。

## 2. 上游同类结论

这个问题在多个独立 PS1 模拟器项目中都有记录，且结论一致：**PS1 光驱在回答
`CdlGetlocP`（SubQ 子通道位置查询）时，报告的应该是"最后交付扇区 + 2"，而很多
老实现只报告当前/上一扇区，或者干脆返回绝对值**。

| 项目 | 位置 | 取值 |
|---|---|---|
| DuckStation | `src/core/cdrom.cpp` `SUBQ_SECTOR_SKEW` | `2`（并且是可开关的设置项 `cdrom_subq_skew`） |
| PCSX-ReARMed | `libpcsxcore/cdrom.c` `SUBQ_FORWARD_SECTORS` | `2u`（notaz 加在 `cdrReadInterrupt()` 里的 hack） |
| MiSTer `PSX_MiSTer` | issue #66 / #303 "First boss does not render" | 同一根因 |

另外 elotrolado / Reddit 上的实机反馈表明：**真机 + 直读同样会出现该现象**，
干净 redump 镜像也会出现，属于 dump 与驱动器时序差异导致的游戏侧容错失败，
不是某一版模拟器的独有 bug。

## 3. 本地源码中的根因

`src/plugins.c` 里的 `CDR__getBufferSub()` 在本工程中**恒返回 NULL**：

```c
unsigned char * CDR__getBufferSub(void) { ... return NULL ; }
```

于是 `CdlGetlocP` 永远走 fallback 分支（自己根据 `cdr.Prev` 拼一个 SubQ 应答）。
原来的 fallback 只做了"绝对 MSF → 曲目相对 MSF"的换算（秒字段 `-2`），
**没有把"最后交付扇区 + 2"这一点体现出来**。

《名将》在 BOSS 处会硬编码查询一个比自己实际需要的扇区**提前 2 扇区**的时间码，
用它来决定何时把 BOSS 的 tile 数据传进 VRAM。时间码对不上 → 传输永远不发生 →
BOSS 贴图错乱/缺失。其它场景不依赖这个查询，所以只有 BOSS 出问题。

## 4. 修复实现

新增编译期开关宏 `PCSXBOX_SUBQ_SKEW`，默认开启；在 GetlocP 的 fallback 应答上
叠加"最后交付扇区 + 2"的相位。

**关键点：四份内核的相位补偿量不同**，因为它们 `cdrReadInterrupt()` 的结构不同：

| 内核 | 源文件 | 行尾 | `cdr.Prev` 语义 | 需要的 skew |
|---|---|---|---|---|
| 1.5（`src`，default.xbe） | `src/cdrom.c` | CRLF | 已交付扇区 **+1** | **1** |
| 1.4（`src/good`，default14.xbe） | `src/good/cdrom.c` | CRLF | 已交付扇区 **+1** | **1** |
| 1.6（`src/1.6`，default16.xbe） | `src/1.6/CdRom.c`（注意文件名大写） | **LF** | 已交付扇区 **+1** | **1** |
| Reloaded（`src/1.5 (Reloaded)`，defaultr.xbe） | `src/1.5 (Reloaded)/cdrom.c` + `cdrom.h` | CRLF | 已交付扇区本身 | **2** |

相位差异的原因：老三代内核在 `cdrReadInterrupt()` **末尾**才调用 `ReadTrack()`，
`cdr.Prev` 已经跑到"已交付 + 1"；Reloaded 内核在 `cdrReadInterrupt()` **开头**先
`ReadTrack(cdr.SetSector)`，所以 `cdr.Prev` 等于"已交付"本身。

注意 `cdr.Prev` 存的是 **BCD 编码的绝对 MSF**（`time2addrB()` 里 `-150`，
`00:02:00` 对应 LBA 0），所以加成是十进制 `+N` 后按 75 帧进位再做 BCD 转换。

Reloaded 内核的宏定义放在 `src/1.5 (Reloaded)/cdrom.h`：

```c
#ifndef PCSXBOX_SUBQ_SKEW
#define PCSXBOX_SUBQ_SKEW 2
#endif
```

其余三核的宏定义直接放在各自的 `cdrom.c` / `CdRom.c` 顶部。

## 5. 实机验证

* 修复前：《名将》第一关 BOSS 贴图错乱（原版 v1.5 Plus 同样如此）。
* 修复后：BOSS 贴图正常。
* 回归：《生化危机 3》CHD、《雷电计划》CHD 均正常加载运行。

## 6. 如何回退

把对应内核里的宏改成 0 重新编译即可完全恢复旧行为：

```c
#define PCSXBOX_SUBQ_SKEW 0
```

或从命令行注入：

```bash
EXTRA_DEFINE=PCSXBOX_SUBQ_SKEW=0 CORE=15 JOBS=$(nproc) bash build_oldxdk/build.sh
```

## 7. 已知副作用

SubQ skew 会破坏 **libcrypt** 类防拷校验（DuckStation 作者 stenzek 在实现同一
hack 时明确写过这一点）。如果遇到 PAL 区防拷游戏出现异常，优先怀疑这里，
用上面的方法把 `PCSXBOX_SUBQ_SKEW` 改为 0 回退后重测。

## 8. 相关文件

```
src/cdrom.c                     # 1.5 内核
src/good/cdrom.c                # 1.4 内核
src/1.6/CdRom.c                 # 1.6 内核
src/1.5 (Reloaded)/cdrom.c      # Reloaded 内核
src/1.5 (Reloaded)/cdrom.h      # Reloaded 内核的宏定义
```

每个被修改的文件旁都留有一份改动前的备份 `*.pre_subq_skew`。
