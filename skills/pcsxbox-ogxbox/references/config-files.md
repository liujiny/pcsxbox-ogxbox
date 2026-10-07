# 配置与测试

## 存档与设置文件

* 存档目录默认 `E:\SAVES\PCSXBOX`。
* 每个游戏一个 `<游戏名>.stg`，**结构体裸写**，不要用文本编辑器手改。
* `.stg` 里存了 `m_psxfix_core_version`、`m_nScreen*`、frameskip、图形修正等；
  换构建/换核心后旧 `.stg` 可能与新结构不匹配，**清一次最省事**。
* 把某个 `.stg` 先备份再改名，下次启动该游戏就会自动进 Game Configuration。

已上传的两份参考文档在仓库 `docs/` 下（**要字段级细节时读这两份，不要在这里重写**）：

| 文档 | 内容 |
|---|---|
| `docs/PCSXBox_stg_format_zh.md` | 逐个字段解释 `.stg`（以 Raiden Project 为例，含偏移、取值、实测推断） |
| `docs/PCSXBox_stg_all_games.tsv` | 每个游戏的设置表 |
| `docs/PCSXBox_frameskip_HOWTO.txt` | 怎么把 frameskip 关成 Off，以及选项到底在哪个菜单 |

仓库地址 `https://github.com/liujiny/pcsxbox-ogxbox`。

## 菜单在哪

**R3 → In Game Options → Configuration → General Settings**（第 1 页，共 2 页）只有：
`Throttle/Fast Forward Speed`、`Slowdown Delay`、`Show FPS? Yes/No`。
**这里没有 Throttle Method，也没有 Frameskip** —— 别在这一页找。

**Game Configuration 是"启动游戏之前"的画面**，不是 R3 菜单的子页：

* 进法：在游戏列表里把光标停在目标游戏上，按 **X**（不是 A）启动。
  `doSelectGame` 里 A 和 X 都是"确认"，按 X 时 `forceConfig=1`，先弹 Game Configuration。
* 或者在还没有 `.stg` 时启动，`loadSettings()` 返回 1，自动进配置画面。

Game Configuration 共 24 行，关 frameskip：

```
第 6 行 "Set graphics fixes"  -> 按 A 进入
  找到 "Frameskip"            -> 按 A 切换，直到显示 Off
按 B 返回，再按 B 退出（退出时 saveSettings() 写进该游戏 .stg）
```

## frameskip 的代码路径

```
src/gpu/src/cfg.c:1552   UseFrameSkip = xbox_get_frameskip();   // GPUopen/ReadConfig 只读一次
src/gpu/src/gpu.c:721    if (g_psxFixFF) UseFrameSkip = xbox_get_frameskip();  // 仅 Throttle Method = Fixed 时每帧重读
```

⇒ 只把 Frameskip 切成 Off 就够了，Throttle Method 不是必须动的。
第 9 行的 `Throttle Method` 想切 `Fixed` 也行。

## 实机验收清单

换构建后按这个流程过一遍：

```
启动模拟器 → 加载 Captain Commando.bin → 画面正常 → 操作正常
→ R3 菜单正常 → 缩略图正常 → 返回游戏正常 → Exit Game 正常
```

主要回归游戏：`Captain Commando.bin`（2D）、`Resident Evil 3`（CHD）、
`Raiden Project`（CHD，屏幕元素密集时看帧数）。

**不要用 CHD 去验证 CHD 之前的构建**（那时没有 CHD 支持）。也不要拿没有源码的
PCSXBox V23 当对照。
