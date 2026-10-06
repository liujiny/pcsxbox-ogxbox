
import re, sys
cfg = sys.argv[1] if len(sys.argv) > 1 else "Release|Xbox"
s = open("pcsxbox.vcproj", encoding="utf-8", errors="replace").read()
out = []
for m in re.finditer(r'<File\s+RelativePath="([^"]+)"(.*?)</File>', s, re.S):
    rel, body = m.group(1), m.group(2)
    if not re.search(r'\.(c|cpp|cxx)$', rel, re.I):
        continue
    excluded = False
    for fm in re.finditer(r'<FileConfiguration\s+Name="([^"]+)"([^>]*)>(.*?)</FileConfiguration>', body, re.S):
        if fm.group(1) == cfg and 'ExcludedFromBuild="TRUE"' in fm.group(2):
            excluded = True
    if not excluded:
        out.append(rel)
print("\n".join(sorted(set(out))))

