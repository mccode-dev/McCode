[&larr; back to overview](README.md)

# mcdisplay family

All variants wrap `mcrun --trace` under the hood; `INSTR` plus any
`name=value` instrument parameters are forwarded straight to it. The
**default** is `mcdisplay`/`mxdisplay` &rarr; pyqtgraph.

## mcdisplay / mxdisplay *(default = pyqtgraph)*

| Option | Description |
|---|---|
| `INSTR` | instrument file (`.instr` or compiled binary) |
| `name=value ...` | simulation parameters, passed through to `mcrun` |
| `--default` | automatically use instrument default parameter values |
| `--no-mpi` | forward `--no-mpi` to `mcrun` (simulate without MPI) |
| `-n`, `--ncount N` | number of particles to trace (default: 300) |
| `--dirname DIR` | override output directory name |
| `--inspect COMP` | show only rays reaching component `COMP` |
| `--invcanvas` | invert canvas background |
| `--tof`, `--TOF`, `--ToF` | *(McStas only)* enable time-of-flight display mode |

**Interactive keys:** `q` quit &middot; `p` save png &middot; `s` save svg (not on Windows) &middot; `space`/`F5` next ray &middot; click a subplot to zoom, right-click to exit zoom &middot; `h`/`F1` component list.

## mcdisplay-matplotlib / mxdisplay-matplotlib

Thin bash wrapper piping `mcrun --trace` output into a matplotlib 3D viewer — options are parsed by the wrapper script itself, not Python argparse.

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `-n N`, `--ncount N`, `--ncount=N` | number of particles to trace (capped at 1e2 by the wrapper) |
| `--trace={1,2}` | classic (1) or new (2, default) visualisation mode |
| `--backend=NAME` | matplotlib backend; `pdf`/`pgf`/`ps`/`svg` save a hardcopy instead of showing a window |
| `--help` | show wrapper usage and exit |

## mcdisplay-webgl / mxdisplay-webgl *(NodeJS/vite-based)*

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `--default` | automatically use instrument default parameter values |
| `--no-mpi` | forward `--no-mpi` to `mcrun` (simulate without MPI) |
| `-n`, `--ncount N` | number of particles to trace (default: 300) |
| `-t`, `--trace N` | visualisation mode (default: 2) |
| `-d`, `--dirname DIR` | override output directory name |
| `--inspect COMP` | show only rays reaching component `COMP` |
| `--nobrowse` | do not open a web browser |
| `--timeout SEC` | shutdown time of the local viewer web server (default: 300) |

*(The viewer ships prebuilt; only if its `dist/` is missing does the first run perform a one-time `npm install && npm run build`, which needs internet access and can take minutes.)*

## mcdisplay-webgl-classic / mxdisplay-webgl-classic

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `--default` | automatically use instrument default parameter values |
| `--no-mpi` | forward `--no-mpi` to `mcrun` (simulate without MPI) |
| `-n`, `--ncount N` | number of particles to trace (default: 300) |
| `-t`, `--trace N` | visualisation mode (default: 2) |
| `-d`, `--dirname DIR` | override output directory name |
| `--inspect COMP` | show only rays reaching component `COMP` |
| `--first COMP` / `--last COMP` | zoom range start/end component |
| `--invcanvas` | invert canvas background |
| `--nobrowse` | do not open a web browser |

## mcdisplay-cad / mxdisplay-cad *(bonus: export instrument geometry to CAD)*

Requires the `cadquery` Python package.

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `--default` | automatically use instrument default parameter values |
| `--no-mpi` | forward `--no-mpi` to `mcrun` (simulate without MPI) |
| `-n`, `--ncount N` | number of particle rays to include (default: 0 — geometry only) |
| `--dirname DIR` | override output directory name |
| `-t`, `--trace N` | `mcrun --trace` level: 1 (default) legacy wireframe, 2 solid primitives |
| `-s`, `--segments N` | tessellation resolution (circle segments, default: 16) |
| `--tubes` | draw lines as solid cylinders (rays too, when `-n > 0`) |
| `-f`, `--format FMT` | output format: `glb` (default), `gltf`, `step`, `stl`, `vrml`, `obj`, `ply`, `off`, `xml`/`x3d`, `vtk`, `vtkjs` |

## mcdisplay-plopp / mxdisplay-plopp

Requires `scipp` and `plopp` (plus `ipympl` and `jupyterlab` for the interactive UI). Without `-o`, opens a 3D JupyterLab UI.

| Option | Description |
|---|---|
| `INSTR` | instrument file (`.instr` or compiled binary) |
| `name=value ...` | simulation parameters |
| `-n N` | number of rays to trace (default: 100) |
| `-o`, `--output FILE` | write static png/pdf/svg 2D projections instead of opening the UI |
| `-y`, `--yes` | forwarded to `mcrun`: use defaults for unset parameters (always on in the UI) |

## mcdisplay-mantid / mxdisplay-mantid *(export Mantid instrument definition)*

Writes `INSTR.xml`, a Mantid IDF built from the instrument geometry and its Mantid-flagged detector monitors.

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `--default` | automatically use instrument default parameter values |
| `--dirname DIR` | override output directory name |
| `-n`, `--ncount N` / `--mpi N` | accepted, but always forced to 1 |

## mcdisplay-matlab / mxdisplay-matlab *(legacy Matlab/Octave/iFit variant)*

A second, older implementation living under `tools/matlab` as a bash wrapper around a `.m` script rather than Python. Runs under **Matlab** or **Octave** — the wrapper auto-detects whichever is available, preferring Matlab, then Octave, and falls back to the plain Python `mcdisplay` if neither is found.

| Option | Description |
|---|---|
| `INSTR` | instrument file (`.instr` or compiled binary) |
| `name=value ...` | simulation parameters, forwarded to `mcrun` |
| `-m` | explicitly request Matlab |
| `-o` | explicitly request Octave |
| `-h` | show wrapper usage and exit |
| `-n N`, `--ncount N` | number of particles to simulate |
| `--inspect=COMP` | only plot components matching `COMP` — a partial name (e.g. `Monitor`), or an interval (`Monok:Sample`, `2:10`, `2:end`) |
| `-png`, `-jpg`, `-fig`, `-eps`, `-pdf`, `-tif` | *(parsed by `mcdisplay.m` itself, not the wrapper)* export the 3D view to the given format |

*(`-m`/`-o`/`-h` are wrapper-level flags and must come first; the export-format flags and `--inspect` are forwarded as-is to the underlying `.m` script.)*

---
[&larr; back to overview](README.md)
