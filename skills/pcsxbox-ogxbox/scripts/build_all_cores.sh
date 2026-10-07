#!/usr/bin/env bash
# Build all four PCSXBox cores and assemble one drop-in test folder.
#
#   bash scripts/build_all_cores.sh
#
# The heavy lifting is done by the project's own build_oldxdk/build.sh; this
# script only drives it four times and renames the results into the fixed
# filenames the emulator chain-loads by.
#
# Env overrides:
#   PROJ    project root      (default: the known local checkout)
#   JOBS    parallel jobs     (default: nproc)
#   NAME    folder under Release/  (default: PCSXBox_build)
#   OPT_SET speed|size        (default: speed, forwarded to build.sh)
#
# Produces  Release/$NAME/{default.xbe,default14.xbe,defaultr.xbe,default16.xbe}
# and       Release/$NAME/SHA256SUMS.txt
#
# Nothing is installed on the console and no existing Release/ folder is
# overwritten: a fresh $NAME directory is required to be absent, or you pass a
# new NAME.  Verify on hardware before shipping.
set -euo pipefail

PROJ="${PROJ:-/mnt/e/Projects/PlayStation 1 - PCSXBox}"
JOBS="${JOBS:-$(nproc)}"
NAME="${NAME:-PCSXBox_build}"
BUILDSH="$PROJ/build_oldxdk/build.sh"
DEST="$PROJ/Release/$NAME"

[ -x "$BUILDSH" ] || { echo "missing $BUILDSH"; exit 2; }
[ -e "$DEST" ] && { echo "$DEST already exists - pick another NAME= ; not overwriting"; exit 2; }

# core slot | CORE value | .xbe produced | OUTDIR
#   slot 0 = 1.4        slot 1 = 1.5      slot 2 = Reloaded   slot 3 = 1.6
# CORE=15r gets its own OUTDIR: it compiles a *different* set of source files
# into the same object-name space, so sharing build_oldxdk/obj would collide.
CORES=(15 14 16 15r)
OUTDIRS=(build_oldxdk build_oldxdk build_oldxdk build_oldxdk_15r)
XBES=(pcsxbox15.xbe pcsxbox14.xbe pcsxbox16.xbe pcsxbox15r.xbe)
DEFAULTXBES=(default.xbe default14.xbe default16.xbe defaultr.xbe)

echo "=== building four cores (JOBS=$JOBS OPT_SET=${OPT_SET:-speed}) ==="
for i in "${!CORES[@]}"; do
  echo "--- CORE=${CORES[$i]} ---"
  ( cd "$PROJ" && CORE="${CORES[$i]}" JOBS="$JOBS" OUTDIR="${OUTDIRS[$i]}" bash "$BUILDSH" ) \
    | tail -6
done

mkdir -p "$DEST"
for i in "${!CORES[@]}"; do
  from="$PROJ/${OUTDIRS[$i]}/${XBES[$i]}"
  [ -f "$from" ] || { echo "missing build product: $from"; exit 1; }
  cp -f "$from" "$DEST/${DEFAULTXBES[$i]}"
done

( cd "$DEST" && sha256sum default.xbe default14.xbe defaultr.xbe default16.xbe > SHA256SUMS.txt )
ls -l "$DEST"
cat "$DEST/SHA256SUMS.txt"

cat <<EOM

Assembled: $DEST
Copy the four .xbe into the emulator folder, replacing the existing set as a
whole.  Do NOT mix this build with XBEs from another build, and clear
E:\\SAVES\\PCSXBOX before first run.

If you deliberately ship fewer cores, ship NO companion XBE at all: a launch
into a missing default14/defaultr/default16.xbe fails instantly with a black
screen and no R3 menu.  Either copy all four, or delete all three companions
and keep every game on the 1.5 core.
EOM
