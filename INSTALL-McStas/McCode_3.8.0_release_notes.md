# McStas & McXtrace 3.8.0 — Release Notes

*Covering all changes since the last full release, 3.7.9 (July 2, 2026), including everything that has already shipped quietly via the intermediate conda-forge releases 3.7.10–3.7.25. Roughly 90 pull requests, July 2 – September 10, 2026.*

## Highlights

3.8.0 brings a full family of new **difference- and co-plotting tools** (`mcplotdiff-*` and `mccoplot-*`) across all three plotting backends, a substantially overhauled `mctest`, a real correctness fix to the KISS random-number generator under `SPLIT`, continued hardening of the McCode grammar against the new `mcstas-antlr` parser, three newly imported instruments (including the first community-contributed SPODI powder diffractometer from MLZ Garching), and a long tail of CI/test-suite stabilisation that quietly made every one of the 3.7.1x/3.7.2x conda releases possible.

The headline item, though, is a ground-up rewrite of the **McStas and McXtrace manuals** — the first full makeover since the McStas 2.x and McXtrace 1.x series — described below.

As always, a warm thank-you to everyone who contributed code, instruments and fixes this cycle — see the **Contributors** section at the end, including three brand-new contributors to the project.

---

## Documentation — the McStas & McXtrace manuals, rewritten

This release includes the first full makeover of the McStas and McXtrace reference manuals since the McStas 2.x and McXtrace 1.x series respectively — by file count, the single largest change in this release cycle ([#2592](https://github.com/mccode-dev/McCode/pull/2592)).

Tool-usage chapters (`mcrun`/`mxrun`, `mcplot`, `mcdisplay`, `mcgui`, `mcdoc`, and the rest) were rewritten against the actual current command-line interfaces in `tools/Python`, rather than the older, partly stale prose describing them. Component reference chapters — the per-component parameter tables and descriptions in the manual appendices — are now generated directly from the current `.comp` source headers, closing a long-standing gap where a manual's parameter listing could quietly drift out of sync with what a component actually accepts; the `mcdoc` documentation-generation tool itself was substantially reworked to support this. Along the way, a long list of smaller inaccuracies were corrected: the description of the default KISS vs. optional Mersenne-Twister random-number engines was fixed to match the code (nicely dovetailing with this release's RNG/`SPLIT` correctness fix above), components listed as obsolete in the appendices are now explicitly labelled as such, stale legacy sections and an old release info block were removed, links were moved to https, and a batch of LaTeX build-pipeline fixes (Ångström-symbol rendering, non-UTF8 input handling, `verbatim` vs. `lstlisting` environments, hyperlink rendering) keep the manuals building cleanly end to end. The McXtrace manual additionally had a dedicated pass to remove copy-paste leftovers from the McStas manual and bring its "running" and RNG sections in line with McStas's.

This revision was carried out with **AI-assisted drafting**: source material — the classic McStas usage articles, the grammar/`cogen.c` code generator, the example instruments, the component sources, and the tool CLIs — was given to an AI coding agent to draft the updated text, which was then reviewed and merged by @willend. Given the scale of the rewrite, **minor errors or omissions in the manuals should be expected** in this release; please report anything you spot via a GitHub issue so it can be corrected in a follow-up conda release.

---

## New tooling (common to McStas and McXtrace)

### `mcplotdiff-html` / `mcplotdiff-pyqtgraph` / `mcplotdiff-matplotlib` — difference plotting

A new family of tools that plots the *difference* between two simulation results, monitor by monitor: `diff.monN = a.monN − b.monN` for every monitor present (with matching binning) in both datasets. This is the fast way to see exactly how a component tweak, a parameter change, or a McCode version upgrade moved your results, or to sanity-check MPI vs. non-MPI output. 1D monitors get an error-propagated difference curve; 2D monitors get a diverging blue/white/red colour scale centred on zero, with a signed log option so small differences stay visible without losing their sign. By default the difference datasets are also written out as ordinary McCode `.dat` files (with an `mccode.sim` index), so a diff run is itself a normal, reopenable McCode simulation folder. The tool was built out first for the browser-based plotter (`mcplotdiff-html`, [#2560](https://github.com/mccode-dev/McCode/pull/2560)) and then ported to `pyqtgraph` ([#2562](https://github.com/mccode-dev/McCode/pull/2562)) and `matplotlib` ([#2564](https://github.com/mccode-dev/McCode/pull/2564)), before being reworked to report percentage differences ([#2599](https://github.com/mccode-dev/McCode/pull/2599)) and taught to write out its own diff datasets ([#2585](https://github.com/mccode-dev/McCode/pull/2585)). `mcviewtest`, the test-suite viewer, now spawns it automatically ([#2568](https://github.com/mccode-dev/McCode/pull/2568)).

### `mccoplot-html` / `mccoplot-pyqtgraph` / `mccoplot-matplotlib` — overlay ("co-")plotting

The companion to `mcplotdiff`: instead of subtracting two datasets, `mccoplot` overlays their 1D monitors on the same axes for a direct visual comparison of curve shape and position. It shares `mcplotdiff`'s command-line syntax and monitor-matching logic, and supports comparing not just two but *n* datasets at once ("coplot solution with n x coplots", [#2594](https://github.com/mccode-dev/McCode/pull/2594)). Introduced in [#2587](https://github.com/mccode-dev/McCode/pull/2587), with a McXtrace-side fix in [#2589](https://github.com/mccode-dev/McCode/pull/2589), and now also wired into `mcviewtest` ([#2588](https://github.com/mccode-dev/McCode/pull/2588), [#2595](https://github.com/mccode-dev/McCode/pull/2595)).

### `mctest` — expanded test/benchmark front-end

`mctest`, the installation test-and-benchmark tool used throughout our own CI, picked up several user-facing improvements this cycle: a `-s`/`--seed` option to control the RNG seed used in test runs ([#2575](https://github.com/mccode-dev/McCode/pull/2575)), an option to emulate McCode's classic "Unix epoch" default seed for reproducibility with older results ([#2576](https://github.com/mccode-dev/McCode/pull/2576)), and a new `--strict` mode that fails a test if an instrument is missing its `%Example:` header line(s) — helpful for catching under-documented instruments ([#2602](https://github.com/mccode-dev/McCode/pull/2602), with a spillover-bug fix in [#2604](https://github.com/mccode-dev/McCode/pull/2604)). A round of general robustness fixes followed in [#2603](https://github.com/mccode-dev/McCode/pull/2603) and [#2613](https://github.com/mccode-dev/McCode/pull/2613) (a GPU-box-specific failure), and @mads-bertelsen-agentic fixed a `TypeError` on test re-runs caused by `displaytime` never being restored from the saved JSON state ([#2624](https://github.com/mccode-dev/McCode/pull/2624)).

### `mcplot-html` refinements

The browser-based 1D/2D plotter (used from `mctest` and elsewhere) received a round of polish: a slider to control the plot iframe size ([#2548](https://github.com/mccode-dev/McCode/pull/2548)), several geometry/scaling adjustments ([#2547](https://github.com/mccode-dev/McCode/pull/2547), [#2549](https://github.com/mccode-dev/McCode/pull/2549), [#2550](https://github.com/mccode-dev/McCode/pull/2550)), a fix so 2D colourbars stay visible in more cases ([#2563](https://github.com/mccode-dev/McCode/pull/2563)), and — on the `matplotlib` side — a meaningful speed-up for large 2D matrices ([#2570](https://github.com/mccode-dev/McCode/pull/2570)) and a fix for legends that had stopped being draggable ([#2614](https://github.com/mccode-dev/McCode/pull/2614)).

### Smaller tooling improvements

* `mcrun`/`mxrun`: multi-scan `-M` can now be combined with list-scan `-L` ([#2618](https://github.com/mccode-dev/McCode/pull/2618)); `mccode.dat` is now plottable in the NeXus-scan case ([#2619](https://github.com/mccode-dev/McCode/pull/2619)).
* `mcgui`/`mxgui`: non-numeric entries in the step-count field are now passed through to `mcrun` for interpretation rather than rejected up front ([#2625](https://github.com/mccode-dev/McCode/pull/2625)).
* `mcdoc`: `%Example:` lines are now listed in bold ([#2636](https://github.com/mccode-dev/McCode/pull/2636)).
* Windows batch-wrappers for the various tools were corrected and rectified ([#2566](https://github.com/mccode-dev/McCode/pull/2566), [#2567](https://github.com/mccode-dev/McCode/pull/2567)).
* `mcviewtest` picked up several fit-and-finish fixes: tolerating a trailing `/` in a reference path ([#2593](https://github.com/mccode-dev/McCode/pull/2593)) among others noted above.

---

## Code generator & grammar (common to McStas and McXtrace)

* **Fixed a real RNG correctness bug**: the KISS generator's per-particle random state is a 7-word array, but the code generated for restoring state after a `SPLIT` loop only copied a single word — silently corrupting the RNG state on every `SPLIT`. The generated code now loops over the full `RANDSTATE_LEN` state. Separately, the Mersenne-Twister generator's seed was never actually being applied at start-up due to a stale `#if RNG_ALG == 2` check (should have been `== 1`). Both fixed by @mads-bertelsen-agentic in [#2620](https://github.com/mccode-dev/McCode/pull/2620) — worth an upgrade on its own if you use `SPLIT` with the KISS generator.
* **`COPY(instance)` simplified**: copying a component instance via `COPY(...)` no longer also transfers its `EXTEND` block and other instance-only attachments, matching what most users already expected and documented via a new ADR ([#2627](https://github.com/mccode-dev/McCode/pull/2627); two instruments that relied on the old behaviour were updated in [#2640](https://github.com/mccode-dev/McCode/pull/2640)).
* **`mcstas-antlr` compatibility**: work continued on validating the instrument-definition grammar against the new ANTLR-based parser, now run in CI on every commit ([#2610](https://github.com/mccode-dev/McCode/pull/2610)), with follow-up grammar edits ([#2612](https://github.com/mccode-dev/McCode/pull/2612)) and missing pragmas ([#2616](https://github.com/mccode-dev/McCode/pull/2616)) to keep the two parsers in step.
* **`mccode-pygen`** (the Python-instrument code generator) now carries instrument input parameters through to the generated base `.py` file ([#2601](https://github.com/mccode-dev/McCode/pull/2601), building on a suggestion from @MilanKlausz in [#2554](https://github.com/mccode-dev/McCode/pull/2554)/[#2555](https://github.com/mccode-dev/McCode/pull/2555)).
* **Scan-syntax collision fixed**: an ambiguity between the `stdlib::` component-library namespace syntax and `mcrun`'s `min:delta:max` scan-range syntax could cause mis-parsing; resolved in [#2626](https://github.com/mccode-dev/McCode/pull/2626).
* **Runtime cleanup**: removed unneeded use of `GETPATH()` and fixed issue #2606 in the runtime library ([#2609](https://github.com/mccode-dev/McCode/pull/2609)).
* **Performance tip, courtesy of @g5t**: a fix to a Union test instrument ([#2591](https://github.com/mccode-dev/McCode/pull/2591)) is a good general reminder — a `WHEN(...)` clause is evaluated once per particle in the hot `TRACE` loop, so a string comparison there (`strcmp`) is much more expensive than precomputing an integer flag once in `INITIALIZE` and testing that instead.

---

## Infrastructure, build system & CI (common to McStas and McXtrace)

* **New CMake `FetchContent` integration** for Greg Tucker's `mcstas-chopper-lib`, so chopper-related instruments can pull the library straight from its own repository at configure time instead of vendoring it ([#2631](https://github.com/mccode-dev/McCode/pull/2631)), with a test instrument added to exercise it ([#2632](https://github.com/mccode-dev/McCode/pull/2632)).
* **Test-suite consolidation**: the conda and non-conda "basictest" workflows were merged into one, now using the new `mcviewtest`/`mcplotdiff`/`mccoplot` tooling throughout ([#2577](https://github.com/mccode-dev/McCode/pull/2577)), followed by a steady stream of stabilisation work: fixing flaky nightly output ([#2578](https://github.com/mccode-dev/McCode/pull/2578), [#2581](https://github.com/mccode-dev/McCode/pull/2581)), tuning worker/ncount limits so runners aren't killed by CI time limits ([#2582](https://github.com/mccode-dev/McCode/pull/2582)), running with 2 cores on macOS and auto-detected elsewhere ([#2583](https://github.com/mccode-dev/McCode/pull/2583)), using `mpi=2` on macOS in basictests too ([#2586](https://github.com/mccode-dev/McCode/pull/2586)), tagging datasets with the GitHub worker ID so parallel runs don't collide ([#2598](https://github.com/mccode-dev/McCode/pull/2598)), and a `RUNALL` trigger whenever more than 4 components change in a PR ([#2641](https://github.com/mccode-dev/McCode/pull/2641)). `cif2hkl` was disabled in `Test_Powders` to remove a source of platform-dependent flakiness ([#2634](https://github.com/mccode-dev/McCode/pull/2634)).
* **Windows packaging**: the `autobuild` download step for the InnoSetup installer was repaired ([#2573](https://github.com/mccode-dev/McCode/pull/2573)).
* **Housekeeping**: release-doc updates for 3.7.9 ([#2545](https://github.com/mccode-dev/McCode/pull/2545)), an updated PR template ([#2559](https://github.com/mccode-dev/McCode/pull/2559)), an updated notebook template to match McStasScript 0.0.89 ([#2580](https://github.com/mccode-dev/McCode/pull/2580)), various further CI workflow fixes ([#2615](https://github.com/mccode-dev/McCode/pull/2615)), and general instrument-header/README hygiene across the test suite ([#2639](https://github.com/mccode-dev/McCode/pull/2639)).

---

## McStas-centric changes

**New instruments:**

* **`FRMII_SPODI` / `FRMII_SPODI_MULTI`** — a model of the SPODI high-resolution powder diffractometer at the FRM II reactor (MLZ, Garching), contributed by V. Kochetov, C. Hauf, M. Hoelzel and A. Senyshyn, via GitHub by first-time contributor **@kinetik161** ([#2608](https://github.com/mccode-dev/McCode/pull/2608)). Enabled to run on GPU/OpenACC shortly after ([#2623](https://github.com/mccode-dev/McCode/pull/2623)).
* **Three imaging instruments** imported from various sources ([#2569](https://github.com/mccode-dev/McCode/pull/2569)): `PSI_ICON` (the ICON imaging beamline at PSI), and the e-learning instruments `Radiography_Lithium_Battery` and `Radiography_Sword` (both by Mads Bertelsen and Viktor L. Holm, University of Copenhagen).
* Thanks to the new `mcstas-chopper-lib` `FetchContent` integration (see Infrastructure below), McStas installations now also gain two new contributed components — `Masked_ESS_butterfly` and `NXdisk_chopper` (a NeXus-standard disk chopper) — plus their three accompanying test/example instruments (`Masked_ESS_butterfly_image`, `NXdisk_chopper_display`, `NXdisk_chopper_image`), fetched from Greg Tucker's repository at build time and installed straight into the regular `mcstas-comps/contrib` and `examples/Tests_optics` locations, so they show up in `mcdoc`, `mcgui` and the example browser exactly as if they'd always shipped with McStas.

**Component and instrument fixes:**

* Fixed `PowderN`'s erroneous `order` parameter by @Lomholy ([#2622](https://github.com/mccode-dev/McCode/pull/2622)), following an earlier SANS-spheres intensity fix from the same contributor ([#2600](https://github.com/mccode-dev/McCode/pull/2600)).
* First-time contributor **@MilanKlausz** patched issue #2637: gravity in `Vertical_Bender` was always disabled regardless of the `gravitation` setting ([#2638](https://github.com/mccode-dev/McCode/pull/2638)).
* `ILL_SALSA`: linter fixes and an added `n reflections` parameter, by @Lomholy ([#2572](https://github.com/mccode-dev/McCode/pull/2572)).
* **Union framework**: a new `nowritefile` parameter was added to all Union loggers and abs_loggers, by @mads-bertelsen-agentic ([#2607](https://github.com/mccode-dev/McCode/pull/2607)), letting a logger accumulate results without writing its own output file — handy when only a combined/derived output is needed.
* ESS-source description work and comparison against MCPL-based inputs ([#2596](https://github.com/mccode-dev/McCode/pull/2596)), with a follow-up fix for an ESS/MCPL instrument compilation failure under OpenACC ([#2597](https://github.com/mccode-dev/McCode/pull/2597)).
* Filenames used in `Monitor_nD` instances of the MCPL test instruments no longer include timestamps, making output easier to diff ([#2574](https://github.com/mccode-dev/McCode/pull/2574)).
* Added a second `MCPL_output` producing neutrons in the instrument coordinate frame to the ESS MCPL example ([#2629](https://github.com/mccode-dev/McCode/pull/2629)); a moved MCPL reference file was subsequently accounted for ([#2630](https://github.com/mccode-dev/McCode/pull/2630)).

---

## McXtrace-centric changes

* **GPU/OpenACC**: continued work toward resolving issue #2514 ([#2556](https://github.com/mccode-dev/McCode/pull/2556)), a likely fix for the `Abs_objects` test on GPU ([#2557](https://github.com/mccode-dev/McCode/pull/2557)), and — while the underlying issue is tracked down — a temporary workaround disabling the affected key tests ([#2558](https://github.com/mccode-dev/McCode/pull/2558)).
* **`Filter.comp`**: better handling of non-standard input files ([#2552](https://github.com/mccode-dev/McCode/pull/2552)).
* `mxcoplot-html` fixed to work correctly on McXtrace datasets ([#2589](https://github.com/mccode-dev/McCode/pull/2589), part of the new `mccoplot` family above).

---

## Contributors

Thank you to everyone who contributed to this release:

* **@willend** (Peter Willendrup) — the majority of tooling, plotting, McXtrace GPU work, CI/testsuite stabilisation and release engineering.
* **@Lomholy** — `PowderN` and SANS-spheres fixes, `ILL_SALSA` improvements.
* **@g5t** (Gregory Tucker, ESS) — the TRACE-path performance fix in the Union test suite ([#2591](https://github.com/mccode-dev/McCode/pull/2591)), and author of the external `mcstas-chopper-lib` library now pulled in via the new CMake `FetchContent` integration.

**New contributors this release** — welcome, and thank you for your first pull requests to McCode:

* **@kinetik161** (V. Kochetov, C. Hauf, M. Hoelzel, A. Senyshyn — MLZ, Garching) — the `FRMII_SPODI` powder diffractometer instruments ([#2608](https://github.com/mccode-dev/McCode/pull/2608)).
* **@mads-bertelsen-agentic** (Mads Bertelsen) — the RNG/`SPLIT` correctness fix ([#2620](https://github.com/mccode-dev/McCode/pull/2620)), the Union `nowritefile` parameter ([#2607](https://github.com/mccode-dev/McCode/pull/2607)), and an `mctest` re-run fix ([#2624](https://github.com/mccode-dev/McCode/pull/2624)).
* **@MilanKlausz** (Milan Klausz, ESS) — the `Vertical_Bender` gravity fix ([#2638](https://github.com/mccode-dev/McCode/pull/2638)), building on an earlier pygen suggestion ([#2554](https://github.com/mccode-dev/McCode/pull/2554)).

---

**Full changelog:** https://github.com/mccode-dev/McCode/compare/v3.7.9...v3.8.0
