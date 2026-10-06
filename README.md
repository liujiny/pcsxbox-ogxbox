# PCSXBox for the Original Xbox (ogXbox)

A working source tree of the classic **PCSXBox** PlayStation 1 emulator for the
original Microsoft Xbox, rebuilt and verified against the **legacy official Xbox
XDK (5849.17) + Visual C++ 7.1 (`cl 13.10.3077`)** toolchain — *not* RxDK.

This snapshot is the result of a rebuild effort that recovered the project from
a black-screen state back to a fully working emulator on real hardware, and then
added **CHD disc-image support** and two measured performance changes.

Verified on real ogXbox hardware: boot, load a game, correct picture, R3 pause
menu, thumbnail, return to game, exit game — all working.

---

## Repository layout

The original sources lived at `E:\Projects\PlayStation 1 - PCSXBox`, with two
*shared* sibling dependency trees (`E:\Projects\common` and `E:\Common`). To keep
that relative include layout (`..\common\...`, `..\..\Common\...`) intact, and
therefore to keep `pcsxbox.vcproj` openable in Visual Studio .NET 2003 without
edits, this repository mirrors the `E:\` drive directly:

```
.
+-- Common/                          # was E:\Common
|   +-- Include/
|   +-- Src/
+-- Projects/
    +-- common/                      # was E:\Projects\common
    +-- PlayStation 1 - PCSXBox/     # the project itself
```

Only the files actually needed to compile and link are vendored. The `common/`
folder here is a 138-file subset of the original shared folder — the rest of
that folder belonged to unrelated emulators.

---

## Toolchain

| Component | Required |
|---|---|
| Xbox XDK | **Legacy** Microsoft Xbox XDK `5849.17` (`D:\Tools\ogxbox_legacy\XDK\xbox`) |
| Compiler | `bin/vc71/CL.Exe`, `bin/vc71/Link.Exe` (VC7.1, `cl 13.10.3077`) |
| Image builder | `bin/imagebld.exe` |
| Host for the build script | WSL / Linux, driving the Windows binaries through interop |

`build_oldxdk/build.sh` locates the project root from its own path, so the tree
can be checked out anywhere. Override when needed:

```bash
PROJ=/mnt/e/... XDK=/mnt/d/Tools/ogxbox_legacy/XDK/xbox bash build_oldxdk/build.sh
```

---

## Building

From the project directory:

```bash
cd "Projects/PlayStation 1 - PCSXBox"

CORE=15 JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.5 core  -> pcsxbox15.xbe
CORE=14 JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.4 core  -> pcsxbox14.xbe
CORE=16 JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.6 core  -> pcsxbox16.xbe
```

Useful knobs:

| Variable | Meaning |
|---|---|
| `CORE` | `14`, `15` or `16` — which CPU core to build |
| `JOBS` | parallel compile jobs |
| `OUTDIR` | where outputs go (default `build_oldxdk`) |
| `OPT_SET` | `speed` (default, `/O2 /Ob2 /Ot`) or `size` (legacy `/O2 /Ob1 /Os`) |
| `MODE` | `release` (default) or `debug` |
| `EXTRA_DEFINE` | add a `-D` for diagnostics |

A full three-core build takes roughly 15 seconds on a modern machine.

### Output

`build_oldxdk/` receives `pcsxbox<core>.exe`, `pcsxbox<core>.map` and the final
`pcsxbox<core>.xbe`.

### Optional: restore the `default*.xbe` filenames

The emulator chain-loads companion images by fixed name. If you ship all three
cores, name them:

| Built file | Ship as |
|---|---|
| `pcsxbox15.xbe` | `default.xbe` |
| `pcsxbox14.xbe` | `default14.xbe` |
| `pcsxbox16.xbe` | `default16.xbe` |

Launch `default.xbe`. When a game setting sends it elsewhere, the running
`default.xbe` hands off to `default14.xbe` or `default16.xbe`.

---

## CHD support

CHD disc images are supported through a vendored copy of `libchdr` plus the
`CChdFile` wrapper in `src/chd/`, built with `CHDR_SYSTEM_ZLIB`. The CHD sources
are compiled for every core, so no per-core configuration is needed.

Tested on hardware with CHD conversions of *Resident Evil 3* and *Raiden
Project* — both load and run.

---

## Performance changes included

Two changes were benchmarked; both are present here.

**A — software rasteriser inlining** (`src/gpu/src/soft.c`)

`soft.c` defines `#define __inline _inline`, which on VC7.1 means the 13
per-pixel helper functions (`Blit8`, `Blit16`, ...) were never actually inlined.
They are now `__forceinline`.

**B — dynarec block-exit inlining** (`src/ix86*/ir3000a.c`)

The `psxBranchTest` call at basic-block exit (`iBranchTest`) is inlined into the
fast path. An escape hatch is provided: compile with
`EXTRA_DEFINE=PCSXBOX_NO_FAST_BRANCHTEST` to get the un-inlined behaviour back.

---

## Notes for testing on hardware

* `docs/PCSXBox_frameskip_HOWTO.txt` explains how to turn **frameskip off** (so
  the FPS counter reflects real work rather than a skipped frame) and clarifies
  where the relevant options actually live in the R3 menu.
* `docs/PCSXBox_stg_format_zh.md` —— 逐个字段解释每游戏 `.stg` 设置文件
  （以 Raiden Project 为例，含偏移、取值与实测推断）。
* `ANALYSIS_BASELINE3.md`, `ANALYSIS_OPT1.md` and `ANALYSIS_OPT2.md` in the
  project directory record the baseline and the two optimisation steps.

---

## What is deliberately *not* in this repository

* **BIOS dumps** (`scph1001.bin`, ...) — copyrighted, not redistributable.
* **Disc images** (`.bin`, `.iso`, `.cue`, `.chd`) — copyrighted game data.
* Prebuilt `Release/` packages, built `.xbe` / `.exe` / `.map` files and compiler
  object directories. See `.gitignore`.
* `pcint.txt` and `rec2.txt` — large generated PSX address dumps, not source.

You must supply your own BIOS and your own legally obtained game images.

---

## Credits

PCSXBox is the work of the original PCSXBox / XPort authors, built on PCSX.
This repository is a preservation and rebuild snapshot of that codebase, with a
legacy-XDK build script, CHD support and two performance changes.
