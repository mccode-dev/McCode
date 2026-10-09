# AGENTS.md — McCode (McStas / McXtrace)

Guidance for AI coding agents working in the McCode repository (`mccode-dev/McCode`).
McCode is the shared code base behind **McStas** (neutron) and **McXtrace** (X-ray)
Monte Carlo ray-tracing simulation packages. Read this before making changes.

---

## 1. What the project is

- A **domain-specific language** (instrument files, `.instr`) plus a **library of
  components** (`.comp`) is compiled by the McCode code generator into a single C
  program per instrument, which is then compiled and run.
- Two flavours share most infrastructure:
  - **McStas** — neutrons (`mcstas-comps`, `mcstas-r.h`, `mcrun`, `mcplot`, …)
  - **McXtrace** — X-rays (`mcxtrace-comps`, `mcxtrace-r.h`, `mxrun`, `mxplot`, …)
- Shared runtime lives in `mccode-r.h.in` / `mccode-r.c`; flavour-specific runtime
  in `mcstas-r.h` / `mcxtrace-r.h`.
- Supporting pieces: Python tooling (`tools/Python/…`), LaTeX manuals
  (`doc/manuals/…`), CMake build system, conda-based packaging and app bundles.

### Golden rule: McStas and McXtrace move together
Most changes (runtime, tools, docs, build) must be applied to **both** flavours.
The structure is shared but the details are **asymmetric** — constants, tool names
(`mcrun` ↔ `mxrun`), particle state and some sections differ. Port deliberately and
audit each side independently; never blind-copy.

---

## 2. Languages and targets

| Area | Language / tech |
|---|---|
| Runtime, components, generated code | C99 (must also build with MSVC) |
| GPU acceleration | OpenACC (NVIDIA `nvc`, `-acc=gpu`) |
| Parallelism | MPI (and OpenACC on GPU) |
| Helper programs (e.g. `cif2hkl`) | Fortran (gfortran **and** Flang) |
| Tooling (`mcrun`, `mcplot`, `mcdoc`, …) | Python 3.x |
| HTML plotting frontends | JavaScript (D3.js) + HTML templates |
| Build / packaging | CMake (+ `CMakePresets.json`), conda-forge, micromamba |
| Documentation | LaTeX (PDF), pandoc-based HTML pipeline |

### Supported platforms — every change must survive all of them
- Linux: gcc and clang
- macOS: clang, **both** arm64 and x86_64
- Windows: MSVC (`cl.exe`) and Flang for Fortran; x86_64, including x86_64 conda on ARM64 Windows

If you cannot test a platform, say so explicitly rather than assuming it works.

---

## 3. Hard rules

1. **Never edit generated C files** (the `.c` produced from an `.instr`). Fix the
   component, the runtime, or — only when explicitly asked — the code generator.
   Treat code-generator changes as out of scope unless the maintainer requests them.
2. **Minimal, targeted changes.** No new flags, options, abstraction layers or
   refactors beyond what the task needs. Prefer the simplest direct fix.
3. **Propose, then apply.** For non-trivial edits, describe the change and wait for
   confirmation unless told to edit in place. Once told to fix something, just fix
   it — no preamble.
4. **Verify against the source, not the docs or comments.** Runtime header comments
   can be wrong; check the actual code (e.g. conversion constants in
   `mcstas-r.h` / `mcxtrace-r.h` / `mccode-r.h.in`).
5. **Do not silently change numerical behaviour.** Any change that could alter
   simulation results (RNG use, physics, precision, ordering) must be called out.
6. **Do not add dependencies** without discussion — the stack is packaged on
   conda-forge for three OSes.

---

## 4. Components (`.comp`)

Standard section layout: `DEFINE COMPONENT`, `SETTING PARAMETERS`, `SHARE`,
`DECLARE`, `INITIALIZE`, `TRACE`, `SAVE`, `FINALLY`, `MCDISPLAY`, with a doc
header (`%I`, `%D`, `%P`, `%L`, …).

- **One code path for CPU and GPU.** Helper functions used in `TRACE` should be made
  GPU-callable with `#pragma acc routine` (follow existing examples) rather than
  duplicated behind `#ifndef OPENACC` branches.
- **No per-particle heap allocation** in `TRACE`. Use fixed-size stack arrays or
  allocate once in `INITIALIZE`.
- `DECLARE` variables are effectively read-only after `INITIALIZE`; anything written
  in `TRACE` must be per-particle / per-thread.
- Shared accumulators (monitors) need `#pragma acc atomic` on GPU.
- Guard RNG edge cases: `rand01()` can return exactly 1.0 (or 0.0) — index
  selection must not go out of bounds.
- Watch operator precedence in index arithmetic; out-of-bounds cache lookups have
  caused silent heap corruption before.
- Initialise every geometry field (e.g. surface normals for all shape types,
  including OFF/mesh geometry) — uninitialised data shows up as platform-dependent NaNs.
- `printf` format specifiers must match argument types exactly; mismatches produce
  platform-divergent garbage.
- `%P` parameter docs: use the canonical unit notation and Ångström spelling used
  elsewhere in the library (`mcdoc` renders these into the manuals).

---

## 5. Portability pitfalls (read before touching C/Fortran)

### Complex numbers
MSVC does not implement C99 `_Complex` arithmetic. Use the McCode complex wrapper
(`mccode-complex-lib.h`: `cdouble`, `cplx`, `cadd`, `csub`, `cmul`, `cdiv`, `radd`,
`cneg`, …) instead of native complex operators.
- There is no `rsub`: real − complex is `radd(r, cneg(z))`.
- Do **not** type-pun `cdouble*` ↔ `double*` or `double complex*`; struct layout is
  not guaranteed.
- If a macro must handle both `double` and `cdouble`, split it into two explicitly
  typed variants.
- Bundled libraries that take complex pointers (e.g. eigen-solver helpers) need the
  same treatment.
- Do not use `I` as an identifier (variable, parameter, macro). The system header
  `<complex.h>` (also included by `<tgmath.h>`, e.g. in the SasView components)
  defines `I` as a macro for the imaginary unit, so such code breaks when an
  instrument combines it with those components. This clash with a system header
  can not be avoided by McCode in general.

### Fortran
- Windows has a ~1 MB default stack: avoid large runtime-sized automatic arrays —
  use `allocatable`.
- Use modern intrinsics (`date_and_time`, not `idate`/`itime`) and
  `open(newunit=…, iostat=…, iomsg=…)`.
- Validate inputs before opening files.

### Windows / macOS environment
- On ARM64 Windows, prefer `PROCESSOR_ARCHITEW6432` over `PROCESSOR_ARCHITECTURE`
  for architecture detection (emulated shells report `AMD64`).
- The VS installer ignores standard proxy environment variables (uses WinHTTP).
- macOS app bundles that contain only scripts need `LSRequiresNativeExecution` in
  `Info.plist` to avoid spurious Rosetta prompts; micromamba resolves the correct
  conda platform at runtime.

---

## 6. MPI, RNG and reproducibility

- Default RNG is KISS (Mersenne Twister available as a build-time option).
- With MPI, each rank seeds from the base seed plus its rank, so **results depend
  on the number of ranks** even with a fixed seed. When comparing runs or
  platforms, keep `-n`, `--seed` **and** `-np` identical before suspecting
  compiler/FMA differences.
- Any proposal to change seeding (e.g. per-particle seeding, counter-based RNGs)
  is a numerical-behaviour change — flag it.

---

## 7. GPU / OpenACC work

- Typical build: `nvc -acc=gpu -gpu=mem:managed -fast -DOPENACC`. Managed memory
  removes a whole class of host/device transfer hypotheses.
- Diagnosing CPU-vs-GPU divergence:
  1. Same seed, same ncount, compare monitor by monitor to localise the first
     diverging component.
  2. Then audit that component's `TRACE` for shared writes, uninitialised state,
     and precision-sensitive branches (discriminants near zero, near-tangent hits).
  3. Check whether the difference is in total intensity or only distribution, and
     whether it is stable across repeated GPU runs.
- `nvc` internal compiler errors can be context-sensitive. Combine **minimal
  synthetic reproducers** with **bisection of the real generated file** — both are
  needed. Known trouble pattern: `acc atomic` on doubly-indirected pointers
  (`double **` through a struct).

---

## 8. Python tooling (`tools/Python/…`)

- Key entry points: `mcrun` (run/scan/optimise orchestration), `mcplot` frontends
  (matplotlib, pyqtgraph, HTML/D3), `mcdoc`, `mcdisplay`, shared `mccodelib`.
- **Feature parity across plot frontends:** an option added to one
  (matplotlib / pyqtgraph / HTML) should be added to all, with matching behaviour.
- Scan output layout: numbered subdirectories per step plus top-level
  `mccode.dat` / `mccode.sim`. 2D `.dat` files stack `I`, `I_err`, `N` blocks —
  parse only the first for intensity.
- Be careful extending `mcrun` parameter syntax: it must not collide with existing
  forms such as NCrystal `stdlib::…` configuration strings.
- After merges touching scan/optimisation code, re-validate NeXus output and
  non-list scan output — these have broken silently before.
- Validate new features against **real simulation output**, not just unit tests.
- Avoid legacy/deprecated imports (e.g. `scipy.misc`); don't add heavy dependencies
  for small gains.
- For HTML frontends, test in more than one browser and at narrow viewport widths
  (Playwright works well); avoid CSS that clips content (`overflow: hidden` on
  layout containers).

---

## 9. Documentation (`doc/manuals/…`)

- McStas and McXtrace manuals are maintained in parallel; McStas usually leads and
  McXtrace receives an adapted port (tool names, constants, sections differ).
- PDFs are built via CMake + LaTeX. HTML uses **pandoc with LaTeX preprocessing**
  (flattening `\input`/`\include`); `htlatex`/`make4ht` do not handle the manuals'
  math markup.
- Use `\providecommand` (not `\newcommand`) for shared macros.
- Keep everything UTF-8 end to end (sources, converters, server headers) so
  æ/ø/å and Å render correctly.
- Technical statements (constants, defaults, API) must be checked against the
  runtime source before editing the manual.
- Bulk scripts that rewrite many files must have a **dry-run mode** and be run dry
  first.

---

## 10. Build, packaging and CI

- CMake is the build system; use the provided presets where available.
- Distribution is via conda-forge (feedstocks for the McCode suite and related
  libraries such as NCrystal, MCPL), plus conda-based macOS app bundles and
  Windows installers (micromamba-driven).
- Windows compilers come from VS Build Tools (VS2022, toolsets v142/v143) for
  conda-forge compatibility.
- CI runs on Linux, macOS and Windows — a change is not done until it is green on
  all three.
- Guard against **silent CI failures**: verify downloads (e.g. check the file type
  of a fetched installer), use `set -euo pipefail` in bash, and fail loudly.
- Scripts should be idempotent and unattended-safe: detect existing state, prefer
  modify over reinstall, explicit `--apply`-style flags for destructive actions.

---

## 11. Testing and debugging checklist

- Compile-check every touched C file with warnings on; for memory issues use
  `-fsanitize=address,undefined` (Linux/Docker is more reliable than macOS for
  ASan). For Fortran: `-fcheck=all -ffpe-trap=invalid`.
- Run the relevant test/example instruments (e.g. `Test_<Component>.instr`) on CPU,
  and on GPU when the component is OpenACC-relevant.
- Validate numerical refactors with a standalone comparison test before
  integrating.
- Isolate one variable at a time: MPI rank count → platform → compiler → flags.
- Report clearly what was tested, on which platforms, and what was **not**.

---

## 12. Working with the maintainers

- Small, reviewable commits on a topic branch; one concern per change.
- When editing files with tools that may time out, edit in small chunks
  (~10–20 lines) and re-read the region afterwards to confirm the actual state
  before retrying — never re-apply blindly.
- State assumptions and open questions explicitly; mark anything unverified as such.
- Keep McStas/McXtrace terminology precise and consistent (component names,
  geometry terms, DSL keywords).
