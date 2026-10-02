#!/usr/bin/env python3
import argparse
import bisect
import os
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from cwdemangle import demangle
from merge_lst import REL_SECTIONS, rel_module_ids

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONFIG_DIR = os.path.join(REPO, "tp", "config")
FORMAT_TAG = "TPGZ1"
TPGZ_FIRST_MODULE_ID = 0x1000
SECTION_NAMES = {index: name for name, index in REL_SECTIONS.items()}
GAME_VERSIONS = {
    0: ("GCN_NTSCU", "GZ2E01"),
    1: ("GCN_PAL", "GZ2P01"),
    2: ("GCN_NTSCJ", "GZ2J01"),
    3: ("WII_NTSCU_10", "RZDE01_00"),
    4: ("WII_NTSCU_12", "RZDE01_02"),
    5: ("WII_PAL", "RZDP01"),
    6: ("WII_NTSCJ", "RZDJ01"),
}
ERRORS = [
    "system reset", "machine check", "DSI", "ISI", "external interrupt", "alignment", "program",
    "floating point", "decrementer", "system call", "trace", "performance monitor", "breakpoint",
    "system interrupt", "thermal interrupt",
]
SYMBOL_RE = re.compile(r"^(\S+) = (\S+?):0x([0-9A-Fa-f]+);(?:.*?size:0x([0-9A-Fa-f]+))?")
SPLIT_FILE_RE = re.compile(r"^(\S.*):$")
SPLIT_RANGE_RE = re.compile(r"^\s+(\S+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)")


class SymbolTable:
    def __init__(self):
        self.symbols = {}
        self.files = {}

    def add(self, section, start, size, name):
        self.symbols.setdefault(section, []).append((start, size, name))

    def finish(self):
        for section, entries in self.symbols.items():
            entries.sort()
            ends = [e[0] for e in entries[1:]] + [entries[-1][0] + 1]
            self.symbols[section] = [(s, z or max(end - s, 1), n) for (s, z, n), end in zip(entries, ends)]
        return self

    def describe(self, section, offset):
        entries = self.symbols.get(section, [])
        i = bisect.bisect_right(entries, (offset, float("inf"))) - 1
        if i < 0 or offset >= entries[i][0] + entries[i][1]:
            return None
        start, _, name = entries[i]
        text = demangle(name) + (f"+0x{offset - start:X}" if offset > start else "")
        path = next((p for s, e, p in self.files.get(section, []) if s <= offset < e), None)
        return f"{text}  [{path}]" if path else text


def load_decomp(path, sections=None):
    table = SymbolTable()
    with open(os.path.join(path, "symbols.txt")) as f:
        for m in filter(None, map(SYMBOL_RE.match, f)):
            section = 0 if sections is None else sections.get(m[2])
            if section is not None:
                table.add(section, int(m[3], 16), int(m[4] or "0", 16), m[1])
    splits = os.path.join(path, "splits.txt")
    current = None
    for line in open(splits) if os.path.exists(splits) else []:
        m = SPLIT_FILE_RE.match(line)
        if m:
            current = None if m[1] == "Sections" else m[1]
            continue
        m = SPLIT_RANGE_RE.match(line)
        section = m and (0 if sections is None else sections.get(m[1]))
        if current and section is not None:
            table.files.setdefault(section, []).append((int(m[2], 16), int(m[3], 16), current))
    return table.finish()


def load_elf(path):
    table = SymbolTable()
    with open(path, "rb") as f:
        data = f.read()
    shoff, = struct.unpack_from(">I", data, 0x20)
    shnum, = struct.unpack_from(">H", data, 0x30)
    headers = [struct.unpack_from(">10I", data, shoff + i * 40) for i in range(shnum)]
    for header in (h for h in headers if h[1] == 2):
        strtab = headers[header[6]][4]
        for pos in range(header[4], header[4] + header[5], 16):
            name, value, size, info, _, shndx = struct.unpack_from(">IIIBBH", data, pos)
            if (info & 0xF) in (1, 2) and 0 < shndx < 0xFF00:
                end = data.index(b"\0", strtab + name)
                table.add(shndx, value, size, data[strtab + name:end].decode(errors="replace"))
    return table.finish()


def find_tpgz_modules(build_dir):
    rels, elves = {}, {}
    for root, _, files in os.walk(os.path.join(build_dir, "modules")):
        for name in files:
            path = os.path.join(root, name)
            if name.endswith(".rel"):
                with open(path, "rb") as f:
                    rels[name[:-4]] = struct.unpack(">I", f.read(4))[0]
            elif name == os.path.basename(root):
                with open(path, "rb") as f:
                    if f.read(4) == b"\x7fELF":
                        elves[name] = path
    return {module_id: (name, elves[name]) for name, module_id in rels.items() if name in elves}


class Symbolizer:
    def __init__(self, build, decomp_version, build_dir):
        version_dir = os.path.join(CONFIG_DIR, decomp_version)
        rel_ids = os.path.join(REPO, "res", "map", "rel_ids", build + ".lst")
        game_rels = rel_module_ids(CONFIG_DIR, decomp_version, rel_ids if os.path.exists(rel_ids) else None)
        self.dol = load_decomp(version_dir)
        self.modules = {i: (n, os.path.join(version_dir, "rels", n)) for n, i in game_rels.items()}
        self.modules.update(find_tpgz_modules(build_dir) if build_dir else {})
        self.tables = {}

    def module(self, module_id):
        if module_id not in self.modules:
            return None, None
        name, path = self.modules[module_id]
        if module_id not in self.tables:
            is_tpgz = module_id >= TPGZ_FIRST_MODULE_ID
            self.tables[module_id] = load_elf(path) if is_tpgz else load_decomp(path, REL_SECTIONS)
        return f"{name}.rel", self.tables[module_id]

    def address(self, value):
        if not isinstance(value, tuple):
            return f"{value:08X}"
        module_id, section, offset = value
        name = self.module(module_id)[0] or f"module {module_id:X}"
        return f"{name} {SECTION_NAMES.get(section, f'section{section}')}+0x{offset:X}"

    def describe(self, value):
        if not isinstance(value, tuple):
            return self.dol.describe(0, value)
        module_id, section, offset = value
        table = self.module(module_id)[1]
        return table.describe(section, offset) if table else None


def parse_value(token):
    parts = token.split(".")
    return tuple(int(p, 16) for p in parts) if len(parts) == 3 else int(token, 16)


def scan_image(path):
    try:
        import zxingcpp
        from PIL import Image
    except ImportError:
        sys.exit("reading images needs: pip install zxing-cpp pillow")
    for result in zxingcpp.read_barcodes(Image.open(path)):
        if result.text.startswith(FORMAT_TAG):
            return result.text
    sys.exit("no TPGZ crash QR code found in the image")


def find_build_dir(build):
    platform, region = build.split("_", 1)
    for name in sorted(os.listdir(REPO)):
        cache = os.path.join(REPO, name, "CMakeCache.txt")
        if os.path.exists(cache):
            text = open(cache).read()
            if re.search(rf"^PLATFORM:\w+={platform}$", text, re.M) and re.search(rf"^REGION:\w+={region}$", text, re.M):
                return os.path.join(REPO, name)
    return None


def repo_version():
    git = lambda *args: subprocess.run(["git", *args], cwd=REPO, capture_output=True, text=True).stdout.strip()
    try:
        return git("rev-parse", "--short", "HEAD") + ("-dirty" if git("status", "--porcelain") else "")
    except OSError:
        return None


def main(argv):
    parser = argparse.ArgumentParser(description="Decode a TPGZ crash QR code into a symbolized stack trace.")
    parser.add_argument("image", nargs="?", help="screenshot or photo containing the crash QR code")
    parser.add_argument("--text", help="the decoded QR text, instead of an image")
    parser.add_argument("--build-dir", help="TPGZ build directory matching the crashed build (for TPGZ symbols)")
    args = parser.parse_args(argv[1:])
    if not args.image and not args.text:
        parser.error("give an image or --text")

    text = args.text or scan_image(args.image)
    tokens = text.split()
    if len(tokens) < 40 or tokens[0] != FORMAT_TAG:
        sys.exit(f"not a TPGZ crash code: {text[:60]}")
    version, game_version, error = tokens[1].lower(), int(tokens[2], 16), int(tokens[3], 16)
    if game_version not in GAME_VERSIONS:
        sys.exit(f"unknown game version {game_version}")
    values = [parse_value(t) for t in tokens[4:]]
    build, decomp_version = GAME_VERSIONS[game_version]
    build_dir = args.build_dir or find_build_dir(build)
    symbolizer = Symbolizer(build, decomp_version, build_dir)

    error_name = ERRORS[error] if error < len(ERRORS) else f"error {error}"
    print(f"TPGZ {version} {build} ({decomp_version}): {error_name} exception")
    current = repo_version()
    if not build_dir:
        print("warning: no matching build directory found, TPGZ addresses will not be symbolized (use --build-dir)")
    elif current and current != version:
        print(f"warning: crash is from {version} but the checkout is {current}, TPGZ symbols may be wrong")

    width = max(len(symbolizer.address(v)) for v in values) + 2

    def line(label, value):
        print(f"  {label:<6}{symbolizer.address(value):<{width}}{symbolizer.describe(value) or ''}".rstrip())

    print()
    line("pc", values[0])
    line("lr", values[1])
    print(f"  {'dsisr':<6}{values[2]:08X}\n  {'dar':<6}{values[3]:08X}\n\nregisters:")
    for i, value in enumerate(values[4:36]):
        line(f"r{i}", value)
    print("\nstack trace:")
    for i, value in enumerate(values[36:]):
        line(str(i), value)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
