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
FORMAT_TAG = "TPGZ1"
SECTION_NAMES = {index: name for name, index in REL_SECTIONS.items()}
TPGZ_FIRST_MODULE_ID = 0x1000
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
        self.entries = {}
        self.files = {}

    def add(self, section, offset, size, name):
        self.entries.setdefault(section, []).append((offset, size, name))

    def add_file(self, section, start, end, path):
        self.files.setdefault(section, []).append((start, end, path))

    def finish(self):
        for entries in self.entries.values():
            entries.sort()
        for files in self.files.values():
            files.sort()

    def lookup(self, section, offset, exact):
        entries = self.entries.get(section, [])
        i = bisect.bisect_right(entries, (offset, float("inf"), "")) - 1
        if i < 0:
            return None
        start, size, name = entries[i]
        if exact and offset >= start + max(size, 1):
            return None
        return name, offset - start

    def file(self, section, offset):
        for start, end, path in self.files.get(section, []):
            if start <= offset < end:
                return path
        return None


def read_decomp_symbols(table, symbols_path, section_ids=None):
    with open(symbols_path) as f:
        for line in f:
            m = SYMBOL_RE.match(line)
            if not m:
                continue
            section = m[2] if section_ids is None else section_ids.get(m[2])
            if section is not None:
                table.add(section, int(m[3], 16), int(m[4] or "0", 16), m[1])


def read_splits(splits_path):
    if not os.path.exists(splits_path):
        return
    current = None
    with open(splits_path) as f:
        for line in f:
            m = SPLIT_FILE_RE.match(line)
            if m:
                current = None if m[1] == "Sections" else m[1]
                continue
            m = SPLIT_RANGE_RE.match(line)
            if m and current:
                yield current, m[1], int(m[2], 16), int(m[3], 16)


def read_decomp_splits(table, splits_path, section_ids=None):
    for path, name, start, end in read_splits(splits_path):
        section = name if section_ids is None else section_ids.get(name)
        if section is not None:
            table.add_file(section, start, end, path)


def read_elf_symbols(path):
    table = SymbolTable()
    with open(path, "rb") as f:
        data = f.read()
    shoff = struct.unpack_from(">I", data, 0x20)[0]
    shnum = struct.unpack_from(">H", data, 0x30)[0]
    headers = [struct.unpack_from(">10I", data, shoff + i * 40) for i in range(shnum)]
    for header in headers:
        if header[1] != 2:
            continue
        strtab = headers[header[6]]
        for pos in range(header[4], header[4] + header[5], 16):
            name_off, value, size, info, _, shndx = struct.unpack_from(">IIIBBH", data, pos)
            if (info & 0xF) not in (1, 2) or not 0 < shndx < 0xFF00:
                continue
            start = strtab[4] + name_off
            name = data[start:data.index(b"\0", start)].decode(errors="replace")
            table.add(shndx, value, size, name)
    table.finish()
    return table


def is_elf(path):
    try:
        with open(path, "rb") as f:
            return f.read(4) == b"\x7fELF"
    except OSError:
        return False


class Symbolizer:
    def __init__(self, decomp_version, rel_ids_path, build_dir):
        config_dir = os.path.join(REPO, "tp", "config")
        version_dir = os.path.join(config_dir, decomp_version)
        self.dol = SymbolTable()
        read_decomp_symbols(self.dol, os.path.join(version_dir, "symbols.txt"))
        read_decomp_splits(self.dol, os.path.join(version_dir, "splits.txt"))
        self.dol.finish()
        self.dol_sections = self.read_dol_sections(os.path.join(version_dir, "splits.txt"))

        self.game_modules = {}
        rel_ids = rel_ids_path if os.path.exists(rel_ids_path) else None
        for name, module_id in rel_module_ids(config_dir, decomp_version, rel_ids).items():
            self.game_modules[module_id] = (name, os.path.join(version_dir, "rels", name))
        self.game_tables = {}

        self.tpgz_modules = {}
        if build_dir:
            self.find_tpgz_modules(build_dir)
        self.tpgz_tables = {}

    @staticmethod
    def read_dol_sections(splits_path):
        sections = {}
        for _, name, start, end in read_splits(splits_path):
            lo, hi = sections.get(name, (start, end))
            sections[name] = (min(lo, start), max(hi, end))
        return sections

    def find_tpgz_modules(self, build_dir):
        elves = {}
        rels = {}
        for root, _, files in os.walk(os.path.join(build_dir, "modules")):
            for name in files:
                path = os.path.join(root, name)
                if name.endswith(".rel"):
                    with open(path, "rb") as f:
                        rels[name[:-4]] = struct.unpack(">I", f.read(4))[0]
                elif name == os.path.basename(root) and is_elf(path):
                    elves[name] = path
        for name, module_id in rels.items():
            if name in elves:
                self.tpgz_modules[module_id] = (name, elves[name])

    def module_table(self, module_id):
        if module_id >= TPGZ_FIRST_MODULE_ID:
            if module_id not in self.tpgz_modules:
                return None, None
            name, path = self.tpgz_modules[module_id]
            if module_id not in self.tpgz_tables:
                self.tpgz_tables[module_id] = read_elf_symbols(path)
            return f"{name}.rel", self.tpgz_tables[module_id]
        if module_id not in self.game_modules:
            return None, None
        name, path = self.game_modules[module_id]
        if module_id not in self.game_tables:
            table = SymbolTable()
            read_decomp_symbols(table, os.path.join(path, "symbols.txt"), REL_SECTIONS)
            read_decomp_splits(table, os.path.join(path, "splits.txt"), REL_SECTIONS)
            table.finish()
            self.game_tables[module_id] = table
        return f"{name}.rel", self.game_tables[module_id]

    def format_value(self, value):
        if not isinstance(value, tuple):
            return f"{value:08X}"
        module_id, section, offset = value
        module, _ = self.module_table(module_id)
        section_name = SECTION_NAMES.get(section, f"section{section}")
        if module is None:
            return f"module {module_id:X} {section_name}+0x{offset:X}"
        return f"{module} {section_name}+0x{offset:X}"

    def describe(self, value, exact):
        if isinstance(value, tuple):
            module_id, section, offset = value
            _, table = self.module_table(module_id)
            return describe_symbol(table, section, offset, exact) if table else None
        for section, (start, end) in self.dol_sections.items():
            if start <= value < end:
                return describe_symbol(self.dol, section, value, exact)
        return None


def describe_symbol(table, section, offset, exact):
    found = table.lookup(section, offset, exact)
    if found is None:
        return None
    name, delta = found
    text = f"{demangle(name)}+0x{delta:X}" if delta else demangle(name)
    where = table.file(section, offset)
    return f"{text}  [{where}]" if where else text


def parse_value(token):
    parts = token.split(".")
    if len(parts) == 3:
        return tuple(int(p, 16) for p in parts)
    return int(token, 16)


def load_image(path):
    try:
        import cv2
        image = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
    except ImportError:
        try:
            from PIL import Image
            image = Image.open(path).convert("L")
        except ImportError:
            sys.exit("reading images needs opencv-python or pillow")
        except OSError:
            image = None
    if image is None:
        sys.exit(f"could not read {path}")
    return image


def scan_image(path):
    image = load_image(path)
    try:
        import zxingcpp
    except ImportError:
        zxingcpp = None
    if zxingcpp:
        for result in zxingcpp.read_barcodes(image):
            if result.text.startswith(FORMAT_TAG):
                return result.text
    else:
        import cv2
        detector = cv2.QRCodeDetector()
        for scale in (1, 2, 0.5, 3):
            scaled = image if scale == 1 else cv2.resize(image, None, fx=scale, fy=scale,
                                                         interpolation=cv2.INTER_CUBIC)
            text, _, _ = detector.detectAndDecode(scaled)
            if text:
                return text
    sys.exit("no TPGZ crash QR code found in the image")


def find_build_dir(build):
    platform, region = build.split("_", 1)
    for name in sorted(os.listdir(REPO)):
        cache = os.path.join(REPO, name, "CMakeCache.txt")
        if not os.path.exists(cache):
            continue
        with open(cache) as f:
            text = f.read()
        if re.search(rf"^PLATFORM:\w+={platform}$", text, re.M) and re.search(rf"^REGION:\w+={region}$", text, re.M):
            return os.path.join(REPO, name)
    return None


def repo_version():
    try:
        head = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=REPO, capture_output=True,
                              text=True).stdout.strip()
        dirty = subprocess.run(["git", "status", "--porcelain"], cwd=REPO, capture_output=True,
                               text=True).stdout.strip()
        return head + ("-dirty" if dirty else "")
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

    text = args.text if args.text else scan_image(args.image)
    tokens = text.split()
    if len(tokens) < 40 or tokens[0] != FORMAT_TAG:
        sys.exit(f"not a TPGZ crash code: {text[:60]}")
    version, game_version, error = tokens[1].lower(), int(tokens[2], 16), int(tokens[3], 16)
    values = [parse_value(t) for t in tokens[4:]]
    pc, lr, dsisr, dar = values[:4]
    registers = values[4:36]
    backtrace = values[36:]

    if game_version not in GAME_VERSIONS:
        sys.exit(f"unknown game version {game_version}")
    build, decomp_version = GAME_VERSIONS[game_version]
    build_dir = args.build_dir or find_build_dir(build)
    rel_ids = os.path.join(REPO, "res", "map", "rel_ids", build + ".lst")
    symbolizer = Symbolizer(decomp_version, rel_ids, build_dir)

    error_name = ERRORS[error] if error < len(ERRORS) else f"error {error}"
    print(f"TPGZ {version} {build} ({decomp_version}): {error_name} exception")
    current = repo_version()
    if not build_dir:
        print("warning: no matching build directory found, TPGZ addresses will not be symbolized (use --build-dir)")
    elif current and current != version:
        print(f"warning: crash is from {version} but the checkout is {current}, TPGZ symbols may be wrong")
    print()

    width = max(len(symbolizer.format_value(v)) for v in values) + 2

    def line(label, value, exact):
        info = symbolizer.describe(value, exact)
        print(f"  {label:<6}{symbolizer.format_value(value):<{width}}{info or ''}".rstrip())

    line("pc", pc, False)
    line("lr", lr, False)
    print(f"  {'dsisr':<6}{dsisr:08X}")
    print(f"  {'dar':<6}{dar:08X}")
    print("\nregisters:")
    for i, value in enumerate(registers):
        line(f"r{i}", value, True)
    print("\nstack trace:")
    for i, value in enumerate(backtrace):
        line(str(i), value, False)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
