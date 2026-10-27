#!/usr/bin/env python3
"""Regression test for the mcplot data loader (mccodelib.mcplotloader).

Runs Test_mcplot_loader.instr (next to this script; the McStas and McXtrace
copies of this script are identical) as a 3-point scan of 'w' in ASCII and
in NeXus format, and as a single NeXus run at the first scan point, all with
the same seed. Checks that McCodeDataLoader reads the same data from both
formats:

  * scan curves (mccode.dat)
  * every 0D/1D/2D monitor of every scan step (step subfolders / NeXus entries)
  * a single run (ASCII step 0 / single-entry mccode.h5)
  * the monitor keys mcplotdiff/mccoplot match simulations by (load_monitors)

Usage: python3 test_mcplot_loader.py [--mcrun mcrun] [--workdir DIR]
mccodelib is imported from the Python path, else from mcrun --showcfg tooldir.
Exit status is 0 when all checks pass.
"""
import argparse
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
INSTR = HERE / "Test_mcplot_loader.instr"
FIELDS = ("title", "xlabel", "ylabel", "xlimits", "values", "statistics",
          "xvals", "yvals", "y_err_vals", "Nvals", "zvals", "counts")

failures = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        failures.append(what)


def mcrun(args, cwd, extra):
    cmd = [args.mcrun, "-c", INSTR.name, "-n", "10000", "-s", "1234"] + extra
    print("+ " + " ".join(cmd), flush=True)
    p = subprocess.run(cmd, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       text=True, errors="replace", timeout=900)
    check(p.returncode == 0, " ".join(extra) + ": simulation exit status 0" + ("" if p.returncode == 0 else "\n" + p.stdout[-3000:]))


def compare(a, n, label):
    """the data of a (ASCII) and n (NeXus) node lists, field by field"""
    import numpy as np
    check(len(a) == len(n), f"{label}: {len(a)} == {len(n)} data sets")
    for x, y in zip(a, n):
        what = f"{label}: {x.component or x.title}"
        if type(x) is not type(y) or x.component != y.component:
            check(False, f"{what}: {type(x).__name__} {x.component!r} vs {type(y).__name__} {y.component!r}")
            continue
        bad = [f for f in FIELDS if hasattr(x, f) and not (
            np.shape(getattr(x, f)) == np.shape(getattr(y, f)) and np.allclose(getattr(x, f), getattr(y, f), rtol=1e-5)
            if f.endswith(("vals", "counts")) else getattr(x, f) == getattr(y, f))]
        check(not bad, f"{what} ({type(x).__name__})" + (f": differs in {bad}" if bad else ""))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--mcrun", default="mcrun")
    ap.add_argument("--workdir", default="run_mcplot_loader")
    args = ap.parse_args()
    import h5py  # noqa: F401 - required, fail early
    try:
        import mccodelib  # noqa: F401
    except ImportError:
        tooldir = subprocess.run([args.mcrun, "--showcfg", "tooldir"], stdout=subprocess.PIPE, text=True).stdout
        sys.path.insert(0, str(Path(tooldir.strip().splitlines()[-1]) / "Python"))
    from mccodelib.mcplotloader import McCodeDataLoader

    def load(d):
        loader = McCodeDataLoader(str(cwd / d))
        loader.load()
        return loader.plot_graph

    cwd = Path(args.workdir).resolve()
    shutil.rmtree(cwd, ignore_errors=True)
    cwd.mkdir(parents=True)
    shutil.copy(INSTR, cwd)

    scan = ["-N", "3", "w=0.02,0.06"]
    mcrun(args, cwd, ["-d", "ascii_scan"] + scan)
    mcrun(args, cwd, ["-d", "nexus_scan", "--format=NeXus"] + scan)
    mcrun(args, cwd, ["-d", "nexus_single", "--format=NeXus", "w=0.02"])
    if failures:
        return 1

    a, n = load("ascii_scan"), load("nexus_scan")
    compare(a.getdata_lst(), n.getdata_lst(), "scan curves")
    check(len(a.secondaries) == len(n.secondaries) == a.getnumdata(),
          f"scan: one step list per curve ({a.getnumdata()}, {len(a.secondaries)}, {len(n.secondaries)})")
    for sa, sn in zip(a.secondaries, n.secondaries):
        for i, (x, y) in enumerate(zip(sa.getdata_lst(), sn.getdata_lst())):
            compare([x], [y], f"scan step {i}")
    compare(load("ascii_scan/0").getdata_lst(), load("nexus_single").getdata_lst(), "single run")
    compare(load("ascii_scan/0").getdata_lst(), load("nexus_single/mccode.h5").getdata_lst(), "single mccode.h5")

    # mcplot-html names its pages (and places index.html) by the monitor file paths
    names = [[Path(d.filepath).name for d in load(p).getdata_lst()] for p in ("ascii_scan/0", "nexus_single")]
    check(names[0] == names[1], f"single run: monitor file names {names[0]}" + ("" if names[0] == names[1] else f" vs {names[1]}"))

    # mcplotdiff/mccoplot pair monitors across simulations by these keys
    from mccodelib.mcplotdiffloader import load_monitors
    for x, y in (("ascii_scan/0", "nexus_single"), ("ascii_scan/0", "nexus_single/mccode.h5"), ("ascii_scan", "nexus_scan")):
        ka, kn = (sorted(load_monitors(str(cwd / p))[0]) for p in (x, y))
        check(ka == kn, f"load_monitors {x} vs {y}: monitor keys {ka}" + ("" if ka == kn else f" vs {kn}"))

    print(f"\n{len(failures)} check(s) failed" if failures else "\nAll checks passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
