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
