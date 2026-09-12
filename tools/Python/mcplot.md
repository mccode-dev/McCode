[&larr; back to overview](README.md)

# mcplot / mxplot family

Three related tool groups live under `tools/Python/mcplot`, sharing a
loader/matching layer:

- **mcplot / mxplot** — plot one simulation's monitors
- **mcplotdiff / mxplotdiff** — plot the *difference* `a - b` between two simulations (2 datasets only)
- **mccoplot / mxcoplot** — *overlay* 2 or more simulations' monitors on the same axes

Each has three rendering variants: **html**, **matplotlib**, **pyqtgraph**. Plain `mcplot` additionally has a fourth, legacy **matlab** variant (see bottom of this page).

## mcplot / mxplot

### mcplot-html / mxplot-html

| Option | Description |
|---|---|
| `simulation` | file or directory to plot |
| `-n`, `--nobrowse` | do not open a web browser |
| `-l`, `--log` | enable log scale |
| `--autosize` | expand plot to window size on load |
| `--libpath PATH` | path to js library files (default: copy locally) |
| `-o`, `--output FILE` | output `.html` file |
| `-W`, `--width N` | iframe width |
| `-H`, `--height N` | iframe height |

### mcplot-matplotlib / mxplot-matplotlib

| Option | Description |
|---|---|
| `simulation` | file or directory to plot |
| `-t`, `--test` | data-loader test run (print plot graph structure) |
| `--html` | save to html via mpld3 (Linux only) |
| `--format FMT` | save to pdf/png/eps/svg... without opening a window |
| `--output FILE` | output file (extension may set format) |
| `--log` | start with log scale |
| `--backend NAME` | matplotlib backend to use |

### mcplot-pyqtgraph / mxplot-pyqtgraph

| Option | Description |
|---|---|
| `simulation` | file or directory to plot |
| `-t`, `--test` | data-loader test run |
| `--invcanvas` | invert canvas background (black &rarr; white) |

## mcplotdiff / mxplotdiff

Compares exactly two datasets: `diff = a - b`.

### mcplotdiff-html / mxplotdiff-html

| Option | Description |
|---|---|
| `a` | first simulation (minuend) |
| `b` | second simulation (subtrahend) |
| `-A`, `--label-a NAME` | short label for `a` in titles/filenames |
| `-B`, `--label-b NAME` | short label for `b` in titles/filenames |
| `-n`, `--nobrowse` | do not open a web browser |
| `-l`, `--log` | also produce log-scale plot (single-file mode) |
| `-D`, `--no-dat` | do not write McCode-format `.dat`/`mccode.sim` alongside the html |
| `-o`, `--output DIR` | output directory |
| `--autosize` | expand plot to window size on load |
| `--libpath PATH` | path to js library files |
| `-W`, `--width N` / `-H`, `--height N` | iframe dimensions |

### mcplotdiff-matplotlib / mxplotdiff-matplotlib

| Option | Description |
|---|---|
| `a` / `b` | the two simulations to diff (`a - b`) |
| `-A`, `--label-a NAME` / `-B`, `--label-b NAME` | short labels for titles/filenames |
| `-t`, `--test` | plot graph structure test/print |
| `--html` | save to html via mpld3 (Linux only) |
| `--format FMT` / `--output FILE` | save to file without opening a window |
| `--log` | start with log scale |
| `--backend NAME` | matplotlib backend to use |

### mcplotdiff-pyqtgraph / mxplotdiff-pyqtgraph

| Option | Description |
|---|---|
| `a` / `b` | the two simulations to diff (`a - b`) |
| `-A`, `--label-a NAME` / `-B`, `--label-b NAME` | short labels for titles |
| `-t`, `--test` | data-loader test run |
| `--invcanvas` | invert canvas background |

*(2D diff monitors use a diverging blue/white/red colour map; press `c` to cycle colour maps as usual.)*

## mccoplot / mxcoplot

Overlays **2 or more** datasets (unlike mcplotdiff, not limited to two).

### mccoplot-html / mxcoplot-html

| Option | Description |
|---|---|
| `datasets` | 2+ simulation files/directories to overlay |
| `-L`, `--labels A,B,C` | comma-separated short labels, one per dataset (default: derived from each path) |
| `-C`, `--colours c1,c2,...` | comma-separated overlay colours, one per dataset |
| `-n`, `--nobrowse` | do not open a web browser |
| `-l`, `--log` | also produce log-scale plot (single-file mode) |
| `-o`, `--output DIR` | output directory |
| `--autosize` | expand plot to window size on load |
| `--libpath PATH` | path to js library files |
| `-W`, `--width N` / `-H`, `--height N` | iframe dimensions |

### mccoplot-matplotlib / mxcoplot-matplotlib

| Option | Description |
|---|---|
| `datasets` | 2+ simulation files/directories to overlay |
| `-L`, `--labels A,B,C` | comma-separated short labels per dataset |
| `-C`, `--colours c1,c2,...` | comma-separated overlay colours per dataset |
| `-t`, `--test` | print matched monitor groups before plotting |
| `--html` | save to html via mpld3 (Linux only) |
| `--format FMT` / `--output FILE` | save to file without opening a window |
| `--log` | start with log scale |
| `--backend NAME` | matplotlib backend to use |

### mccoplot-pyqtgraph / mxcoplot-pyqtgraph

| Option | Description |
|---|---|
| `datasets` | 2+ simulation files/directories to overlay |
| `-L`, `--labels A,B,C` | comma-separated short labels per dataset |
| `-C`, `--colours c1,c2,...` | comma-separated overlay colours per dataset |
| `-t`, `--test` | print matched monitor groups before plotting |
| `--invcanvas` | invert canvas background |

*(Legends always show compact letters `A`, `B`, `C`, ...; the full dataset identity is shown separately in the title/header.)*

## mcplot-matlab / mxplot-matlab *(legacy Matlab/Octave/iFit variant)*

A second, older implementation of plain `mcplot` (single-simulation plotting only — no `mcplotdiff`/`mccoplot` equivalent exists for this variant), living under `tools/matlab` as a bash wrapper around a `.m` script rather than Python. Runs under **Matlab**, **Octave**, or **iFit** (a Matlab-compiler-based standalone runtime, see <https://ifit.mccode.org>) — the wrapper auto-detects whichever is available, preferring Matlab, then Octave, then iFit, and falls back to the plain Python `mcplot` if none of the three are found.

| Option | Description |
|---|---|
| `FILE`\|`DIR` | monitor file or simulation directory to plot (default: current directory) |
| `-m` | explicitly request Matlab (skip auto-detection) |
| `-o` | explicitly request Octave |
| `-i` | explicitly request iFit |
| `-h` | show wrapper usage and exit |
| `-png`, `-jpg`, `-fig`, `-eps`, `-pdf` | *(parsed by `mcplot.m` itself, not the wrapper)* export each plotted monitor to the given format instead of just displaying it |

*(`-m`/`-o`/`-i`/`-h` are wrapper-level flags and must come first; the export-format flags are forwarded as-is to the underlying `.m` script.)*

---
[&larr; back to overview](README.md)
