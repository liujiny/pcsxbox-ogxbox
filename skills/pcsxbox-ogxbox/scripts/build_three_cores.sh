#!/usr/bin/env bash
# Build all three PCSXBox cores and assemble one drop-in test folder.
#
#   bash scripts/build_three_cores.sh
#
# The heavy lifting is done by the project's own build_oldxdk/build.sh; this
# script only drives it three times and renames the results into the fixed
# filenames the emulator chain-loads by.
#
# Env overrides:
#   PROJ    project root      (default: the known local checkout)
#   JOBS    parallel jobs     (default: nproc)
#   NAME    folder under Release/  (default: PCSXBox_build)
#   OPT_SET speed|size        (default: speed, forwarded to build.sh)
#
# Produces  Release/$NAME/{default.xbe,default14.xbe,default16.xbe}
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
OUTDIR="build_oldxdk"
DEST="$PROJ/Release/$NAME"

[ -x "$BUILDSH" ] || { echo "missing $BUILDSH"; exit 2; }
[ -e "$DEST" ] && { echo "$DEST already exists - pick another NAME= ; not overwriting"; exit 2; }

echo "=== building three cores (JOBS=$JOBS OPT_SET=${OPT_SET:-speed}) ==="
for CORE in 15 14 16; do
  echo "--- CORE=$CORE ---"
  ( cd "$PROJ" && CORE="$CORE" JOBS="$JOBS" OUTDIR="$OUTDIR" bash "$BUILDSH" ) \
    | tail -6
done

declare -A SRC=( [15]=pcsxbox15.xbe [14]=pcsxbox14.xbe [16]=pcsxbox16.xbe )
declare -A DST=( [15]=default.xbe  [14]=default14.xbe [16]=default16.xbe )

mkdir -p "$DEST"
for CORE in 15 14 16; do
  from="$PROJ/$OUTDIR/${SRC[$CORE]}"
  [ -f "$from" ] || { echo "missing build product: $from"; exit 1; }
  cp -f "$from" "$DEST/${DST[$CORE]}"
done

( cd "$DEST" && sha256sum default.xbe default14.xbe default16.xbe > SHA256SUMS.txt )
ls -l "$DEST"
cat "$DEST/SHA256SUMS.txt"

cat <<EOF

Assembled: $DEST
Copy the three .xbe into the emulator folder, replacing the existing trio
as a unit.  Do NOT mix this build with XBEs from another build, and clear
E:\\SAVES\\PCSXBOX before first run.
EOF
