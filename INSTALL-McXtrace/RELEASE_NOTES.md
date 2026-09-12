Changes in McStas/McXtrace 3.8.4

*Covers everything since the last full release, 3.7.9 (July 2, 2026), including all changes already shipped quietly via the intermediate conda-forge releases 3.7.10–3.8.3, and a ground-up rewrite of the McStas/McXtrace manuals.*

*Sorry for any incovenience caused if you installed any of 3.8.1-3... Some doc-details were still not in place!*

## What's Changed

### Common to McStas and McXtrace

#### Manuals
* **First full manual makeover since the McStas 2.x / McXtrace 1.x series.** Tool-usage chapters (`mcrun`/`mxrun`, `mcplot`, `mcdisplay`, `mcgui`, `mcdoc`, ...) rewritten against the actual CLI code; component reference/parameter tables now generated directly from the current `.comp` sources (closing a long-standing manual/code drift); `mcdoc` itself reworked to support it. By @willend in #2592
* Numerous long-standing inaccuracies fixed along the way: RNG engine description (KISS default / MT optional), obsolete components now explicitly labelled as such, stale legacy sections removed, http->https, and a batch of LaTeX build-pipeline fixes; McXtrace manual also had McStas copy-paste leftovers removed and its "running"/RNG sections aligned with McStas's — all by @willend in #2592
* **AI-assisted**: reference material and source code (usage articles, grammar/`cogen.c`, example instruments, component sources, tool CLIs) were given to an AI coding agent to draft the updated text, reviewed and merged by @willend. Given the scale of the rewrite, minor errors/omissions in the manuals are expected — please flag anything you spot via a GitHub issue
* `mcdoc`'s browser-based overview page now also links to per-tool cheat-sheets (`mcrun`, `mcgui`, `mcplot`, `mcdisplay`, `mctest`, `mcdoc`, code generators) and to new standalone Instrument/Component grammar reference pages, all a click away from the component browser by @willend

#### New tooling
* New tools `mcplotdiff-html`/`mcplotdiff-pyqtgraph`/`mcplotdiff-matplotlib`: plot the *difference* between two simulation results, monitor by monitor (`diff.monN = a.monN - b.monN`), with a diverging blue/white/red colour scale for 2D monitors and diff datasets written out as normal, reopenable McCode `.dat` files by @willend in #2560, #2562, #2564 — later reworked to show %-difference in #2599 and to always write diff datasets in #2585
  * `mcviewtest` now spawns `mcplotdiff-html` automatically by @willend in #2568
* New tools `mccoplot-html`/`mccoplot-pyqtgraph`/`mccoplot-matplotlib`: companion to `mcplotdiff`, overlaying two (or more) datasets' 1D monitors on the same axes for direct curve comparison by @willend in #2587, with n-way overlay support in #2594 and a McXtrace-side fix in #2589
  * `mcviewtest` also spawns `mccoplot` generation by @willend in #2588, #2595
* `mctest`: added `-s`/`--seed` option and an option to emulate McCode's classic "Unix epoch" default seed, for reproducible test runs by @willend in #2575, #2576
* `mctest`: new `--strict` mode fails a test if an instrument is missing its `%Example:` header line(s) by @willend in #2602 (spillover-bug fix in #2604), plus general robustness improvements in #2603 and a GPU-box-specific fix in #2613
* `mctest`: fixed `TypeError` on re-runs caused by `displaytime` never being restored from the saved JSON state by @mads-bertelsen-agentic in #2624
* `mcplot-html`: added a slider to control plot iframe size, and geometry/scaling adjustments, by @willend in #2547, #2548, #2549, #2550
* Fix issue #2561 — make 2D-plot colourbars visible in more cases by @willend in #2563
* Speed up execution of `mcplot-matplotlib` (especially for large 2D matrices) by @willend in #2570
* Matplotlib-plotters: fix for non-draggable legends by @willend in #2614
* `mcrun`/`mxrun`: allow multiscan `-M` with list-scan `-L` and new scan/list syntax `par=a:delta:b` for both -N and L modes by @willend in #2618
* Plottable `mccode.dat` in the NeXus-scan case by @willend in #2619
* `mcgui`/`mxgui`: non-numeric entries in nsteps are now deferred to `mcrun` for interpretation instead of rejected up front by @willend in #2625
* `mcdoc`: list `%Example:` lines in bold by @willend in #2636
* Minor tweak on `mcviewtest` — tolerate a trailing `/` in ref path by @willend in #2593

#### Code generator & grammar
* **RNG correctness fix**: the KISS generator's 7-word per-particle random state was only partially backed up/restored across a `SPLIT` loop (silently corrupting RNG state), and the Mersenne-Twister seed was never actually applied at start-up due to a stale `RNG_ALG` check — both fixed by @mads-bertelsen-agentic in #2620
* Cogen: simplify `COPY(instance)` to no longer transfer `EXTEND` et al. by @willend in #2627 (two affected instruments updated in #2640)
* Enable `mcstas-antlr` in McCode GitHub CI by @willend in #2610
* Edits for increased compatibility with `mcstas-antlr` by @willend in #2612
* Consequence-edits from antlr-compatibility: add missing pragmas by @willend in #2616
* Add input parameters to base `.py` instrument generated by `mccode-pygen` by @willend in #2601
* Implement pygen-suggestion from @MilanKlausz (#2554) by @willend in #2555
* Fix for collision between `stdlib::` syntax and `mcrun` `min:delta:max` scan syntax by @willend in #2626
* Fix issue #2606 + other unneeded use of `GETPATH()` by @willend in #2609
* [Ref] move static comparison out of the hot TRACE path by @g5t in #2591 — a good general reminder that a `WHEN(...)` clause runs once per particle, so precomputing a flag in `INITIALIZE` beats a `strcmp()` in TRACE

#### Infrastructure / build system / CI
* New CMake `FetchContent` integration for Greg Tucker's `mcstas-chopper-lib` by @willend in #2631, with a test instrument added in #2632
* Consolidate conda/non-conda 'basictest' workflows and use with new `mcviewtest` by @willend in #2577
* Minor bugfixes for testsuite-workflows/nightlies by @willend in #2578, #2581
* Back to 1e6, testsuite workers are killed by time-limit by @willend in #2582
* Let testsuite run with 2 cores on macOS, auto elsewhere by @willend in #2583
* Use `mpi=2` on macOS also in basictests by @willend in #2586
* Include GitHub worker id as unique identifier for datasets by @willend in #2598
* Trigger 'RUNALL' if > 4 comps were changed by @willend in #2641
* Suppress use of `cif2hkl` in `Test_Powders` by @willend in #2634
* Repair 'autobuild' download of 'innosetup' installer by @willend in #2573
* Release doc updates 3.7.9 by @willend in #2545
* Update pull_request_template.md by @willend in #2559
* Update notebook template to reflect McStasScript 0.0.89 properties by @willend in #2580
* CI: various workflow fixes by @willend in #2615
* Minor instrument header fixes + plug missing README.md's by @willend in #2639
* Rectification of Windows batch-wrappers by @willend in #2566, #2567

### McStas only
* New instrument: import of `FRMII_SPODI` / `FRMII_SPODI_MULTI`, a powder diffractometer at MLZ Garching (contributed by V. Kochetov, C. Hauf, M. Hoelzel and A. Senyshyn) by @kinetik161 in #2608 — enabled to run on GPU/OpenACC by @willend in #2623
* New instruments: import of 3 x imaging instruments from various sources (`PSI_ICON`, `Radiography_Lithium_Battery`, `Radiography_Sword`) by @willend in #2569
* New components (via `mcstas-chopper-lib`, see FetchContent integration above): `Masked_ESS_butterfly` and `NXdisk_chopper`, with their test/example instruments `Masked_ESS_butterfly_image`, `NXdisk_chopper_display` and `NXdisk_chopper_image`, courtesy of @g5t's `mcstas-chopper-lib` — installed directly into the normal `mcstas-comps/contrib` and `examples/Tests_optics` locations, so they appear in `mcdoc`/`mcgui` like any other McStas component
* PowderN: remove erroneous `order` parameter by @Lomholy in #2622
* Sans spheres intensity fix by @Lomholy in #2600
* ILL_SALSA: fix linter and add n reflections to parameters by @Lomholy in #2572
* Patch issue #2637 — `Vertical_Bender` gravity always disabled by @MilanKlausz in #2638
* Union: add `nowritefile` parameter to all Union loggers and abs_loggers by @mads-bertelsen-agentic in #2607
* Work on ESS source description / comparison with MCPL inputs by @willend in #2596
* Fix for compilation failure of ESS/MCPL instrument under openacc by @willend in #2597
* Use filenames in MCPL instrument `Monitor_nD` instances to get rid of timestamps by @willend in #2574
* Add another `MCPL_output` -> producing neutrons in instrument coordinate frame by @willend in #2629
* MCPL file was moved - update instrument file by @willend in #2630

### McXtrace only
* McXtrace: work toward fixing #2514 (GPU) by @willend in #2556
* Likely fix for McXtrace / `Abs_objects` test on GPU by @willend in #2557
* WORKAROUNDS: disable key tests in McXtrace (#2514) by @willend in #2558
* `Filter.comp`: better handling of non-standard input files by @willend in #2552
* Fix for `mxcoplot-html` McXtrace by @willend in #2589

## New Contributors
* @kinetik161 made their first contribution in #2608
* @mads-bertelsen-agentic made their first contribution in #2624
* @MilanKlausz made their first contribution in #2638

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.7.9...v3.8.4
