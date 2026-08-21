# McStas / McXtrace User Tools Cheat Sheet

Source: `McCode/tools/Python` (plus `McCode/tools/matlab` for the legacy Matlab/Octave/iFit variants)

**Naming convention:** almost everything named `mc<something>` on the McStas
side has a direct `mx<something>` counterpart on the McXtrace side, built
from the *same* source, with the neutron/photon ("particle") wording
and a few McStas-only options (e.g. `--gravity`, time-of-flight mode)
adjusted accordingly. Where a tool has several rendering **variants**
(html / matplotlib / pyqtgraph / webgl / ...), the variant is appended after
a dash, e.g. `mcplot-html` / `mxplot-html`.

## Tool-class pages

| Page | Covers |
|---|---|
| [Code generators](codegen.md) | `mcstas`/`mcxtrace`, `mcstas-pygen`/`mcxtrace-pygen`, `mcstas-jupylab`/`mcxtrace-jupylab` |
| [mcgui](mcgui.md) | `mcgui` / `mxgui` |
| [mcrun](mcrun.md) | `mcrun` / `mxrun` |
| [mcplot family](mcplot.md) | `mcplot` / `mxplot`, `mcplotdiff` / `mxplotdiff`, `mccoplot` / `mxcoplot` — html / matplotlib / pyqtgraph / matlab variants |
| [mcdisplay family](mcdisplay.md) | `mcdisplay` / `mxdisplay` — pyqtgraph (default) / matplotlib / webgl / webgl-classic / cad / matlab variants |
| [mctest family](mctest.md) | `mctest` / `mxtest`, `mcviewtest` / `mxviewtest` |
| [mcdoc](mcdoc.md) | `mcdoc` / `mxdoc` |

## Quick reference: which tool for which job?

| I want to... | Use |
|---|---|
| Compile an `.instr` file to C directly (rarely needed by hand) | [`mcstas` / `mcxtrace`](codegen.md#mcstas-mcxtrace) |
| Convert an `.instr` model to Python for use with McStasScript | [`mcstas-pygen` / `mcxtrace-pygen`](codegen.md#mcstas-pygen-mcxtrace-pygen-convert-an-instrument-to-python) |
| Explore an instrument interactively in Jupyter via McStasScript | [`mcstas-jupylab` / `mcxtrace-jupylab`](codegen.md#mcstas-jupylab-mcxtrace-jupylab-explore-an-instrument-in-jupyter) |
| Edit/compile/run instruments interactively | [`mcgui` / `mxgui`](mcgui.md) |
| Run a simulation from the command line, or a parameter scan/optimisation | [`mcrun` / `mxrun`](mcrun.md) |
| Plot one simulation's results | [`mcplot-{html,matplotlib,pyqtgraph}` <br> `mxplot-{html,matplotlib,pyqtgraph}`](mcplot.md#mcplot-mxplot) |
| Compare exactly two simulations (`a - b`) | [`mcplotdiff-{html,matplotlib,pyqtgraph}` <br> `mxplotdiff-{html,matplotlib,pyqtgraph}`](mcplot.md#mcplotdiff-mxplotdiff) |
| Overlay 2+ simulations for direct comparison | [`mccoplot-{html,matplotlib,pyqtgraph}` <br> `mxcoplot-{html,matplotlib,pyqtgraph}`](mcplot.md#mccoplot-mxcoplot) |
| Visualise instrument geometry + particle trajectories | [`mcdisplay-{pyqtgraph,matplotlib,webgl,webgl-classic}` <br> `mxdisplay-{pyqtgraph,matplotlib,webgl,webgl-classic}`](mcdisplay.md) |
| Export instrument geometry to a CAD file | [`mcdisplay-cad` <br> `mxdisplay-cad`](mcdisplay.md#mcdisplay-cad-mxdisplay-cad-bonus-export-instrument-geometry-to-cad) |
| Plot/display results in Matlab, Octave, or iFit instead | [`mcplot-matlab` / `mxplot-matlab`](mcplot.md#mcplot-matlab-mxplot-matlab-legacy-matlaboctaveifit-variant) <br> [`mcdisplay-matlab` / `mxdisplay-matlab`](mcdisplay.md#mcdisplay-matlab-mxdisplay-matlab-legacy-matlaboctaveifit-variant) |
| Test/benchmark the whole instrument library against a McCode installation | [`mctest` / `mxtest`](mctest.md#mctest-mxtest) |
| Build a browsable pass/fail + diff report from `mctest` results | [`mcviewtest` / `mxviewtest`](mctest.md#mcviewtest-mxviewtest) |
| Generate/browse instrument & component documentation | [`mcdoc` / `mxdoc`](mcdoc.md) |
