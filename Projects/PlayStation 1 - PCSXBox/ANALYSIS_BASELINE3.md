# PCSXBox OG Xbox — 源码结构与黑屏结论（Baseline 3 分析）

结论先行：**当前源码是"一个前端 + 三套核心"的开发合并工作区，不是 v1.5 Plus 的发布源码。**
发布版的三个 XBE 与当前树的三个 .vcproj 一一对应，但前端 UI/插件版本是更新的、且
UnaiGPU 插件源码不在这棵树里。黑屏是可定位的代码问题，不是玄学。

---

## 1. 发布版 = 3 个 XBE，每个 XBE 内嵌一条源码路径（二进制证据）

对 `G:\download\PlayStation 1 - PCSXBox v1.5 Plus` 三个 XBE 做 `strings`：

| XBE | 内嵌源码路径 | 核心 | SHA256 |
|---|---|---|---|
| default.xbe  | `.\src\plugins.c`      | 1.5 Plus | 9EF9F1CD… |
| default14.xbe| `.\src\good\plugins.c` | 1.4      | 0D9BD52D… |
| defaultr.xbe | `.\src\1.6\plugins.c`  | Reloaded | DC5793A8… |

UI 字符串（UTF-16）也一一对应：
`PCSXBOX 1.5 Plus Core` / `PCSXBOX 1.4 Core` / `PCSXBOX Reloaded Core`，
附加 `v1.5 Plus By Tabajara`、`Reloaded By Tabajara`、`Set UnaiGPU Fixes`。

⇒ **defaultr.xbe = Reloaded core = 源码里的 `src\1.6`**（不是另一个神秘目录）。
   `default16.xbe` 只是当前开发树把 Reloaded 改名后的产物。

## 2. 当前树 = 三套核心 + 多套 GPU 快照的合并工作区

三个 .vcproj 的宏和目录映射（已核对）：

| 工程 | 附加宏 | 核心目录 | 输出 | 平台 ini | 菜单标题 |
|---|---|---|---|---|---|
| `pcsxbox.vcproj`         | (无)     | `src`       | 1.5 | `pcsx.ini`   | PCSXbox 1.5 Main Menu |
| `pcsxbox_oldcore.vcproj` | `OLDCORE`| `src\good`  | 1.4 | `pcsx14.ini` | PCSXbox 1.4 Main Menu |
| `pcsxbox_betacore.vcproj`| `BETACORE`| `src\1.6`  | 1.6 | `pcsx16.ini` | PCSXbox 1.6 Main Menu |

三者共用同一个 `pcsxbox.cpp` 前端，靠宏切换 `cores[]/gpus[]`、ini 名和 3-XBE 调度。
这正是发布版的三 XBE 架构 —— 但 UI 字符串对不上：

* 发布版：`Core Version : %S` → `1.4 / 1.5 Plus / Reloaded`，`GPU Plugin Version : %S`
* 当前树：`cores[] = {"1.4","1.5","1.6"}`，`gpus[] = {"1.12","1.15","1.16"}`
* 当前树**完全没有** `UnaiGPU` / `UnaiGpuOld` / `Reloaded` / `1.5 Plus` 这些字符串
  （全树 grep = 0 命中），也没有 UnaiGPU 插件源码。

⇒ 发布版前端是**更晚的一次改动**（插件化 GPU 命名），其源码不在这棵树里。
   当前树是它的**上游开发快照/合并点**，不是发布点。

其它目录的性质：
* `oldxcore/` — 2009-02 的**独立旧工程**（自带 `pcsxbox.dsp/psx.dsp/.dsw`），
  不参与当前 .vcproj，也不是 default.xbe 的构建来源。**不要**把它当答案。
* `src/1.5_orig`、`newsrc`、`newsrc2`、`orig`、`no`、`gpu17`、
  `src/gpu/src112|src115|src116|src_beforemerge|src_good` — 历史/对比快照。

## 3. 黑屏根因：全屏目标矩形退化成 0×0

`src\gpu\src\draw.c::DoBufferSwap()` → `xbox_put_image()` →
`pcsxbox.cpp::render_to_texture()`。

同一帧里：
```
m_pnlGameScreen.Render( m_gameRectSource.left, m_gameRectSource.top,
                        m_gameRectSource.right, m_gameRectSource.bottom,
                        m_nScreenX, m_nScreenY, m_nScreenMaxX, m_nScreenMaxY );
```
`CPanel::Render(x,y,w,h,x2,y2,w2,h2)`（`common/panel.cpp:658`）语义是
**源矩形 (x,y,w,h) → 目标矩形 (x2,y2) 尺寸 (w2,h2)**。
即 `w2/h2 = m_nScreenMaxX/Y` 是**目标宽高**。

`m_nScreen*` 的写入点只有三处：
1. `doLoadIni()`（`common/commonfuncs.cpp:6627`）默认 `0,0,640,480`
2. `loadSettings()` 的 `fread`（`pcsxbox.cpp:2340`）
3. `extractGameConfig()` 从 launch data 恢复（`pcsxbox.cpp:2898`）

而 `doLoadIni()` 在 `InitializeWithScreen()` 内，`InitializeWithScreen()` 只在菜单循环
（`commonfuncs.cpp:2002  xbApp.FrameMove()` → `pcsxbox.cpp:4454 FrameMove()`）里跑。
**游戏运行期间不跑**。所以进入游戏后 `m_nScreen*` 就停在
`loadSettings()`/`extractGameConfig()` 留下的值上。

R3 暂停菜单的缩略图用的是：
```
m_pnlGameScreen.Render( m_gameRectSource..., (640-248)*multx, 48*multy,
                        200*multx, 150*multy );      // commonfuncs.cpp:7571+
```
**源矩形完全相同，只有目标矩形是写死的常量。**

⇒ 因此"全屏黑、缩略图正常"唯一自洽的解释是：
   **目标矩形退化成 0×0（或非法值）**，四边形画不出来；缩略图因为用常量矩形，照常显示。
   这与之前所有实验的结果一致（换 Sprite->Draw、强制 640×480、强制 200×150、
   去 BeginScene/EndScene、BlockUntilNotBusy、去 clear、去 overlay —— 全都是改绘制方式，
   而问题在参数）。

⇒ 也解释了 inline-1.5 实验"更差"：那条路径绕过了 launch data / 正常 settings 恢复，
   `m_nScreen*` 直接是构造值或陈旧 `.stg` 的值，于是连缩略图路径上的
   `divx` 计算（`commonfuncs.cpp:7565` 用 `m_nScreenMaxX`）也一起失效。

## 4. 多 XBE 跳转：为什么"删掉其它 XBE 就死机"

`pcsxbox.cpp::doCDGame()` 的调度（宏相关，当前 1.5 分支）：
```
case 0 : XLaunchNewImage( "D:\\default14.xbe", ... ); return ;
case 2 : XLaunchNewImage( "D:\\default16.xbe", ... ); return ;
default: break ;      // core==1 -> 本进程 psx_WinMain()
```
`XLaunchNewImage()` 指向不存在的 XBE 时会挂死、回不到菜单 —— 与你看到的现象完全一致。
另外每个游戏的 `.stg` 里存了 `m_psxfix_core_version`，所以"删掉 XBE"是否出问题取决于
该游戏的设置。

**已修**：跳转前先 `stat()`，目标不存在就不跳（并支持发布版命名 `defaultr.xbe`）。

## 5. 已应用的两处修改（本次）

1. `render_to_texture()`（`pcsxbox.cpp:4644` 附近）在 `Render()` 之前，
   若 `m_nScreenMaxX<=0 || m_nScreenMaxY<=0 || m_nScreenX<0 || m_nScreenY<0`，
   钳到 `0,0,640,480`（即 doLoadIni 的默认值）。
2. 前 ~5 秒在画面上打印实际目标矩形：`SCREEN x=.. y=.. w=.. h=.. src=..,..`；
   若触发了修复则打印 `SCREENFIX ...` 约 10 秒。这一次实机就能定论。
3. `XLaunchNewImage` 目标存在性检查。

## 6. 下一步建议（优先级）

1. 先跑本包，确认 `SCREEN` 行。若显示 `SCREENFIX` → 根因确认，基线可用。
2. 测试前把 `E:\SAVES\PCSXBOX` 改名备份，避免读到发布版/旧版写的 `.stg`
   （struct 布局可能不同，会喂进垃圾 `m_nScreen*`）。
3. 基线通过后再谈性能；`init_texture()` 里 `return 1` 之后的死代码只是逻辑冗余，
   不是黑屏原因。

---

## 7. 实测结果（2026-10-06 实机）与根因修正

实机读出：

```
SCREEN x=0 y=0 w=640 h=480 src=640,480
```

* 目标矩形**本来就是合法的 640x480**，钳位保护**没有触发**（没有出现 `SCREENFIX`）。
  ⇒ 第 3 节里"退化成 0x0"的假设，在**单一自洽构建**里并不成立，它是一个**保护网**而非本次修复点。
* 本次真正改变的是：**三个 XBE 全部来自同一份源码/同一套 struct 布局**，
  并且测试前清空了 `E:\SAVES\PCSXBOX`（旧 `.stg`）。

修正后的结论：之前的黑屏来自**跨构建混用**——

1. 旧测试包里放的是「我们编的 `default.xbe`」+「发布版原版 `default14.xbe`/`defaultr.xbe`」；
   1.5 前端跳到发布版 1.4 核心时，`PCSXBOX_LAUNCH_DATA`（`pcsxbox.cpp:2761`，
   头部 757 字节 + 168 字节扩展 + reserved）的字段偏移与发布版二进制不一定一致，
   核心读到错位的画面/参数块。
2. `E:\SAVES\PCSXBOX\*.stg` 里 `m_nScreen*` 等字段来自发布版写的旧布局，
   被当前构建按新布局 `fread`（`pcsxbox.cpp:2340`）→ 进游戏后画面参数错乱。
3. 删掉其它 XBE 就立刻死机，也印证了"确实发生过 `XLaunchNewImage` 到另一个 XBE"。

⇒ **结论：不要混用不同构建的 XBE，也不要用别的构建写的 `.stg`。**
   三核心必须整体替换（本包即如此），换核心时清一次 `E:\SAVES\PCSXBOX`。

## 8. 最终产物

* `Release/PCSXBox_baseline3_final/` — 干净基线（无诊断叠加）。
  只有钳位保护会在真的需要时打印 `SCREENFIX`。
* `Release/PCSXBox_baseline3_test/` — 带 `SCREEN` 诊断的版本，保留备查。
* 全部三个核心 0 编译失败，工程脚本 `build_oldxdk/build.sh`（`CORE=14|15|16`）。
