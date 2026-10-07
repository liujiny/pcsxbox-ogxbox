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
CORE=16 JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.6/Reloaded -> pcsxbox16.xbe
```

| 变量 | 含义 |
|---|---|
| `CORE` | `14` / `15` / `16`，选哪套 core |
| `JOBS` | 并行编译数（默认 8） |
| `OUTDIR` | 输出目录（默认 `build_oldxdk`） |
| `OPT_SET` | `speed`（默认 `/O2 /Ob2 /Ot /Og`）或 `size`（旧行为 `/O2 /Ob1 /Os /Og`） |
| `MODE` | `release`（默认）或 `debug` |
| `EXTRA_DEFINE` | 追加一个 `-D`，用于诊断宏 |

三核心全编大约十几秒（现代机器）。日志在 `$OUTDIR/logs/`，编译失败会**在链接前中止**。

## 三个核心的映射

| CORE | 宏 | core 源码目录 | 输出 |
|---|---|---|---|
| 15 | （无） | `src` | `pcsxbox15.xbe` |
| 14 | `OLDCORE` | `src\good` | `pcsxbox14.xbe` |
| 16 | `BETACORE` | `src\1.6` | `pcsxbox16.xbe` |

三个工程共用同一个 `pcsxbox.cpp` 前端。`parse_vcproj.py` 读出 `Release|Xbox` 配置的源文件
列表，`remap_core.py` 再把 `pcsxbox.vcproj` 里的目录重映射到目标 core 目录。

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

| 编译产物 | 放成 |
|---|---|
| `pcsxbox15.xbe` | `default.xbe` |
| `pcsxbox14.xbe` | `default14.xbe` |
| `pcsxbox16.xbe` | `default16.xbe` |

启动 `default.xbe`；游戏设置要求换核心时，`default.xbe` 会把自己交给 `default14.xbe`
或 `default16.xbe`。历史发布版把 Reloaded 叫 `defaultr.xbe`，当前代码两个名字都会探测。

需要一个"编三个核心 + 装成测试目录"的一步脚本时，用
`scripts/build_three_cores.sh`（见该脚本头部说明）。
