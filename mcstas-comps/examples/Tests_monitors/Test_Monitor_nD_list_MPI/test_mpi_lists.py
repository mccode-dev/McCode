#!/usr/bin/env python3
"""MPI regression test for McCode event-list output (Monitor_nD "list all").

Runs Test_Monitor_nD_list_MPI.instr with ``mcrun --mpi=NP`` for every mode
(see the instrument description) in McCode (ASCII) and NeXus format, with a
timeout, and checks that

  * the simulation terminates (list output used to hang under MPI when a
    node had no events, or when nodes flushed their buffers unequally),
  * the list holds exactly the expected number of events,
  * no MPI spool files (mcspool_*.tmp) are left in the output directory,
  * NeXus: the 'events' data set in mccode.h5 is a virtual data set over the
    per-node files mccode_node<N>.h5, which reads back (with h5py) as exactly
    their concatenation in rank order, no mccode_node*.h5 file holds events
    that mccode.h5 does not reference, and 'h5repack -l CONTI' (when found on
    PATH) turns it into a self-contained file with the same events,
  * NeXus scan (-N2): each step's events are kept in their own NXentry, none
    overwritten by a later step,
  * NeXus, POSIX with pkill/pgrep: a run whose ranks get SIGTERM twice (as
    when mpirun forwards a 'pkill -TERM') saves once per rank, with no
    duplicate events in mccode_node*.h5, warns that the lists are not linked
    from mccode.h5, and nexus_vds.py then links all of them.

Usage (from any directory):
  python3 test_mpi_lists.py [--np 2] [--formats auto] [--mcrun mcrun]

NeXus is checked with h5py; with --formats auto it is skipped (and reported)
when h5py is missing or the NeXus build of the instrument fails.
Exit status is 0 when all checks pass.
"""
import argparse
import math
import os
import re
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


def check_nexus(outdir, h5repack=None):
    """Return (number of listed events, list of problems) for NeXus output;
    (None, []) when there is no mccode.h5."""
    import h5py
    import numpy as np
    path = outdir / "mccode.h5"
    if not path.exists():
        return None, []
    rows = 0
    problems = []
    referenced = set()
    sources = {}  # (node file, data set) -> events data set in mccode.h5 linking it
    events = {}
    with h5py.File(path, "r") as f:
        for entry in (g for g in f.values() if g.attrs.get("NX_class") in (b"NXentry", "NXentry")):
            comps = entry["instrument/components"]
            for comp in comps.values():
                output = comp.get("output")
                if output is None:
                    continue
                for name, grp in output.items():
                    if not (name.startswith("events_list") and "events" in grp):
                        continue
                    ds = grp["events"]
                    data = ds[()]  # full read, through the virtual data set
                    rows += data.shape[0]
                    events[grp.name] = data
                    if not ds.is_virtual:
                        continue
                    parts, ranks = [], []
                    for src in ds.virtual_sources():
                        referenced.add(src.file_name)
                        other = sources.setdefault((src.file_name, src.dset_name), grp.name)
                        if other != grp.name:  # e.g. NeXus scan steps sharing (overwriting) a node data set
                            problems.append(f"{grp.name} and {other} both link {src.file_name}:{src.dset_name}")
                        m = re.fullmatch(r"mccode_node(\d+)\.h5", src.file_name)
                        if not m:
                            problems.append(f"{name}: unexpected VDS source file '{src.file_name}'")
                            continue
                        ranks.append(int(m.group(1)))
                        with h5py.File(outdir / src.file_name, "r") as sf:
                            parts.append(sf[src.dset_name][()])
                    if ranks != sorted(set(ranks)):
                        problems.append(f"{name}: VDS sources not in rank order {ranks}")
                    if parts and not np.array_equal(np.vstack(parts), data):
                        problems.append(f"{name}: VDS differs from the concatenated mccode_node*.h5 data")
    for node in sorted(outdir.glob("mccode_node*.h5")):
        if node.name in referenced:
            continue
        with h5py.File(node, "r") as nf:
            n = sum(d.shape[0] for d in nf.values() if isinstance(d, h5py.Dataset))
        if n:
            problems.append(f"{node.name}: {n} events not referenced by mccode.h5")
    if h5repack and events:
        flat = outdir / "flat.h5"
        r = subprocess.run([h5repack, "-l", "CONTI", str(path), str(flat)],
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        if r.returncode:
            problems.append(f"h5repack failed: {r.stdout[-500:]}")
        else:
            with h5py.File(flat, "r") as f:
                for name, data in events.items():
                    ds = f[name + "/events"]
                    if ds.is_virtual or not np.array_equal(ds[()], data):
                        problems.append(f"{name}: h5repack -l CONTI did not give the same, flat events")
            flat.unlink()
    return rows, problems


def check_sigterm(mcrun, workdir, np_, ncount, timeout):
    """Interrupt a NeXus MPI run with SIGTERM; return a list of problems."""
    import h5py
    import numpy as np
    out = "NeXus_sigterm"
    outdir = workdir / out
    shutil.rmtree(outdir, ignore_errors=True)
    # the ranks only: mpirun stays alive and keeps forwarding their output
    pattern = rf"^\./[{INSTR.stem[0]}]{INSTR.stem[1:]}\.out.*--dir={out}( |$)"
    with open(workdir / f"{out}.log", "w") as log:
        proc = subprocess.Popen([mcrun, INSTR.name, f"--mpi={np_}", f"--ncount={int(ncount)}", f"--dir={out}",
                                 "--format=NeXus", f"--bufsiz={BUFSIZ}", "mode=2"],
                                cwd=workdir, stdout=log, stderr=subprocess.STDOUT)
        t0 = time.time()
        while len(list(outdir.glob("mccode_node*.h5"))) < np_ and proc.poll() is None and time.time() - t0 < timeout:
            time.sleep(0.2)
        if proc.poll() is not None:
            return [f"run ended before it could be interrupted (exit status {proc.returncode})"]
        time.sleep(1)
        for _ in range(2):  # twice, as when mpirun forwards a SIGTERM the ranks also got
            subprocess.run(["pkill", "-TERM", "-f", pattern])
            time.sleep(0.05)
        try:
            proc.wait(timeout=timeout)
            # ranks may outlive mpirun/mcrun while saving: wait for them (and their file locks)
            t0 = time.time()
            while subprocess.run(["pgrep", "-f", pattern], stdout=subprocess.DEVNULL).returncode == 0:
                if time.time() - t0 > timeout:
                    raise subprocess.TimeoutExpired("ranks", timeout)
                time.sleep(0.2)
        except subprocess.TimeoutExpired:
            subprocess.run(["pkill", "-KILL", "-f", pattern])
            proc.kill()
            return [f"no exit within {timeout:.0f} s after SIGTERM (hang)"]
    text = (workdir / f"{out}.log").read_text(errors="replace")
    problems = []
    saves = text.count("Finishing simulation")
    if not 1 <= saves <= np_:
        problems.append(f"{saves} final saves for {np_} ranks")
    if "Warning: interrupted MPI run" not in text or "nexus_vds.py" not in text:
        problems.append("no warning naming mccode_node*.h5 and nexus_vds.py")
    rows = 0
    for node in sorted(outdir.glob("mccode_node*.h5")):
        with h5py.File(node, "r") as nf:
            for ds in nf.values():
                data = ds[()]
                rows += data.shape[0]
                dup = data.shape[0] - np.unique(data, axis=0).shape[0]
                if dup:
                    problems.append(f"{node.name}: {dup} duplicate events")
    helper = None
    r = subprocess.run([mcrun, "--showcfg=tooldir"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
    if r.returncode == 0 and r.stdout.strip():
        helper = Path(r.stdout.strip().splitlines()[-1]) / "Python" / "mccodelib" / "nexus_vds.py"
    if not helper or not helper.exists():
        return problems + [f"nexus_vds.py not found ({helper})"]
    r = subprocess.run([sys.executable, str(helper), str(outdir)],
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    if r.returncode:
        return problems + [f"nexus_vds.py failed: {r.stdout[-500:]}"]
    linked, link_problems = check_nexus(outdir)
    if linked != rows:
        problems.append(f"{linked} events linked in mccode.h5, {rows} in mccode_node*.h5")
    return problems + link_problems


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--np", type=int, default=2, help="number of MPI processes (>= 2, default 2)")
    parser.add_argument("--mcrun", default="mcrun", help="mcrun executable (default: mcrun)")
    parser.add_argument("--formats", default="auto",
                        help="comma-separated list of McCode,NeXus or 'auto' (McCode + NeXus if available)")
    parser.add_argument("--ncount", type=float, default=1e4, help="rays for modes 0-3,5 (default 1e4)")
    parser.add_argument("--flush-ncount", type=float, default=None,
                        help="rays for mode 4 (default 2.2e6 per node, i.e. unequal TRACE-time flushes); 0 skips mode 4")
    parser.add_argument("--sigterm-ncount", type=float, default=1e8,
                        help="rays for the NeXus SIGTERM check (POSIX, needs pkill; default 1e8); 0 skips it")
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

    h5repack = shutil.which("h5repack")
    if "NeXus" in formats and not h5repack:
        print("NOTE: h5repack not found, NeXus output is not flattened")

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
            problems = []
            if fmt == "McCode":
                got = count_ascii(outdir)
            else:
                got, problems = check_nexus(outdir, h5repack)
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
            if problems:
                status = (status if status != "ok" else "") + " " + "; ".join(problems)
            if status != "ok":
                failures.append(f"{label}: {status}")
                print(f"FAIL {label}: {status}")
            else:
                print(f"ok   {label}: {got} events in {dt:.1f} s")

    if "NeXus" in formats and args.sigterm_ncount > 0 and os.name == "posix" and shutil.which("pkill") \
            and shutil.which("pgrep"):
        label = f"NeXus  SIGTERM np={args.np}"
        problems = check_sigterm(mcrun, workdir, args.np, args.sigterm_ncount, args.timeout)
        if problems:
            failures.append(f"{label}: " + "; ".join(problems))
            print(f"FAIL {label}: " + "; ".join(problems))
        else:
            print(f"ok   {label}: one save per rank, no duplicate events, linked by nexus_vds.py")
    elif "NeXus" in formats and args.sigterm_ncount > 0:
        print("SKIP: NeXus SIGTERM check (needs POSIX, pkill and pgrep)")

    if "NeXus" in formats:  # two scan steps append NXentries to one mccode.h5, sharing mccode_node*.h5
        label = f"NeXus  scan N=2 mode=2 np={args.np}"
        outdir = workdir / "NeXus_scan"
        rc, out, dt, timed_out = run([mcrun, INSTR.name, f"--mpi={args.np}", f"--ncount={int(args.ncount)}",
                                      f"--dir={outdir.name}", "--format=NeXus", f"--bufsiz={BUFSIZ}", "-N2", "mode=2,2"],
                                     workdir, args.timeout)
        expected = 2 * expected_events(2, args.np, math.floor(args.ncount / args.np))
        got, problems = check_nexus(outdir) if rc == 0 and not timed_out else (None, [f"mcrun exit status {rc}"])
        if got != expected or problems:
            failures.append(f"{label}: expected {expected} events, got {got} " + "; ".join(problems))
            print(f"FAIL {label}: expected {expected} events, got {got} " + "; ".join(problems) + f"\n{out[-2000:]}")
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
            shutil.rmtree(workdir / "NeXus_sigterm", ignore_errors=True)
            shutil.rmtree(workdir / "NeXus_scan", ignore_errors=True)
        else:
            shutil.rmtree(workdir, ignore_errors=True)
    print("\nAll MPI list-output checks passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
