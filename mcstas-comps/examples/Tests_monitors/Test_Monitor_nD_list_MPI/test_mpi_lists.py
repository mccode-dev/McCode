#!/usr/bin/env python3
"""MPI regression test for McCode event-list output (Monitor_nD "list all").

Runs Test_Monitor_nD_list_MPI.instr with ``mcrun --mpi=NP`` for every mode
(see the instrument description) in McCode (ASCII) and NeXus format, with a
timeout, and checks that

  * the simulation terminates (list output used to hang under MPI when a
    node had no events, or when nodes flushed their buffers unequally),
  * the list holds exactly the expected number of events,
  * no MPI spool files (mcspool_*.tmp) are left in the output directory.

Usage (from any directory):
  python3 test_mpi_lists.py [--np 2] [--formats auto] [--mcrun mcrun]

NeXus is checked with h5py; with --formats auto it is skipped (and reported)
when h5py is missing or the NeXus build of the instrument fails.
Exit status is 0 when all checks pass.
"""
import argparse
import math
import os
import shutil
import signal
import subprocess
import sys
import tempfile
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
INSTR = HERE / "Test_Monitor_nD_list_MPI.instr"
LISTFILE = "events_list.p.x"   # Monitor_nD file name for filename="events", options "list all x"
BUFSIZ = 1000000               # Monitor_nD flushes 'list all' buffers above 1e6 events


def expected_events(mode, np_, per):
    """Number of listed events for a run with np_ nodes tracing 'per' rays each."""
    return {
        0: per,
        1: per,
        2: np_ * per,
        3: 0,
        4: per + (per + 1) // 2 + max(np_ - 2, 0),
        5: np_,
    }[mode]


def run(cmd, cwd, timeout):
    """Run cmd in its own process group; kill the whole group on timeout."""
    kwargs = dict(cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    if os.name == "posix":
        kwargs["start_new_session"] = True
    t0 = time.time()
    proc = subprocess.Popen(cmd, **kwargs)
    try:
        out, _ = proc.communicate(timeout=timeout)
        return proc.returncode, out, time.time() - t0, False
    except subprocess.TimeoutExpired:
        if os.name == "posix":
            os.killpg(proc.pid, signal.SIGKILL)
        else:
            proc.kill()
        out, _ = proc.communicate()
        return None, out, time.time() - t0, True


def count_ascii(outdir):
    path = outdir / LISTFILE
    if not path.exists():
        return 0
    with open(path) as f:
        return sum(1 for line in f if line.strip() and not line.startswith("#"))


def count_nexus(outdir):
    import h5py
    path = outdir / "mccode.h5"
    if not path.exists():
        return None
    rows = 0
    with h5py.File(path, "r") as f:
        comps = f["entry1/instrument/components"]
        for comp in comps.values():
            output = comp.get("output")
            if output is None:
                continue
            for name, grp in output.items():
                if name.startswith("events_list") and "events" in grp:
                    rows += grp["events"].shape[0]
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--np", type=int, default=2, help="number of MPI processes (>= 2, default 2)")
    parser.add_argument("--mcrun", default="mcrun", help="mcrun executable (default: mcrun)")
    parser.add_argument("--formats", default="auto",
                        help="comma-separated list of McCode,NeXus or 'auto' (McCode + NeXus if available)")
    parser.add_argument("--ncount", type=float, default=1e4, help="rays for modes 0-3,5 (default 1e4)")
    parser.add_argument("--flush-ncount", type=float, default=None,
                        help="rays for mode 4 (default 2.2e6 per node, i.e. unequal TRACE-time flushes); 0 skips mode 4")
    parser.add_argument("--timeout", type=float, default=600, help="seconds per simulation (default 600)")
    parser.add_argument("--workdir", default=None,
                        help="work directory (default: a temporary one); simulation output is removed on success")
    parser.add_argument("--keep", action="store_true", help="keep simulation output also on success")
    args = parser.parse_args()

    if args.np < 2:
        parser.error("--np must be at least 2")
    mcrun = shutil.which(args.mcrun) or args.mcrun
    flush_ncount = args.flush_ncount if args.flush_ncount is not None else 2.2e6 * args.np

    workdir = Path(args.workdir) if args.workdir else Path(tempfile.mkdtemp(prefix="mpi_lists_"))
    workdir.mkdir(parents=True, exist_ok=True)
    shutil.copy(INSTR, workdir / INSTR.name)

    if args.formats == "auto":
        formats = ["McCode", "NeXus"]
        auto = True
    else:
        formats = [f.strip() for f in args.formats.split(",") if f.strip()]
        auto = False

    if "NeXus" in formats:
        try:
            import h5py  # noqa: F401
        except ImportError:
            if not auto:
                print("FAIL: h5py is needed to check NeXus output")
                return 1
            print("SKIP: NeXus (h5py not available)")
            formats.remove("NeXus")

    cases = [(m, args.ncount) for m in (0, 1, 2, 3, 5)]
    if flush_ncount > 0:
        cases.append((4, flush_ncount))

    failures = []
    for fmt in formats:
        compiled = False
        for mode, ncount in cases:
            outdir = workdir / f"{fmt}_mode{mode}"
            cmd = [mcrun, INSTR.name, f"--mpi={args.np}", f"--ncount={int(ncount)}", f"--dir={outdir.name}",
                   f"--format={fmt}", f"--bufsiz={BUFSIZ}", f"mode={mode}"]
            if not compiled:
                cmd.insert(2, "-c")
            rc, out, dt, timed_out = run(cmd, workdir, args.timeout)
            label = f"{fmt:6s} mode={mode} np={args.np} ncount={ncount:g}"
            if not compiled and rc not in (0, None) and auto and fmt == "NeXus" and not outdir.exists():
                print(f"SKIP: NeXus (instrument does not build/run with --format=NeXus)\n{out[-2000:]}")
                break
            compiled = True
            if timed_out:
                failures.append(f"{label}: TIMEOUT after {dt:.0f} s (MPI hang?)")
                print(f"FAIL {label}: timeout after {dt:.0f} s\n{out[-3000:]}")
                continue
            if rc != 0:
                failures.append(f"{label}: mcrun exit status {rc}")
                print(f"FAIL {label}: exit status {rc}\n{out[-3000:]}")
                continue
            per = math.floor(ncount / args.np)
            expected = expected_events(mode, args.np, per)
            got = count_ascii(outdir) if fmt == "McCode" else count_nexus(outdir)
            if fmt == "NeXus" and got is None:
                failures.append(f"{label}: no mccode.h5 written")
                print(f"FAIL {label}: no mccode.h5 written")
                continue
            leftovers = sorted(p.name for p in outdir.glob("mcspool_*"))
            status = "ok"
            if got != expected:
                status = f"expected {expected} events, got {got}"
            if leftovers:
                status = (status if status != "ok" else "") + f" left-over spool files {leftovers}"
            if status != "ok":
                failures.append(f"{label}: {status}")
                print(f"FAIL {label}: {status}")
            else:
                print(f"ok   {label}: {got} events in {dt:.1f} s")

    if failures:
        print(f"\n{len(failures)} failure(s), output kept in {workdir}:")
        for f in failures:
            print("  " + f)
        return 1
    if not formats:
        print("FAIL: no format could be tested")
        return 1
    if not args.keep:
        if args.workdir:
            for fmt in formats:
                for mode, _ in cases:
                    shutil.rmtree(workdir / f"{fmt}_mode{mode}", ignore_errors=True)
        else:
            shutil.rmtree(workdir, ignore_errors=True)
    print("\nAll MPI list-output checks passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
