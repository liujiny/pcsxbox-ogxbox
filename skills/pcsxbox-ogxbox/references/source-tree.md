# 源码结构

## 结论先行

当前树**不是** v1.5 Plus 的发布源码，而是 **1.4 / 1.5 / 1.6(Reloaded) 三套 core 共用一个前端的
开发工作区**。发布版的三个 XBE 与三个 `.vcproj` 一一对应，但前端 UI/插件版本比这棵树更新，
且 UnaiGPU 插件源码不在这里。这是上游开发快照/合并点，不是发布点。

## 三个工程

| 工程 | 附加宏 | core 目录 | 平台 ini | 菜单标题 |
|---|---|---|---|---|
| `pcsxbox.vcproj` | （无） | `src` | `pcsx.ini` | PCSXbox 1.5 Main Menu |
| `pcsxbox_oldcore.vcproj` | `OLDCORE` | `src\good` | `pcsx14.ini` | PCSXbox 1.4 Main Menu |
| `pcsxbox_betacore.vcproj` | `BETACORE` | `src\1.6` | `pcsx16.ini` | PCSXbox 1.6 Main Menu |

三者共用 `pcsxbox.cpp`，靠宏切换 `cores[]` / `gpus[]`、ini 名字和 3-XBE 调度。

## 发布版 vs 当前树的证据

对发布版三个 XBE 做 `strings`，每个都内嵌一条源码路径：

| 发布 XBE | 内嵌源码路径 | 核心 |
|---|---|---|
| `default.xbe` | `.\src\plugins.c` | 1.5 Plus |
| `default14.xbe` | `.\src\good\plugins.c` | 1.4 |
| `defaultr.xbe` | `.\src\1.6\plugins.c` | Reloaded |

⇒ 历史发布版的 `defaultr.xbe`（Reloaded）是用 `src\1.6\` 这份源码编出来的，
不是另一个神秘目录。

**2026-10 更新**：当前开发树已经把上游 PCSXBox v23 的第四核心——真正独立的
**Reloaded 核**（`RELOADEDCORE`）——移植进来了，源码在 `src\1.5 (Reloaded)\`，
构建用 `CORE=15r`，产物 `pcsxbox15r.xbe` → `defaultr.xbe`。`src\1.6` 仍然是独立的
1.6 核（`BETACORE` → `default16.xbe`）。两者**不是**同一份代码，四个核心可以共存：
发布版那三个 XBE 的内嵌路径只说明**当年的发布版**怎么编的，不代表今天的目录含义。

版本字符串对不上：

* 发布版：`Core Version : 1.4 / 1.5 Plus / Reloaded`，GPU 走 `UnaiGpuOld` / `UnaiGPU`。
* 当前树：`cores[] = {"1.4","1.5","1.6"}`，`gpus[] = {"1.12","1.15","1.16"}`；
  全树 grep `UnaiGPU` / `Reloaded` / `1.5 Plus` = 0 命中。

⇒ 发布版前端是**更晚的一次改动**（GPU 插件化命名），其源码不在这棵树里。

## 目录性质

| 目录 | 性质 |
|---|---|
| `pcsxbox.cpp` + `src` | 前端 + 1.5 core，当前默认编译目标 |
| `src\good` | 1.4 core（`OLDCORE`） |
| `src\1.6` | 1.6 core（`BETACORE`），产物 `default16.xbe` |
| `src\1.5 (Reloaded)` | Reloaded core（`RELOADEDCORE`），产物 `defaultr.xbe`；`CORE=15r`，GPU/SPU/dynarec 分别在 `src\gpu\src\1.5 (Reloaded)` / `src\spu\src\1.5 (Reloaded)` / `src\ix86\1.5 (Reloaded)`，由 `build_oldxdk/remap_core.py` 重映射 |
| `src\1.5_orig`、`orig`、`newsrc`、`newsrc2`、`no`、`gpu17` | 历史/对比快照，**不参与当前 .vcproj** |
| `src\gpu\src112` / `src115` / `src116` / `src_beforemerge` / `src_good` | GPU 插件多版本快照；当前编译的是 `src\gpu\src` |
| `oldxcore\` | 2009-02 的**独立旧工程**（自带 `pcsxbox.dsp` / `psx.dsp` / `.dsw`），不参与当前 .vcproj，**不是**任何发布 XBE 的构建来源。不要把它当答案 |
| `build_oldxdk\`、`Release\`、`Media\` | 构建输出与打包目录 |

## 多 XBE 跳转

`pcsxbox.cpp` 里 `doCDGame()` 附近的调度（行号随改动漂移，用 grep 定位）：

```c
static const char *rgCoreXbe[4] =
{
    "D:\\default14.xbe", "D:\\default.xbe",
    "D:\\defaultr.xbe",  "D:\\default16.xbe"
} ;
/* slot 0=1.4  1=1.5(本进程)  2=Reloaded  3=1.6 ... */
```

槽位表 `rgCoreXbe[4]` 与 `.stg` 里的 `m_psxfix_core_version` 一一对应：
`0 → default14.xbe`、`1 → default.xbe`、`2 → defaultr.xbe`（旧名 `reloaded.xbe` 也会探测）、
`3 → default16.xbe`。只有 `m_psxfix_core_version != PCSXBOX_DEFAULT_CORE` 时才跳转。

每个游戏的 `.stg` 里存了 `m_psxfix_core_version`，所以**启动到哪个核心由该游戏设置决定**，
不是全局开关。

两个必须知道的坑：

1. `XLaunchNewImage()` 指向不存在的 XBE 会**挂死**，回不到菜单。所以跳转前要先
   `PCSXBoxXbePresent()`（`stat`）探测；已经在跳转点加了存在性检查，并兼容发布版命名
   `defaultr.xbe`。这就是"把测试目录里除了 `default.xbe` 之外的 XBE 全删掉后一选游戏
   就死机"的原因。
2. 只改了 `default.xbe` 的构建，跑起来可能根本不是它 —— 游戏 `.stg` 可能把你送去
   `default14.xbe` / `defaultr.xbe` / `default16.xbe`。做实验时要么四份整体替换，
   要么强制 `m_psxfix_core_version = 1` 并禁掉跳转。
3. 反过来，**只放一份 `default.xbe`、不放任何伴生 XBE** 也危险：`.stg` 要求换核时
   会跳到不存在的文件，表现为"一选游戏就黑屏、连 R3 菜单都出不来"。要么四份齐全，
   要么把四个 `.stg` / 全局默认核心都钉在 1.5。
