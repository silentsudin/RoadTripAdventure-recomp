#!/usr/bin/env python3
"""measure.py <package> [seconds]: clock-weighted CPU and GPU use of an app on the device.
CPU: sum over threads of run time x the clock of the core it ran on (GHz-equivalent = billions of
cycles per second). GPU: busy fraction x clock (MHz-equivalent)."""
import subprocess, sys, collections
pkg = sys.argv[1]; secs = int(sys.argv[2]) if len(sys.argv) > 2 else 10
pid = subprocess.run(["adb", "shell", "pidof", pkg], capture_output=True, text=True).stdout.split()[0]
subprocess.run(["adb", "push", str(__import__("pathlib").Path(__file__).with_name("sample.sh")), "/data/local/tmp/sample.sh"], capture_output=True)
out = subprocess.run(["adb", "shell", f"sh /data/local/tmp/sample.sh {pid} {secs}"], capture_output=True, text=True).stdout
samples, cur = [], None
for line in out.splitlines():
    p = line.split()
    if not p: continue
    if p[0] == "T": cur = {"t": float(p[1]), "r": {}, "f": [], "g": None}; samples.append(cur)
    elif p[0] == "R":
        stat, sched = line[2:].split(" | ")
        tid = stat.split()[0]
        name = stat[stat.index("(") + 1:stat.rindex(")")].replace(" ", "_")
        fields = stat[stat.rindex(")") + 2:].split()
        cur["r"][tid] = (name, int(sched.split()[0]), int(fields[36]))
    elif p[0] == "F": cur["f"] = [int(x) for x in p[1:]]
    elif p[0] == "G" and len(p) >= 4: cur["g"] = (int(p[1].rstrip('%')), int(p[2]), int(p[3]))
cycles = collections.Counter(); runtime = collections.Counter(); names = {}
gpu_cyc = 0.0; gpu_busy = 0.0; gpu_tot = 0.0
for a, b in zip(samples, samples[1:]):
    for tid, (name, ns, cpu) in b["r"].items():
        if tid in a["r"]:
            d = ns - a["r"][tid][1]
            if d > 0:
                f = b["f"][cpu] if cpu < len(b["f"]) else 0  # kHz
                cycles[tid] += d * 1e-9 * f * 1e3; runtime[tid] += d; names[tid] = name
    if b["g"]:
        busy, clk = b["g"][0], b["g"][2]  # gpu_busy_percentage, Hz
        dt = b["t"] - a["t"]
        gpu_busy += busy * dt; gpu_tot += 100 * dt; gpu_cyc += busy / 100 * clk * dt
wall = samples[-1]["t"] - samples[0]["t"]
tot = sum(cycles.values()) / wall / 1e9
print(f"{pkg}: {wall:.1f} s  CPU {tot:.2f} GHz-equivalent ({sum(runtime.values())/wall/1e9*100:.0f}% of one core)")
for tid, c in cycles.most_common(8):
    print(f"   {c/wall/1e9:5.2f} GHz  {runtime[tid]/wall/1e9*100:5.1f}%  {names[tid]}")
if gpu_tot:
    print(f"  GPU {gpu_cyc/wall/1e6:.0f} MHz-equivalent (busy {gpu_busy/gpu_tot*100:.1f}% of sampled windows)")
