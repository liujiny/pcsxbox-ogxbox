# 构建

## 工具链（固定）

| 部件 | 路径 |
|---|---|
| XDK | `/mnt/d/Tools/ogxbox_legacy/XDK/xbox`（Windows 侧 `D:\Tools\ogxbox_legacy\XDK\xbox`） |
| 编译器 | `bin/vc71/CL.Exe`（VC7.1，`cl 13.10.3077`） |
| 链接器 | `bin/vc71/Link.Exe` |
| 打包 XBE | `bin/imagebld.exe` |

**这是老版官方 XDK 5849.17，不是 RxDK。** 编译/链接/打包都是 Windows 二进制，从 WSL
通过 interop 驱动。宿主是 Linux/WSL，`build_oldxdk/build.sh` 自动定位工程根目录，
可用 `PROJ=` `XDK=` `XDK_WIN=` 覆盖。

## 命令

工程目录 `/mnt/e/Projects/PlayStation 1 - PCSXBox`：

```bash
CORE=15 JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.5 前端+核心 -> pcsxbox15.xbe
CORE=14 JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.4 核心     -> pcsxbox14.xbe
CORE=16 JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.6 -> pcsxbox16.xbe
CORE=15r OUTDIR=build_oldxdk_15r JOBS=$(nproc) bash build_oldxdk/build.sh  # Reloaded -> pcsxbox15r.xbe
```

| 变量 | 含义 |
|---|---|
| `CORE` | `14` / `15` / `16`，选哪套 core |
| `JOBS` | 并行编译数（默认 8） |
| `OUTDIR` | 输出目录（默认 `build_oldxdk`） |
| `OPT_SET` | `speed`（默认 `/O2 /Ob2 /Ot /Og`）或 `size`（旧行为 `/O2 /Ob1 /Os /Og`） |
| `MODE` | `release`（默认）或 `debug` |
| `EXTRA_DEFINE` | 追加一个 `-D`，用于诊断宏 |

四核心全编大约几分钟（现代机器）。日志在 `$OUTDIR/logs/`，编译失败会**在链接前中止**。

## 四个核心的映射

| CORE | 宏 | core 源码目录 | 输出 |
|---|---|---|---|
| 15 | （无） | `src` | `pcsxbox15.xbe` |
| 14 | `OLDCORE` | `src\good` | `pcsxbox14.xbe` |
| 16 | `BETACORE` | `src\1.6` | `pcsxbox16.xbe` |
| 15r（`reloaded`） | `RELOADEDCORE` | `src\1.5 (Reloaded)` | `pcsxbox15r.xbe` |

四个工程共用同一个 `pcsxbox.cpp` 前端。`parse_vcproj.py` 读出 `Release|Xbox` 配置的源文件
列表，`remap_core.py` 再把 `pcsxbox.vcproj` 里的目录重映射到目标 core 目录。

`CORE=15r` 是唯一需要额外 include 路径的：它的头文件按 `"1.5 (Reloaded)\X.h"` 引用，
`psxbios.c` 还会从 `src\` 拉 `sjisfont.h`，所以 `build.sh` 会再追加一条 `src`。
它也会把 `src\gpu\src` / `src\spu\src` / `src\ix86` 整体换成 `1.5 (Reloaded)` 版本，
**并且**多编 `cdriso.c`、`gte_divider.c`、`ppf.c`（SPU 侧 `externals.c`、`xa_new.c`）。
建议给它单独的 `OUTDIR`（例如 `OUTDIR=build_oldxdk_15r`），否则会和其它核心抢同一份 obj。

## 编译/链接要点

```
CODEOPT  = /Gy /GF /MT /W3 /wd4996
DEFINES  = WIN32 _USE_XGMATH _XBOX NDEBUG IS_LITTLE_ENDIAN __WIN32__ __i386__
           PCSX_VERSION="1.4" _SDL CHDR_SYSTEM_ZLIB  (+ OLDCORE | BETACORE)
INCLUDES = <XDK>\include ..\..\Common\include ..\common <coreinc> ..\common\mp3
           src\gpu\src src\gpu\src\fpse ..\common\samba ..\common\sdl
           src\chd src\chd\libchdr\include src\chd\libchdr\src
```

* 链接必须 `/NODEFAULTLIB:LIBC`（以及 `LIBCD`/`MSVCRT`/`MSVCRTD`），否则 `libsmb.lib`
  与 `/MT` 冲突。`libsmb.lib(types.obj) : warning LNK4218` 可忽略。
* 预编译汇编目标文件直接参与链接：`..\common\mp3\obj\{cdctasm,cwin8asm,cwinasm,mdctasm,msisasm}.obj`、
  `.\src\gpu\src\i386.obj`、`..\common\2xsaiw.obj`、`..\common\hq2x16.obj`。
* `imagebld` 参数：`/STACK:0xf0000 /INITFLAGS:0x0 /TESTID:0x08302006
  /TESTNAME:pcsxbox_<core>core /TESTMEDIATYPES:0x80000007`。

## 产物命名与投放

模拟器按固定文件名 chain-load 其它 XBE，所以投放时改名：

| 槽位 | 编译产物 | 放成 |
|---|---|---|
| 1.5 | `pcsxbox15.xbe` | `default.xbe` |
| 1.4 | `pcsxbox14.xbe` | `default14.xbe` |
| Reloaded | `pcsxbox15r.xbe` | `defaultr.xbe` |
| 1.6 | `pcsxbox16.xbe` | `default16.xbe` |

启动 `default.xbe`；游戏设置要求换核心时，由 `pcsxbox.cpp` 的 `rgCoreXbe[4]`
把自己交给 `default14.xbe` / `defaultr.xbe` / `default16.xbe`。
`defaultr.xbe` 旧名 `reloaded.xbe` 也会探测。

**要么四份齐全，要么一份伴生 XBE 都不要放**：跳到不存在的 XBE 会立刻黑屏死机，
连 R3 菜单都出不来。

需要一个"编四个核心 + 装成测试目录"的一步脚本时，用
`scripts/build_all_cores.sh`（见该脚本头部说明）。
