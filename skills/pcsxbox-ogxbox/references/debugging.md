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

**规则**：不要混用不同构建的 XBE，也不要用别的构建写过的 `.stg`。三核心整体替换，
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
