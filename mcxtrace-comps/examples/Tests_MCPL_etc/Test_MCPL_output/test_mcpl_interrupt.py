#!/usr/bin/env python3
"""Regression test: MCPL_output file of an interrupted run (issues #3055, #3181).

Compiles Test_MCPL_output.instr (next to this script; the McStas and McXtrace
copies of this script are identical) or the --instr instrument, starts it
with far more rays than it can finish and interrupts it with SIGINT once
particles are being written.
The file must then hold stat:sum:initial_ray_count == -1 (unknown), not the
requested ray count, and its last particle must be readable. Repeated, as only
some of the signals arrive while a particle is written.

Usage: python3 test_mcpl_interrupt.py [--mcrun mcrun] [--workdir DIR] [--instr FILE]
Needs POSIX signals and mcpltool; exit status is 0 when the check passes.
"""
import argparse
import os
import re
import shutil
import signal
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
NCOUNT = 2e9  # requested; never reached
TIMEOUT = 300
MINSIZE = 1 << 20  # bytes of MCPL output before interrupting
NRUNS = 10


def fail(what, proc, log):
    print(f"FAIL: {what} (exit status {proc.poll()})")
    print(log.read_text(errors="replace")[-3000:])
    return 1


def interrupted_run(cwd, exe):
    shutil.rmtree(cwd / "out", ignore_errors=True)
    log = cwd / "run.log"
    with open(log, "w") as out:
        proc = subprocess.Popen([str(exe), "-d", "out", "-s", "1", f"Ncount={NCOUNT:g}"],
                                cwd=cwd, stdout=out, stderr=subprocess.STDOUT)
    try:
        t0 = time.time()
        # wait for TRACE: particle data on disk, more than a file header
        while not any(f.stat().st_size > MINSIZE for f in cwd.glob("out/*.mcpl")):
            if proc.poll() is not None or time.time() - t0 > TIMEOUT:
                return fail("no particles written before exit/timeout", proc, log)
            time.sleep(0.2)
        proc.send_signal(signal.SIGINT)
        proc.wait(TIMEOUT)
    finally:
        if proc.poll() is None:
            proc.kill()

    files = sorted(cwd.glob("out/*.mcpl.gz"))
    if len(files) != 1:
        return fail(f"expected one .mcpl.gz in {cwd / 'out'}, found {len(files)}: "
                    f"{sorted(p.name for p in cwd.glob('out/*'))}", proc, log)
    head = subprocess.run(["mcpltool", "-j", str(files[0])], stdout=subprocess.PIPE, text=True, check=True).stdout
    nparticles = int(re.search(r"No\. of particles\s*:\s*(\d+)", head).group(1))
    m = re.search(r"stat:sum:initial_ray_count:\s*(-?[\d.eE+]+)", head)
    rays = float(m.group(1)) if m else None
    # reads the last particle; fails if the file holds fewer than it claims
    last = subprocess.run(["mcpltool", "-n", "-l1", f"-s{nparticles - 1}", str(files[0])], stdout=subprocess.DEVNULL)
    ok = rays == -1 and nparticles > 0 and last.returncode == 0
    print(f"{'PASS' if ok else 'FAIL'}: interrupted run: {nparticles} particles, "
          f"initial_ray_count {rays}, last particle {'ok' if last.returncode == 0 else 'missing'}")
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--mcrun", default="mcrun")
    ap.add_argument("--workdir", default="run_mcpl_interrupt")
    ap.add_argument("--instr", type=Path, default=HERE / "Test_MCPL_output.instr")
    args = ap.parse_args()
    if os.name != "posix" or not shutil.which("mcpltool"):
        print("SKIP: MCPL interrupt check (needs POSIX and mcpltool)")
        return 0

    cwd = Path(args.workdir).resolve()
    shutil.rmtree(cwd, ignore_errors=True)
    cwd.mkdir(parents=True)
    shutil.copy(args.instr, cwd)
    subprocess.run([args.mcrun, "-c", "-n", "0", args.instr.name], cwd=cwd, check=True, timeout=900)

    exe = cwd / (args.instr.stem + ".out")
    return max(interrupted_run(cwd, exe) for _ in range(NRUNS))


if __name__ == "__main__":
    sys.exit(main())
