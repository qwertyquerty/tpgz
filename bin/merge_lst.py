#!/usr/bin/env python3
import argparse
import re
import sys

REL_SECTIONS = {".text": 1, ".ctors": 2, ".dtors": 3, ".rodata": 4, ".data": 5, ".bss": 6}
SYMBOL_RE = re.compile(r"^(\S+) = (\S+?):0x([0-9A-Fa-f]+);")


def read_decomp_symbols(path):
    entries = []
    with open(path, "r") as f:
        for line in f:
            m = SYMBOL_RE.match(line)
            if m:
                entries.append((m[1], m[2], int(m[3], 16)))
    return entries


def read_rel_ids(path):
    excluded, pinned = set(), {}
    if path is None:
        return excluded, pinned
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//"):
                continue
            name, value = line.split(":")
            if value == "none":
                excluded.add(name)
            else:
                pinned[name] = int(value, 0)
    return excluded, pinned


def rel_module_ids(config_dir, version, rel_ids_path):
    with open(f"{config_dir}/{version}/config.yml", "r") as f:
        modules = re.findall(r"symbols: config/" + re.escape(version) + r"/rels/([^/]+)/symbols.txt", f.read())
    excluded, pinned = read_rel_ids(rel_ids_path)
    order = [m for m in modules if m not in excluded and m not in pinned]
    for name, module_id in sorted(pinned.items(), key=lambda kv: kv[1]):
        order.insert(module_id - 1, name)
    return {name: i + 1 for i, name in enumerate(order)}


def read_decomp(config_dir, version, rel_ids_path):
    dol = read_decomp_symbols(f"{config_dir}/{version}/symbols.txt")
    counts = {}
    for name, _, _ in dol:
        counts[name] = counts.get(name, 0) + 1
    symbols = {name: (0, 0, addr) for name, _, addr in dol if counts[name] == 1}

    rel_counts, rel_entries = {}, []
    for module, module_id in rel_module_ids(config_dir, version, rel_ids_path).items():
        for name, section, offset in read_decomp_symbols(f"{config_dir}/{version}/rels/{module}/symbols.txt"):
            rel_counts[name] = rel_counts.get(name, 0) + 1
            if section in REL_SECTIONS:
                rel_entries.append((name, module_id, REL_SECTIONS[section], offset))
    for name, module_id, section, offset in rel_entries:
        if rel_counts[name] == 1 and name not in counts:
            symbols[name] = (module_id, section, offset)
    return symbols


def format_entry(name, entry):
    module_id, section, offset = entry
    if module_id == 0:
        return f"{offset:08X}:{name}\n"
    return f"0x{module_id:x},{section:d},{offset:x}:{name}\n"


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("output")
    parser.add_argument("config_dir")
    parser.add_argument("version")
    parser.add_argument("--rel-ids")
    args = parser.parse_args(argv[1:])

    symbols = read_decomp(args.config_dir, args.version, args.rel_ids)
    with open(args.output, "w") as f:
        for name, entry in symbols.items():
            f.write(format_entry(name, entry))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
