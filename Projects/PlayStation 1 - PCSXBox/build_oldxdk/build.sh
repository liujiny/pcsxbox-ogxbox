#!/usr/bin/env bash
# PCSXBox (ogXbox PS1 emulator) -- build with the LEGACY Microsoft Xbox XDK 5849.17 + VC7.1 (cl 13.10.3077)
# Toolchain root: D:\Tools\ogxbox_legacy\XDK   (NOT RXDK)
# cl.exe / link.exe / imagebld.exe are Windows binaries, driven from WSL via interop.
set -uo pipefail

# Project root defaults to the parent directory of this script, so the tree can be
# checked out anywhere.  Override with  PROJ=... XDK=...  if your layout differs.
PROJ="${PROJ:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
XDK="${XDK:-/mnt/d/Tools/ogxbox_legacy/XDK/xbox}"
XDK_WIN="${XDK_WIN:-D:\\Tools\\ogxbox_legacy\\XDK\\xbox}"
CL="$XDK/bin/vc71/CL.Exe"
LINK="$XDK/bin/vc71/Link.Exe"
IMGBLD="$XDK/bin/imagebld.exe"

SCRIPTDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CORE="${CORE:-15}"
export CORE
case "$CORE" in
  15) CORE_DEF=()            ; CORESRC='src'          ; COREINC='src'          ; XBENAME=pcsxbox15.xbe ;;
  14) CORE_DEF=(OLDCORE)     ; CORESRC='src\good'     ; COREINC='src\good'     ; XBENAME=pcsxbox14.xbe ;;
  16) CORE_DEF=(BETACORE)     ; CORESRC='src\1.6'      ; COREINC='src\1.6'      ; XBENAME=pcsxbox16.xbe ;;
  15r|reloaded) CORE_DEF=(RELOADEDCORE) ; CORESRC='src\1.5 (Reloaded)' ; COREINC='src\1.5 (Reloaded)' ; XBENAME=pcsxbox15r.xbe ;;
  *) echo "unknown CORE=$CORE (use 14|15|16)"; exit 2 ;;
esac

MODE="${MODE:-release}"
CFGNAME="$([ "${MODE}" = debug ] && echo 'Debug|Xbox' || echo 'Release|Xbox')"
if [ "$MODE" = "debug" ]; then
  SUBDIR=Debug; OPT=(/Od /Zi); DEFS_EXTRA=(_DEBUG)
  LIBS=(xzlib2.lib xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xboxkrnl.lib xzlib.lib xonlined.lib libsmbd.lib xkbdd.lib)
else
  SUBDIR=Release; DEFS_EXTRA=()
  # OPT_SET=speed matches pcsxbox.vcproj Release (FavorSizeOrSpeed="1" -> /Ot,
  # EnableIntrinsicFunctions="TRUE" -> /Oi).  OPT_SET=size is the old, size-first
  # build (/Ob1 /Os) -- keep it for A/B comparisons:  OPT_SET=size bash build_oldxdk/build.sh
  OPT_SET="${OPT_SET:-speed}"; export OPT_SET   # must reach the --worker sub-processes
  if [ "$OPT_SET" = "size" ]; then OPT=(/O2 /Ob1 /Os /Og); else OPT=(/O2 /Ob2 /Ot /Og); fi
  LIBS=(xzlib2.lib xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xboxkrnl.lib xonline.lib libsmb.lib xkbd.lib)
fi
CODEOPT=(/Gy /GF /MT /W3 /wd4996)
OUTDIR="${OUTDIR:-build_oldxdk}"; OBJDIR="$OUTDIR/obj/$SUBDIR/core$CORE"

INCLUDES=(
  "$XDK_WIN\\include"
  '..\..\Common\include' '..\common' "$COREINC" '..\common\mp3'
  'src\gpu\src' 'src\gpu\src\fpse' '..\common\samba' '..\common\sdl'
  'src\chd' 'src\chd\libchdr\include' 'src\chd\libchdr\src'
)
# The 1.5 (Reloaded) core also needs a *fallback* `src` on the include path: its
# own headers are referenced as "1.5 (Reloaded)\X.h" and psxbios.c pulls sjisfont.h
# out of src\.  COREINC above is searched first, which is what we want.
if [ "$CORE" = "15r" ] || [ "$CORE" = "reloaded" ]; then INCLUDES+=('src'); fi
DEFINES=(WIN32 _USE_XGMATH _XBOX NDEBUG IS_LITTLE_ENDIAN __WIN32__ __i386__ 'PCSX_VERSION="1.4"' _SDL CHDR_SYSTEM_ZLIB)
if [ "${#CORE_DEF[@]}" -gt 0 ]; then DEFINES+=("${CORE_DEF[@]}"); fi
# Optional extra defines for diagnostics, e.g. EXTRA_DEFINE=PCSXBOX_SCREEN_DIAG
if [ -n "${EXTRA_DEFINE:-}" ]; then DEFINES+=("$EXTRA_DEFINE"); fi

IFLAGS=(); for i in "${INCLUDES[@]}"; do IFLAGS+=("/I" "$i"); done
DFLAGS=(); for d in "${DEFINES[@]}" "${DEFS_EXTRA[@]}"; do [ -n "$d" ] && DFLAGS+=("/D" "$d"); done

objname() {
  local b
  b="$(printf '%s' "$1" | tr '\\ ()' '____')"
  while [ "${b#[._]}" != "${b}" ]; do b="${b#?}"; done
  b="${b%.c}"; b="${b%.cpp}"; b="${b%.cxx}"
  printf '%s' "${b}"
}

# ---------------- worker: compile a single translation unit ----------------
if [ "${1:-}" = "--worker" ]; then
  src="$2"
  base="$(objname "$src")"
  obj="$OBJDIR/$base.obj"
  log="$OUTDIR/logs/$base.log"
  if (cd "$PROJ" && "$CL" /nologo /c "${OPT[@]}" "${CODEOPT[@]}" "${DFLAGS[@]}" "${IFLAGS[@]}" "/Fo$obj" "$src") > "$PROJ/$log" 2>&1; then
    echo "OK   $src"
  else
    echo "FAIL $src  ($log)"
  fi
  exit 0
fi

# ---------------- driver: full build ----------------
cd "$PROJ" || exit 1
mkdir -p "$OBJDIR" "$OUTDIR/logs"
JOBS="${JOBS:-8}"

SRCS=$(python3 "$SCRIPTDIR/parse_vcproj.py" "$CFGNAME" | python3 "$SCRIPTDIR/remap_core.py" "$CORE")
# CHD disc-image support (vendored libchdr + the CChdFile wrapper).  These are
# core-independent, so they build once per core into that core's obj dir.
CHD_SRCS=(
  'src\chd\cdchd.cpp'
  'src\chd\libchdr\src\libchdr_chd.c'
  'src\chd\libchdr\src\libchdr_bitstream.c'
  'src\chd\libchdr\src\libchdr_cdrom.c'
  'src\chd\libchdr\src\libchdr_huffman.c'
  'src\chd\libchdr\src\libchdr_flac.c'
  'src\chd\libchdr\src\libchdr_codec_flac.c'
  'src\chd\libchdr\src\libchdr_codec_zlib.c'
  'src\chd\libchdr\src\libchdr_codec_lzma.c'
  'src\chd\libchdr\src\libchdr_codec_huff.c'
  'src\chd\libchdr\src\libchdr_codec_cdfl.c'
  'src\chd\libchdr\src\libchdr_codec_cdlz.c'
  'src\chd\libchdr\src\libchdr_codec_cdzl.c'
  'src\chd\libchdr\deps\lzma-26.02\src\LzmaDec.c'
)
SRCS=$(printf '%s\n' "$SRCS" "${CHD_SRCS[@]}")
NPREP=$(( $(echo "$SRCS" | wc -l) ))

echo "=== [1/3] compile $NPREP units | CORE=$CORE ($CORESRC) | cl 13.10.3077 | XDK 5849.17 | mode=$MODE | jobs=$JOBS ==="
echo "$SRCS" | tr '\n' '\0' \
  | xargs -0 -P "$JOBS" -n1 bash "$SCRIPTDIR/build.sh" --worker \
  | sort | tee "$OUTDIR/logs/compile.txt"
FAILED=$(grep -c '^FAIL' "$OUTDIR/logs/compile.txt" || true)
echo "--- compile failures: ${FAILED:-0} ---"
[ "${FAILED:-0}" -gt 0 ] && { echo "aborting before link"; exit 1; }

echo "=== [2/3] link ==="
PREBUILT=(
  '..\common\mp3\obj\cdctasm.obj' '..\common\mp3\obj\cwin8asm.obj'
  '..\common\mp3\obj\cwinasm.obj' '..\common\mp3\obj\mdctasm.obj'
  '..\common\mp3\obj\msisasm.obj' '.\src\gpu\src\i386.obj'
  '..\common\2xsaiw.obj' '..\common\hq2x16.obj'
)
OBJS=()
while IFS= read -r s; do [ -n "$s" ] && OBJS+=("$OBJDIR/$(objname "$s").obj"); done <<< "$SRCS"
for p in "${PREBUILT[@]}"; do OBJS+=("$p"); done

"$LINK" /nologo /MACHINE:I386 /FIXED:NO /INCREMENTAL:NO /SUBSYSTEM:XBOX \
  "/OUT:$OUTDIR/pcsxbox$CORE.exe" "/MAP:$OUTDIR/pcsxbox$CORE.map" \
  "/LIBPATH:$XDK_WIN\\lib" '/LIBPATH:..\common' \
  /NODEFAULTLIB:LIBC /NODEFAULTLIB:LIBCD /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD \
  "${OBJS[@]}" "${LIBS[@]}" > "$OUTDIR/logs/link.log" 2>&1
LINKRC=$?
tail -5 "$OUTDIR/logs/link.log"
[ $LINKRC -ne 0 ] && { echo "LINK FAILED (see $OUTDIR/logs/link.log)"; exit 1; }

echo "=== [3/3] imagebld -> .xbe ==="
"$IMGBLD" "/IN:$OUTDIR/pcsxbox$CORE.exe" "/OUT:$OUTDIR/$XBENAME" "/MAP:$OUTDIR/pcsxbox$CORE.map" \
  /STACK:0xf0000 /INITFLAGS:0x0 /TESTID:0x08302006 "/TESTNAME:pcsxbox_${CORE}core" \
  /TESTMEDIATYPES:0x80000007 > "$OUTDIR/logs/imagebld.log" 2>&1
cat "$OUTDIR/logs/imagebld.log"
ls -la "$OUTDIR/pcsxbox$CORE.exe" "$OUTDIR/$XBENAME" 2>/dev/null
echo "=== done ==="
