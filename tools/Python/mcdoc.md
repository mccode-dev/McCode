[&larr; back to overview](README.md)

# mcdoc / mxdoc

Generates browsable documentation for installed and local instrument/component files, under `tools/Python/mcdoc`.

| Option | Description |
|---|---|
| `searchterm` | *(optional)* search filter, or a specific `.instr`/`.comp` filename |
| `-i`, `--install` | generate the full installation master doc page |
| `-d`, `--dir DIR` | also include search results from `DIR` (local instruments/components) |
| `-m`, `--manual` | open the user manual PDF |
| `-c`, `--comps` | open the component manual PDF |
| `-w`, `--web` | open the McStas/McXtrace website |
| `-v`, `--verbose` | print a parsing log during execution |
| `-M`, `--md` | also emit Markdown (`.md`) doc files (HTML is always written) |
| `-T`, `--tex` | also emit LaTeX (`.tex`) doc files |
| `-A`, `--all-formats` | emit HTML, Markdown, and LaTeX together |
| `--in-repo` | place output next to the source files instead of the installed docdir (no master page) |
| `-o`, `--outdir DIR` | write results to `DIR` (in-repo use only) |

*(No `searchterm`/`--install`/`--manual`/`--comps`/`--web` at all &rarr; browse the existing installed docs directly.)*

---
[&larr; back to overview](README.md)
