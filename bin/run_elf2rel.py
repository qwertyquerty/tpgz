#!/usr/bin/env python3
import os
import subprocess
import sys


def main(argv):
    result = subprocess.run(argv[1:], capture_output=True, text=True)
    sys.stdout.write(result.stdout)
    sys.stderr.write(result.stderr)
    unresolved = [line for line in (result.stdout + result.stderr).splitlines() if "Unresolved" in line]
    if result.returncode != 0 or unresolved:
        if "-o" in argv:
            output = argv[argv.index("-o") + 1]
            if os.path.exists(output):
                os.remove(output)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
