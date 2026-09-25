#!/usr/bin/env python3
import argparse
import fnmatch
import re

DATA_SECTIONS = (".data", ".bss", ".sdata", ".sbss")

EXCLUDED_UNITS = (
    "dolphin/*",
    "revolution/*",
    "TRK_MINNOW_DOLPHIN/*",
    "PowerPC_EABI_Support/*",
    "NdevExi2A/*",
    "m_Re/*",
    "Z2AudioCS/*",
    "JSystem/JUtility/*",
    "JSystem/JFramework/*",
    "JSystem/JMath/*",
    "JSystem/JHostIO/*",
    "m_Do/m_Do_main.cpp",
    "m_Do/m_Do_printf.cpp",
    "m_Do/m_Do_controller_pad.cpp",
    "m_Do/m_Do_machine*.cpp",
    "m_Do/m_Do_Reset.cpp",
    "m_Do/m_Do_dvd_thread.cpp",
    "m_Do/m_Do_DVDError.cpp",
    "m_Do/m_Do_MemCard*.cpp",
    "m_Do/m_Do_hostIO.cpp",
    "d/d_home_button.cpp",
    "d/d_cursor_mng.cpp",
    "d/d_error_msg.cpp",
    "JSystem/JKernel/JKR*Heap.cpp",
    "JSystem/JKernel/JKRThread.cpp",
    "JSystem/JKernel/JKRAram.cpp",
    "JSystem/JKernel/JKRAramBlock.cpp",
    "JSystem/JKernel/JKRAramPiece.cpp",
    "JSystem/JKernel/JKRAramStream.cpp",
    "JSystem/JKernel/JKR*Ripper.cpp",
    "JSystem/JKernel/JKRDecomp.cpp",
    "JSystem/JAudio2/JAS*Thread.cpp",
    "JSystem/JAudio2/JASProbe.cpp",
    "JSystem/JAudio2/JASReport.cpp",
    "JSystem/JAudio2/JASCmdStack.cpp",
    "JSystem/JAudio2/JASAramStream.cpp",
    "JSystem/JAudio2/JASAiCtrl.cpp",
    "JSystem/JAudio2/JASAudioReseter.cpp",
    "JSystem/JAudio2/JASCriticalSection.cpp",
    "JSystem/JAudio2/JASDSPInterface.cpp",
    "JSystem/JAudio2/JASDriverIF.cpp",
    "JSystem/JAudio2/*dsp*.cpp",
)

OS_MODULE_LIST = (0x800030C8, 0x800030D4)


def is_included(unit):
    return not any(fnmatch.fnmatchcase(unit, pattern) for pattern in EXCLUDED_UNITS)


def parse_splits(path):
    units = []
    current = None
    for line in open(path):
        if line and not line[0].isspace() and line.rstrip().endswith(":"):
            name = line.rstrip()[:-1]
            current = None if name == "Sections" else name
            if current:
                units.append((current, []))
            continue
        m = re.match(r"\s+(\S+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)", line)
        if m and current:
            units[-1][1].append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return units


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output")
    parser.add_argument("splits")
    args = parser.parse_args()

    ranges = [OS_MODULE_LIST]
    for unit, sections in parse_splits(args.splits):
        if not is_included(unit):
            continue
        for name, start, end in sections:
            if name in DATA_SECTIONS and end > start:
                ranges.append((start, end))

    ranges.sort()
    merged = []
    for start, end in ranges:
        if merged and start <= merged[-1][1]:
            merged[-1] = (merged[-1][0], max(merged[-1][1], end))
        else:
            merged.append((start, end))

    with open(args.output, "w") as out:
        out.write("#ifndef TPGZ_SAVESTATE_RANGES_H\n#define TPGZ_SAVESTATE_RANGES_H\n\n")
        out.write("static const SaveStateRange l_dolRanges[] = {\n")
        for start, end in merged:
            out.write(f"    {{0x{start:08X}, 0x{end:08X}}},\n")
        out.write("};\n\n#endif\n")


if __name__ == "__main__":
    main()
