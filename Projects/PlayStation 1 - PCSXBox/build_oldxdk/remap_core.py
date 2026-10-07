import sys, os
CORE = sys.argv[1]
PROJ = "/mnt/e/Projects/PlayStation 1 - PCSXBox"
files = [l.strip() for l in sys.stdin if l.strip()]

def names_in(rel):
    d = os.path.join(PROJ, rel)
    if not os.path.isdir(d): return {}
    return {f.lower(): f for f in os.listdir(d) if f.lower().endswith((".c", ".h"))}

if CORE == "15":
    print("\n".join(files)); sys.exit(0)

# CORE 15r -- 1.5 (Reloaded), the fourth core added in upstream PCSXBox v23
# (RELOADEDCORE).  Its whole core/GPU/SPU implementation sits in dedicated
# "1.5 (Reloaded)" directories; the front-end and ..\common files are shared
# with the other cores, so only the src\ paths are re-pointed here.
if CORE == "15r":
    core = names_in("src/1.5 (Reloaded)")
    ix   = names_in("src/ix86/1.5 (Reloaded)")
    gpu  = names_in("src/gpu/src/1.5 (Reloaded)")
    spu  = names_in("src/spu/src/1.5 (Reloaded)")
    CORE_DIR = "src\\1.5 (Reloaded)"
    IX_DIR   = "src\\ix86\\1.5 (Reloaded)"
    GPU_DIR  = "src\\gpu\\src\\1.5 (Reloaded)"
    SPU_DIR  = "src\\spu\\src\\1.5 (Reloaded)"

    def pick(tbl, base, where):
        t = tbl.get(base)
        if t is None:
            print("MISSING %s in %s" % (base, where), file=sys.stderr); sys.exit(2)
        return t

    out = []
    for f in files:
        n = f.replace("\\", "/")
        if n.startswith("./"): n = n[2:]
        base = os.path.basename(n).lower()
        if not base.endswith((".c", ".cpp", ".cxx")):
            out.append(f); continue
        if n.startswith("src/ix86/"):
            out.append(".\\%s\\%s" % (IX_DIR, pick(ix, base, "src/ix86/1.5 (Reloaded)")))
        elif n.startswith("src/gpu/src/"):
            out.append(".\\%s\\%s" % (GPU_DIR, pick(gpu, base, "src/gpu/src/1.5 (Reloaded)")))
        elif n.startswith("src/spu/src/"):
            out.append(".\\%s\\%s" % (SPU_DIR, pick(spu, base, "src/spu/src/1.5 (Reloaded)")))
        elif n.startswith(("src/win32/", "src/chd/")):
            out.append(f)
        elif n.startswith("src/"):
            out.append(".\\%s\\%s" % (CORE_DIR, pick(core, base, "src/1.5 (Reloaded)")))
        else:
            out.append(f)
    # New files that only exist in the Reloaded core (not in src\ or the .vcproj).
    for extra in ("cdriso.c", "gte_divider.c", "ppf.c"):
        out.append(".\\%s\\%s" % (CORE_DIR, pick(core, extra, "src/1.5 (Reloaded)")))
    for extra in ("externals.c", "xa_new.c"):
        out.append(".\\%s\\%s" % (SPU_DIR, pick(spu, extra, "src/spu/src/1.5 (Reloaded)")))
    print("\n".join(out))
    sys.exit(0)


# CORE 14 (OLDCORE/1.4): interpreter core files come from src/good, but the
# dynarec must come from src/ix86/good, which is src/good/ix86/ir3000a.c PLUS
# recompilerErase() (defined at src/ix86/good/ir3000a.c:2614) - without it the
# shared front-end (src/win32/wndmain.c:133) fails to link.
if CORE == "14":
    sub, ixsub = "src/good", "src/ix86/good"
    ixdir = r"src\ix86\good"
else:
    sub, ixsub = "src/1.6", "src/1.6/ix86"
    ixdir = r"src\1.6\ix86"
top, ix = names_in(sub), names_in(ixsub)
out = []
for f in files:
    n = f.replace("\\", "/")
    if n.startswith("./"): n = n[2:]
    if n.startswith("src/") and not n.startswith(("src/gpu/", "src/spu/", "src/win32/", "src/1.6/", "src/good/")):
        base = os.path.basename(n).lower()
        if base == "padssspsx.c":
            out.append(f); continue
        if n.startswith("src/ix86/"):
            t = ix.get(base)
            if t is None: print("MISSING ix86/%s in %s" % (base, sub), file=sys.stderr); sys.exit(2)
            out.append(".\\%s\\%s" % (ixdir, t))
        else:
            t = top.get(base)
            if t is None: print("MISSING %s in %s" % (base, sub), file=sys.stderr); sys.exit(2)
            out.append(".\\%s\\%s" % (sub.replace("/", "\\"), t))
    else:
        out.append(f)
print("\n".join(out))
