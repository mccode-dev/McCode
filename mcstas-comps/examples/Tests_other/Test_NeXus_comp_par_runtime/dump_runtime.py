#!/usr/bin/env python3
"""
Dump and optionally check the post-INITIALIZE component parameter values
('runtime_value' / 'runtime_value_status' attributes) in a McCode NeXus file.

Usage:
  dump_runtime.py mccode.h5                      # print table
  dump_runtime.py mccode.h5 --quiet --expect source.dist=1.5 \
      "bender.rTopPar={0.99, 0.219, 6.07, 0, 0.003}" conics.radii=status:unsupported:pointer

  --expect comp.par=VALUE         runtime_value must equal VALUE (numbers and
                                  {..} vectors compared with relative tolerance)
                                  and status must be 'ok'
  --expect comp.par=status:STATUS only check runtime_value_status

Exit value: 0 all checks passed, 1 a check failed, 2 file could not be checked.
Any 'layout-mismatch' status is always a failure.
"""
import argparse
import math
import sys

try:
    import h5py
except ImportError:
    print("dump_runtime.py: python module h5py is required", file=sys.stderr)
    sys.exit(2)


def text(v):
    if isinstance(v, bytes):
        return v.decode(errors="replace")
    return "" if v is None else str(v)


def numbers(s):
    """float list from '1.5' or '{1, 2, 3}', None if not numeric"""
    s = s.strip()
    if s.startswith("{") and s.endswith("}"):
        s = s[1:-1]
    try:
        return [float(x) for x in s.split(",")] if s else []
    except ValueError:
        return None


def same(found, expected):
    a, b = numbers(found), numbers(expected)
    if a is not None and b is not None:
        return len(a) == len(b) and all(
            math.isclose(x, y, rel_tol=1e-12, abs_tol=1e-15) for x, y in zip(a, b))
    return found == expected


def load(filename):
    f = h5py.File(filename, "r")
    entries = sorted((k for k in f if k.startswith("entry")),
                     key=lambda k: int(k[5:] or 0))
    if not entries:
        raise KeyError("no NXentry found")
    comps = f[entries[-1] + "/instrument/components"]
    pars = {}
    for nxname in comps:
        group = comps[nxname].get("parameters")
        if group is None:
            continue
        name = nxname[5:] if len(nxname) > 5 and nxname[:4].isdigit() and nxname[4] == "_" else nxname
        for par in group:
            a = group[par].attrs
            pars[(name, par)] = (nxname, text(a.get("value")), text(a.get("runtime_value")),
                                 text(a.get("runtime_value_status")))
    return pars


def main():
    p = argparse.ArgumentParser(description=__doc__.split("\n\n")[1])
    p.add_argument("h5file")
    p.add_argument("--expect", nargs="*", default=[], metavar="COMP.PAR=VALUE")
    p.add_argument("--quiet", action="store_true", help="do not print the table")
    args = p.parse_args()

    try:
        pars = load(args.h5file)
    except Exception as e:  # missing file, not NeXus, ...
        print("dump_runtime.py: cannot read %s: %s" % (args.h5file, e), file=sys.stderr)
        return 2
    if not pars:
        print("dump_runtime.py: no component parameters in %s" % args.h5file, file=sys.stderr)
        return 2

    if not args.quiet:
        for (_, par), (nxname, lit, rv, st) in sorted(pars.items(), key=lambda i: (i[1][0], i[0][1])):
            print("%-22s %-14s %-22.22s -> %-24.24s [%s]" % (nxname, par, lit, rv, st))

    failures = []
    for (name, par), (_, lit, rv, st) in sorted(pars.items()):
        if not st:
            failures.append("%s.%s: no runtime_value_status" % (name, par))
        elif st == "layout-mismatch":
            failures.append("%s.%s: layout-mismatch" % (name, par))

    for spec in args.expect:
        key, sep, expected = spec.partition("=")
        name, dot, par = key.partition(".")
        if not sep or not dot:
            failures.append("bad --expect '%s' (use comp.par=value)" % spec)
            continue
        if (name, par) not in pars:
            failures.append("%s.%s: parameter not found" % (name, par))
            continue
        _, lit, rv, st = pars[(name, par)]
        if expected.startswith("status:"):
            if st != expected[7:]:
                failures.append("%s.%s: status '%s', expected '%s'" % (name, par, st, expected[7:]))
        elif st != "ok" or not same(rv, expected):
            failures.append("%s.%s: runtime_value '%s' [%s], expected '%s' (literal '%s')"
                            % (name, par, rv, st, expected, lit))

    for f in failures:
        print("FAIL " + f)
    print("dump_runtime.py: %d parameters, %d expectations, %d failures"
          % (len(pars), len(args.expect), len(failures)))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
