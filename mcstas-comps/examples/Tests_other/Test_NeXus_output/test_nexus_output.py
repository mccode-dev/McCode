#!/usr/bin/env python3
"""Regression test for the NeXus output of the McCode runtime.

Runs Test_NeXus_output.instr (next to this script; the McStas and McXtrace
copies of this script are identical) with --format=NeXus and checks
mccode.h5 with h5py:

  * string values containing '%' are stored as given: the instrument
    parameter 'tag' (attribute and data set in entry1/simulation/Param) and
    the literal of a component string parameter (parameters/filename@value)
  * every string attribute and string data set is printable text, i.e. no
    uninitialised memory is written (Monitor_nD BINS output)
  * no debug output ("Received detector from") on stdout
  * a monitor with a file name of 1000 characters neither crashes the
    simulation nor loses the other monitors of the serial run
  * a second run with --append adds entry2

The serial run, and with --np > 0 also an MPI run, are checked.

Usage: python3 test_nexus_output.py [--mcrun mcrun] [--np 2] [--workdir DIR]
Exit status is 0 when all checks pass.
"""
import argparse
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
INSTR = HERE / "Test_NeXus_output.instr"
TAG = "50%%done"          # stored as given ('%%' only: no conversion that reads a missing argument)
PSD_FILENAME = "f%%1"     # psd 'filename' literal in the instrument (quotes removed by the code generator)
PRINTABLE = set(range(32, 127)) | {9, 10, 13}

failures = []
serial_data = []  # entry1/data groups of the serial run


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        failures.append(what)


def text(v):
    if hasattr(v, "shape") and v.shape:  # one-element string data set
        v = v[0]
    return v.decode("latin-1") if isinstance(v, bytes) else str(v)


def mcrun(args, cwd, label):
    cmd = [args.mcrun, INSTR.name, "--format=NeXus", "-n", "10000", "-s", "1234"] + label
    print("+ " + " ".join(cmd), flush=True)
    p = subprocess.run(cmd, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       text=True, errors="replace", timeout=900)
    print(p.stdout[-3000:])
    return p.returncode, p.stdout


def open_h5(path, label):
    """mccode.h5 opened with h5py, or None (a failed check) when missing or unreadable."""
    import h5py
    try:
        return h5py.File(path, "r")
    except OSError as e:
        check(False, f"{label}: {path.name} readable ({e})")
        return None


def check_strings(f, label):
    """Every string attribute and string data set is printable."""
    import h5py
    bad = []

    def visit(name, obj):
        for k, v in obj.attrs.items():
            if isinstance(v, (bytes, str)) and set(text(v).encode("latin-1")) - PRINTABLE:
                bad.append(f"{obj.name}@{k}={text(v)[:40]!r}")
        if isinstance(obj, h5py.Dataset) and obj.dtype.kind == "S":
            if set(text(obj[()]).encode("latin-1")) - PRINTABLE:
                bad.append(f"{obj.name}={text(obj[()])[:40]!r}")
    visit("/", f)
    f.visititems(visit)
    check(not bad, f"{label}: all strings printable" + ("" if not bad else ": " + "; ".join(bad[:5])))


def check_run(args, cwd, outdir, label, mpi=None):
    extra = ["-d", outdir] + ([f"--mpi={mpi}"] if mpi else []) + [f"tag={TAG}"]
    rc, out = mcrun(args, cwd, extra)
    check(rc == 0, f"{label}: simulation exit status 0 (got {rc})")
    check("Received detector from" not in out, f"{label}: no debug output on stdout")
    f = open_h5(cwd / outdir / "mccode.h5", label)
    if f is None:
        return
    with f:
        param = f["entry1/simulation/Param"]
        check(text(param.attrs["tag"]) == TAG, f"{label}: Param@tag == {TAG!r} (got {text(param.attrs['tag'])!r})")
        check(text(param["tag"][()]) == TAG, f"{label}: Param/tag == {TAG!r} (got {text(param['tag'][()])!r})")
        comps = f["entry1/instrument/components"]
        psd = [g for n, g in comps.items() if n.endswith("_psd")][0]
        val = text(psd["parameters/filename"].attrs["value"])
        check(val == PSD_FILENAME, f"{label}: psd parameters/filename@value == {PSD_FILENAME!r} (got {val!r})")
        if "nexus_bins" in INSTR.read_text():  # McStas Monitor_nD only
            bins = [n for c in comps.values() if "output" in c for n in c["output"] if n == "BINS"]
            check(len(bins) >= 2, f"{label}: Monitor_nD BINS maps written (got {len(bins)})")
        if not serial_data:
            serial_data.extend(n for n in f["entry1/data"] if not n.startswith("lmon"))
        check_strings(f, label)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--mcrun", default="mcrun")
    ap.add_argument("--np", type=int, default=2, help="MPI processes, 0 to skip the MPI run")
    ap.add_argument("--workdir", default="run_nexus_output")
    args = ap.parse_args()
    import h5py  # noqa: F401 - required, fail early

    cwd = Path(args.workdir).resolve()
    shutil.rmtree(cwd, ignore_errors=True)
    cwd.mkdir(parents=True)
    shutil.copy(INSTR, cwd)

    check_run(args, cwd, "serial", "serial")
    if args.np > 0:
        check_run(args, cwd, "mpi", f"MPI np={args.np}", mpi=args.np)

    # very long monitor file name: 1000 characters (CHAR_BUF_LENGTH is 1024)
    longname = "L" * 1000
    rc, out = mcrun(args, cwd, ["-d", "long", f"longname={longname}"])
    check(rc == 0, f"long name: simulation exit status 0 (got {rc})")
    f = open_h5(cwd / "long" / "mccode.h5", "long name")
    if f is not None:
        with f:
            data = f["entry1/data"]
            for mon in serial_data:
                check(mon in data, f"long name: entry1/data/{mon} written")
            print("INFO long name: lmon stored:", any(n.startswith("LLLL") for n in data))

    # second run appended to the same file
    rc, out = mcrun(args, cwd, ["-d", "serial", "-a", f"tag={TAG}"])
    check(rc == 0, f"append: simulation exit status 0 (got {rc})")
    f = open_h5(cwd / "serial" / "mccode.h5", "append")
    if f is not None:
        with f:
            check("entry2/instrument/components" in f, "append: entry2 written to mccode.h5")

    print(f"\n{len(failures)} check(s) failed" if failures else "\nAll checks passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
