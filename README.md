# PCSXBox for the Original Xbox (ogXbox)

A working source tree of the classic **PCSXBox** PlayStation 1 emulator for the
original Microsoft Xbox, rebuilt and verified against the **legacy official Xbox
XDK (5849.17) + Visual C++ 7.1 (`cl 13.10.3077`)** toolchain — *not* RxDK.

This snapshot is the result of a rebuild effort that recovered the project from
a black-screen state back to a fully working emulator on real hardware, and then
added **CHD disc-image support**, two measured performance changes, and a
CD-ROM **SubQ skew** compatibility fix that repairs the *Captain Commando*
first-boss tile corruption.

Four CPU cores are built out of this single tree (1.4, 1.5, Reloaded, 1.6).

Verified on real ogXbox hardware: boot, load a game, correct picture, R3 pause
menu, thumbnail, return to game, exit game — all working, for every core.
Regression titles: *Captain Commando*, *Resident Evil 3* (CHD), *Raiden
Project* (CHD).

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

CORE=15  JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.5      -> pcsxbox15.xbe
CORE=14  JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.4      -> pcsxbox14.xbe
CORE=16  JOBS=$(nproc) bash build_oldxdk/build.sh   # 1.6      -> pcsxbox16.xbe
CORE=15r OUTDIR=build_oldxdk_15r JOBS=$(nproc) bash build_oldxdk/build.sh   # Reloaded -> pcsxbox15r.xbe
```

`CORE=15r` (alias `reloaded`) is the fourth core, taken from upstream PCSXBox v23
(`RELOADEDCORE`). Its whole core/GPU/SPU implementation lives in dedicated
`src\1.5 (Reloaded)`, `src\gpu\src\1.5 (Reloaded)`, `src\spu\src\1.5 (Reloaded)`
and `src\ix86\1.5 (Reloaded)` directories; `build_oldxdk/remap_core.py` re-points
the shared `.vcproj` file list at them. Give it its own `OUTDIR` so it does not
overwrite the other cores.

Useful knobs:

| Variable | Meaning |
|---|---|
| `CORE` | `14`, `15`, `16` or `15r` — which CPU core to build |
| `JOBS` | parallel compile jobs |
| `OUTDIR` | where outputs go (default `build_oldxdk`) |
| `OPT_SET` | `speed` (default, `/O2 /Ob2 /Ot`) or `size` (legacy `/O2 /Ob1 /Os`) |
| `MODE` | `release` (default) or `debug` |
| `EXTRA_DEFINE` | add a `-D` for diagnostics |

A full four-core build takes a few minutes on a modern machine.

### Output

`build_oldxdk/` receives `pcsxbox<core>.exe`, `pcsxbox<core>.map` and the final
`pcsxbox<core>.xbe`.

### Optional: restore the `default*.xbe` filenames

The emulator chain-loads companion images by fixed name. If you ship all three
cores, name them:

| Core slot | Built file | Ship as |
|---|---|---|
| 1.5 | `pcsxbox15.xbe` | `default.xbe` |
| 1.4 | `pcsxbox14.xbe` | `default14.xbe` |
| Reloaded | `pcsxbox15r.xbe` | `defaultr.xbe` |
| 1.6 | `pcsxbox16.xbe` | `default16.xbe` |

Launch `default.xbe`. When a per-game setting selects another core, the running
`default.xbe` chain-loads the companion image (`pcsxbox.cpp` `rgCoreXbe`: slot 0
= `default14.xbe`, slot 1 = `default.xbe`, slot 2 = `defaultr.xbe`, slot 3 =
`default16.xbe`; slot 2 also accepts the legacy name `reloaded.xbe`).

**If you ship only some cores, ship no companion images at all** — a launch into
a missing `.xbe` fails immediately with a black screen and no R3 menu. Either
copy all four, or delete `default14.xbe` / `defaultr.xbe` / `default16.xbe` and
keep every game on the 1.5 core.

---

## CD-ROM SubQ skew fix (`PCSXBOX_SUBQ_SKEW`)

`CdlGetlocP` (the PS1 `GetlocP` SubQ position query) is answered by a fallback
path in `cdrom.c`, because `CDR__getBufferSub()` always returns `NULL` in this
tree. The fallback historically reported only the current sector, while the real
drive reports **last delivered sector + 2**. Some games depend on that: *Captain
Commando* hardcodes a `GetlocP` query two sectors ahead of the sector it wants,
and without the offset the first-boss tiles are never uploaded.

The four cores are patched in place; the compensation differs per core because
`cdr.Prev` is advanced at a different point in `cdrReadInterrupt()`:

| Core | File | `cdr.Prev` means | Skew |
|---|---|---|---|
| 1.5 (`src`) | `src/cdrom.c` | delivered + 1 | `1` |
| 1.4 (`src/good`) | `src/good/cdrom.c` | delivered + 1 | `1` |
| 1.6 (`src/1.6`) | `src/1.6/CdRom.c` | delivered + 1 | `1` |
| Reloaded | `src/1.5 (Reloaded)/cdrom.c` + `cdrom.h` | delivered | `2` |

Set the macro to `0` (or `EXTRA_DEFINE=PCSXBOX_SUBQ_SKEW=0`) to restore the old
behaviour. Full write-up, including upstream references (DuckStation
`SUBQ_SECTOR_SKEW`, PCSX-ReARMed `SUBQ_FORWARD_SECTORS`, MiSTer issue #66/#303)
and the known **libcrypt** side effect: `docs/PCSXBox_subq_skew_zh.md`.

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
* `docs/PCSXBox_subq_skew_zh.md` —— 名将第一关 BOSS 贴图错乱的根因、四个内核
  的 SubQ skew 相位差异、回退方法与副作用。
* `docs/PCSXBox_stg_format_zh.md` —— 逐个字段解释每游戏 `.stg` 设置文件
  （以 Raiden Project 为例，含偏移、取值与实测推断）。
* `ANALYSIS_BASELINE3.md`, `ANALYSIS_OPT1.md` and `ANALYSIS_OPT2.md` in the
  project directory record the baseline and the two optimisation steps.

---

## Codex skill

`skills/pcsxbox-ogxbox/` packages the rebuild, debugging and optimisation knowledge
of this repository as a [Codex skill](https://github.com/openai/codex), so an agent
can pick it up without re-deriving it. It routes to focused references (build,
source tree, debugging, optimisation, per-game config) and ships
`scripts/build_all_cores.sh`, which builds all four cores and assembles a
`default.xbe` / `default14.xbe` / `defaultr.xbe` / `default16.xbe` test folder with
a SHA256SUMS file. See `skills/README.md`.

---

## The SHA256 of the build that was tested on hardware

These are the four images of the package that was verified on a real console.
They were built from this same source tree at its original location
(`E:\Projects\PlayStation 1 - PCSXBox`), so rebuilding the tree somewhere else
produces different bytes — the linker embeds the source/PDB paths — but the same
code.

| Ship as | Core | SHA256 |
|---|---|---|
| `default.xbe` | 1.5 | `CB9A82057F2CC5747DEEE4A137C3AECBBBD84A9AC30033515358B1544F16FF1D` |
| `default14.xbe` | 1.4 | `6E211ACCA2F99D0D7D2FC62F3B259D2D4897EE122545096A4275E9F3DF53579B` |
| `defaultr.xbe` | Reloaded | `A87040647F86F134327CB068B4E472C2993F7CAAD31E480B327D163EA44644CA` |
| `default16.xbe` | 1.6 | `93F9AE39A734E25D512630C9D6DDCB9DC4948D5D4D7B062A6D80F39862949C2B` |

---

## What is deliberately *not* in this repository

* **BIOS dumps** (`scph1001.bin`, ...) — copyrighted, not redistributable.
* **Disc images** (`.bin`, `.iso`, `.cue`, `.chd`) — copyrighted game data.
* Prebuilt `Release/` packages, built `.xbe` / `.exe` / `.map` files and compiler
  object directories. See `.gitignore`.
* `pcint.txt` and `rec2.txt` — large generated PSX address dumps, not source.
* Per-edit backups (`*.pre_*`) created while debugging — see `.gitignore`.

You must supply your own BIOS and your own legally obtained game images.

---

## Credits

PCSXBox is the work of the original PCSXBox / XPort authors, built on PCSX.
This repository is a preservation and rebuild snapshot of that codebase, with a
legacy-XDK build script, CHD support and two performance changes.
