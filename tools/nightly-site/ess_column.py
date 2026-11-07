#!/usr/bin/env python3
'''
Turn the output of one "mcstas-ess-instr-citool -a runci -o OUTDIR" into a
column of the ESS instrument nightly site.

  ess_column.py OUTDIR NAME OUTCOME LOG_URL DESTDIR

Moves the mctest label dir OUTDIR/tests/<label> to DESTDIR/ess_NAME_x, so that
summarize.py keys the column by NAME, and points the mcviewtest links at it.
When runci did not succeed (OUTCOME is not "success": setup, build, checks or
extra_pytests failed), adds a failing "runci" row pointing at LOG_URL.
Run without arguments for a self-check.
'''
import glob
import json
import os
import shutil
import sys

RUNCI_ROW = {"displayname": "runci", "sourcefile": "", "instrname": "runci", "testnb": 1,
             "scan": None, "ncount": None, "parvals": "", "detector": None,
             "targetval": None, "testval": None, "testerr": None, "targeterr": None,
             "nsigma": None, "statfactor": None, "linted": None, "compiletime": 0,
             "displayed": None, "displaytime": None, "didrun": False, "runtime": 0,
             "errmsg": "runci failed", "failed": True}


def main(outdir, name, outcome, log, dest):
    col = "ess_%s_x" % name
    d = os.path.join(dest, col)
    os.makedirs(dest, exist_ok=True)
    res = glob.glob(os.path.join(outdir, "tests", "*", "testresults_*.json"))
    if res:
        with open(res[0]) as fh:
            data = json.load(fh)
        shutil.move(os.path.dirname(res[0]), d)
        os.remove(os.path.join(d, os.path.basename(res[0])))
    else:
        data = {}
        os.makedirs(d)
    for t in data.values():
        if isinstance(t, dict) and t.get("localfile"):
            # mcviewtest takes the label dir of its links from localfile
            t["localfile"] = "/".join([col] + t["localfile"].replace("\\", "/").split("/")[-2:])
    if outcome != "success":
        os.makedirs(os.path.join(d, "runci"), exist_ok=True)
        with open(os.path.join(d, "runci", "compile_stdout.txt"), "w") as fh:
            fh.write("runci did not succeed (%s), see %s\n" % (outcome, log))
        data["runci"] = dict(RUNCI_ROW, localfile=col + "/runci/runci.instr", compiled=bool(res))
    with open(os.path.join(d, "testresults_%s.json" % col), "w") as fh:
        json.dump(data, fh, indent=1)


def selftest():
    import tempfile
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from summarize import column_key, summarize_column
    tmp = tempfile.mkdtemp()
    label = "mcstas-3.99.99_x_1e6_Linux_abc"
    os.makedirs(os.path.join(tmp, "out", "tests", label, "A_main"))
    ok = {"localfile": "/w/out/tests/%s/A_main/A_main.instr" % label, "compiled": True,
          "didrun": True, "failed": False, "testnb": 1}
    with open(os.path.join(tmp, "out", "tests", label, "testresults_%s.json" % label), "w") as fh:
        json.dump({"A_main_1": ok, "_meta": {"walltime": "0:01:00"}}, fh)
    dest = os.path.join(tmp, "ess-nightly")
    main(os.path.join(tmp, "out"), "BEER", "failure", "u", dest)  # mctest ran, then runci failed
    main(os.path.join(tmp, "none"), "LOKI", "failure", "u", dest)  # no mctest results at all
    cols = {}
    for f in glob.glob(os.path.join(dest, "*", "testresults_*.json")):
        c = os.path.basename(os.path.dirname(f))
        assert os.path.basename(f) == "testresults_%s.json" % c
        with open(f) as fh:
            cols[column_key(c)] = (json.load(fh), summarize_column(json.load(open(f))))
    beer, loki = cols["BEER"], cols["LOKI"]
    assert beer[0]["A_main_1"]["localfile"] == "ess_BEER_x/A_main/A_main.instr"
    assert (beer[1]["tests"], beer[1]["run_err"], beer[1]["failed"]) == (2, 1, ["runci"]), beer[1]
    assert (loki[1]["tests"], loki[1]["compile_err"]) == (1, 1), loki[1]
    assert os.path.isfile(os.path.join(dest, "ess_LOKI_x", "runci", "compile_stdout.txt"))
    shutil.rmtree(tmp)
    print("selftest ok")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        selftest()
    elif len(sys.argv) == 6:
        main(*sys.argv[1:])
    else:
        sys.exit(__doc__)
