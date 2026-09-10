[&larr; back to overview](README.md)

# mctest family

Installation testing / benchmarking tools, under `tools/Python/mctest`.

## mctest / mxtest

Runs every `%Example` test embedded in the instrument library, compiling,
displaying (single-particle), and running each one, then compares the
result against the target value recorded in the instrument header.

| Option | Description |
|---|---|
| `TESTVERSION` | *(optional)* specific McCode version to test |
| `--ncount N`, `-n N` | ncount sent to `mcrun` (default: `1e6`) |
| `--seed S`, `-s S` | seed sent to `mcrun` (default: `1000`; `0`/`NULL` randomises) |
| `--mpi N` | MPI node count sent to `mcrun` |
| `--openacc` | pass `--openacc` to `mcrun` |
| `--nexus` | compile/run with NeXus output format everywhere |
| `--lint` | just run the C-linter (no simulation run) |
| `--config CONFIG` | test only this specific config — label name or absolute path |
| `--instr PATTERN` | test only instruments matching this regex (comma-separated for multiple) |
| `--comp COMP` | test only instruments that use component `COMP` |
| `--mccoderoot DIR` | root search folder for McCode installations |
| `--testdir DIR` | write test results directly into `DIR` (default: cwd) |
| `--local DIR` | pick up instruments to test from `DIR` instead of the McCode installation |
| `--limit N` | test only the first `N` instruments per version |
| `--skipnontest` | skip compiling instruments that have no `%Example` test |
| `--suffix SUFFIX` | append `SUFFIX` to the test directory name |
| `--uid ID` | unique identifier for the suffix (default: timestamp) |
| `--compilemax S` | max seconds allowed per compilation (default: 600; x100 with `--lint`) |
| `--runmax S` | max seconds allowed per test run (default: 3600) |
| `--displaymax S` | max seconds allowed per test display run (default: 60) |
| `--permissive` | exit 0 even if some tests fail |
| `--strict` | Let instruments without %%Example line(s) instantly fail. |
| `--versions` | display local version info |
| `--verbose` | print a test/no-test status header before each test |

## mcviewtest / mxviewtest

Builds a single browsable HTML comparison report from one or more `mctest`
result sets (as written by `mctest --testdir`), diffing/co-plotting each
row against a chosen reference column via `mcplotdiff-html`/`mccoplot-html`.

| Option | Description |
|---|---|
| `testdir` | *(optional)* root folder containing `mctest` result subfolders (default: cwd) |
| `--reflabel LABEL` | reference column/label to compare against (default: oldest subfolder in cwd) |
| `--testroot DIR` | test-root folder for interactive result management/cleanup |
| `--verbose` | output excessive debug information |
| `--nobrowse` | do not open a browser on completion |
| `--nodiff` | do not generate diff/co-plot comparison cells |
| `--diff-errors-only` | only diff rows that show a discrepancy (default: diff every valid row) |
| `--diffmax S` | max seconds per diff/co-plot comparison (default: 300) |
| `--diffworkers N` | number of comparisons to run in parallel (default: number of CPUs) |

---
[&larr; back to overview](README.md)
