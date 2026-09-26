[&larr; back to overview](README.md)

# Code generators: mcstas / mcxtrace

The actual compiler step underneath `mcrun -c` / `mcgui`'s "Compile" action: translates an `.instr` file into C, which is then compiled into the executable simulation. Identical options on both sides (only the tool name, and which environment variable is consulted for the component search path, differ).

## mcstas / mcxtrace

| Option | Description |
|---|---|
| `file` | the `.instr` file to translate |
| `-o FILE`, `--output-file=FILE` | place the generated C output in `FILE` |
| `-v`, `--version` | print the full version header |
| `--version-num` | print the version number only |
| `--verbose` | display compilation process steps |
| `-I DIR`, `--search-dir=DIR` | append `DIR` to the component search list |
| `-t`, `--trace` | enable trace mode for instrument display (on by default) |
| `--no-trace` | disable trace mode |
| `--no-main` | do not create `main()` (for external embedding) |
| `--no-runtime` | do not embed the run-time libraries |
| `--source` | embed the instrument source code in the executable |

*(The component search path also defaults to whatever the `MCSTAS`/`MCXTRACE` environment variable points at, if set. The help text's usage line also shows a `-p` flag that isn't explained in the option list itself. Ordinarily invoked indirectly via `mcrun`/`mxrun` or `mcgui`/`mxgui`, rather than run by hand.)*

## mcstas-pygen / mcxtrace-pygen — convert an instrument to Python

A binary code-generator tool, sibling to `mcstas`/`mcxtrace` themselves: translates an `.instr` file into a Python module built on **[McStasScript](https://panosc-vinyl.github.io/McStasScript/)** instead of C, so the instrument can be constructed, run, and plotted from Python instead of the `.instr`/`mcrun` route.

| Option | Description |
|---|---|
| `file` | the `.instr` file to translate |
| `-o FILE`, `--output-file=FILE` | place the generated Python output in `FILE` |
| `-v`, `--version` | print the full version header |
| `--version-num` | print the version number only |
| `--verbose` | display compilation process steps |
| `--lint` | generate a `.py` script for McStasScript-style "diagnostic" linting |

*(Its usage synopsis also lists `-I dir1 ...`, `-t`, `-p`, `--no-main`, and `--no-runtime`, inherited from sharing the same usage line as `mcstas`/`mcxtrace` — but none of these are actually explained in `mcstas-pygen`/`mcxtrace-pygen`'s own `--help` text, so treat them as unconfirmed for this variant.)*

The generated module exposes a `make()` function returning a McStasScript instrument object, e.g.:
```python
import mcstasscript as ms
from INSTRUMENT_generated import make
instr = make()
instr.show_diagram()
instr.show_instrument()      # backend='webgl-classic' by default; also 'window' (pyqtgraph), 'webgl', 'pythreejs'
data = instr.backengine()
ms.make_sub_plot(data)
```

See the [McStasScript documentation](https://panosc-vinyl.github.io/McStasScript/) for everything else you can do with the generated instrument object.

## mcstas-jupylab / mcxtrace-jupylab — explore an instrument in Jupyter

A convenience wrapper around `mcstas-pygen`/`mcxtrace-pygen`: converts the given instrument, drops it into a ready-made notebook template (imports, `show_diagram()`/`show_instrument()`/`backengine()`/plotting cells, all pre-written), and launches `jupyter lab` on it.

| Option | Description |
|---|---|
| `INSTR` | the `.instr` file to convert and open in a new Jupyter Lab notebook |

---
[&larr; back to overview](README.md)
