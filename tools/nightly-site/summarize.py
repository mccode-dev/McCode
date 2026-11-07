#!/usr/bin/env python3
'''
Add one night of mctest/mcviewtest results to the nightly-site history.

  summarize.py TODAY_DIR DATE RUN_URL HISTORY_DIR

TODAY_DIR holds one mctest label dir per platform (as merged by the testsuite
"compare" job). Writes HISTORY_DIR/DATE/summary.json, copies the comparison
table there, and rebuilds HISTORY_DIR/index.json from all summaries.
Run without arguments for a self-check.
'''
import glob
import json
import os
import shutil
import sys


def column_key(label):
    ''' mcstas-3.99.99_mpi_x_auto_classic_ubuntu-latest_openmpi_1e6_Linux_<uid>
        -> mpi_x_auto_classic_ubuntu-latest_openmpi_1e6_Linux, stable night to night '''
    return "_".join(label.split("_")[1:-1])


def summarize_column(results):
    s = {"tests": 0, "no_test": 0, "compile_err": 0, "run_err": 0, "value_off": 0, "failed": []}
    for name, t in results.items():
        if name == "_meta":
            continue
        s["tests"] += 1
        # mctest's own verdict: compile errors, then its "failed" flag (runtime error or value off)
        if not t.get("compiled"):
            s["compile_err"] += 1
        elif t.get("failed"):
            s["run_err" if not t.get("didrun") else "value_off"] += 1
        else:
            if not t.get("testnb"):
                s["no_test"] += 1  # compiled, no %Test to run
            continue
        s["failed"].append(name)
    s["failed"].sort()
    s["walltime"] = results.get("_meta", {}).get("walltime")
    return s


def main(today, date, run_url, history):
    columns = {}
    for f in sorted(glob.glob(os.path.join(today, "*", "testresults_*.json"))):
        label = os.path.basename(os.path.dirname(f))
        with open(f) as fh:
            columns[column_key(label)] = dict(summarize_column(json.load(fh)), label=label)
    day = os.path.join(history, date)
    os.makedirs(day, exist_ok=True)
    with open(os.path.join(day, "summary.json"), "w") as fh:
        json.dump({"date": date, "run": run_url, "columns": columns}, fh, indent=1)
    # ponytail: the table's links into the platform dirs only resolve for today
    if os.path.isfile(os.path.join(today, "table.html")):
        shutil.copyfile(os.path.join(today, "table.html"), os.path.join(day, "table.html"))
    index = []
    for f in sorted(glob.glob(os.path.join(history, "*", "summary.json"))):
        with open(f) as fh:
            index.append(json.load(fh))
    with open(os.path.join(history, "index.json"), "w") as fh:
        json.dump(index, fh, separators=(",", ":"))
    return index


def selftest():
    import tempfile
    tmp = tempfile.mkdtemp()
    today, history = os.path.join(tmp, "today"), os.path.join(tmp, "history")
    label = "mcstas-3.99.99_mpi_x_auto_classic_ubuntu-latest_openmpi_1e6_Linux_abc123"
    os.makedirs(os.path.join(today, label))
    res = {"A": {"compiled": True, "didrun": True, "failed": False, "testnb": 1},
           "B": {"compiled": False, "didrun": False, "failed": True},
           "C": {"compiled": True, "didrun": False, "failed": True},
           "D": {"compiled": True, "didrun": True, "failed": True},
           "E": {"compiled": True, "didrun": None, "failed": None, "testnb": 0},
           "F": {"compiled": True, "didrun": True, "failed": None, "testnb": 1},
           "_meta": {"walltime": "1:00:00"}}
    with open(os.path.join(today, label, "testresults_%s.json" % label), "w") as fh:
        json.dump(res, fh)
    open(os.path.join(today, "table.html"), "w").write("<html></html>")
    main(today, "2026-11-02", "u1", history)
    index = main(today, "2026-11-03", "u2", history)
    c = index[-1]["columns"]["mpi_x_auto_classic_ubuntu-latest_openmpi_1e6_Linux"]
    assert [d["date"] for d in index] == ["2026-11-02", "2026-11-03"]
    assert (c["tests"], c["no_test"], c["compile_err"], c["run_err"], c["value_off"]) == (6, 1, 1, 1, 1), c
    assert c["failed"] == ["B", "C", "D"] and c["label"] == label
    assert os.path.isfile(os.path.join(history, "2026-11-03", "table.html"))
    shutil.rmtree(tmp)
    print("selftest ok")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        selftest()
    elif len(sys.argv) == 5:
        main(*sys.argv[1:])
    else:
        sys.exit(__doc__)
