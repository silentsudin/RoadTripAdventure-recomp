#!/usr/bin/env python3
"""Translated-guest corpus size vs the 1542-function denominator."""
import pathlib
import re

gen = pathlib.Path(__file__).resolve().parents[1] / "src/native/guest/gen"
n = sum(1 for f in gen.glob("*.cpp")
        if re.search(r"^// guest:0x[0-9a-f]{8}", f.read_text()[:200], re.M))
print(f"{n} / 1542")
