Changes in McStas/McXtrace 3.10.3

## Highlights

* **One mesh library for all McStas geometry**: every McStas component that reads OFF or PLY files (`Guide_anyshape`, `PowderN`, `Single_crystal`, `Isotropic_Sqw`, `Incoherent`, `Monitor_nD`, `Refractor`, `Shape`, `Lens`, ...) now uses the general `geometry-mesh-lib` instead of `interoff-lib`. They all also accept STL files and trace large meshes faster. `Monitor_nD` mesh detectors now apply He3 absorption, and the new `mcmeshreduce`/`mxmeshreduce` tool shrinks large meshes before use. By Daniel Lomholt Christensen (@Lomholy) and @willend.
* **McXtrace lenses corrected**: `Lens_parab`, `Lens_parab_Cyl`, `Lens_elliptical`, `Lens_CRL_RTM`, `Lens_simple` and `ZonePlate` received a series of fixes. Photons outside the aperture are now absorbed instead of passing through, `Lens_parab` has a circular aperture, `Lens_elliptical` reads the right material data and absorbs along the real path, and `Lens_CRL_RTM` has its factor-2 error in the fast absorption formula fixed. *Note: results for these components change, some substantially.* By @farhi.
* **McXtrace fluorescence corrected**: `Fluorescence`, `FluoPowder` and `FluoCrystal` have a fixed cross-section normalisation, sampling, line selection and `SPLIT` handling, polarisation factors are added, and `Fluo_detector` no longer depends on `Fluorescence`. *Note: results change substantially, for example by a factor 2.7 for `SOLEIL_ROCK`.* By @farhi.
* **New contributed components**: `FlipChopFlipper`, a resonant RF spin flipper for FlipChop at SNS, with the `FlipChop_Hyspec` example by Niels Geerits (@ngeerits95). `Phonon_BvK_PG` (Born-von Kármán phonons in pyrolytic graphite) is updated to Kim Lefmann's (@lefmann) current dynamical-matrix model.
* **`Single_crystal_inelastic` gives reproducible results**: each neutron now selects its (hkl, E) point exactly, instead of reusing lists built for a neighbouring wavevector. Results previously depended on neutron order and MPI split, scattering by about a factor 2 between seeds. *Note: results change.* By @willend.
* **Timing of simulation sections**: the wall-clock time spent in `INITIALIZE`, `TRACE` and `FINALLY` is now written to `mccode.sim` and NeXus output. The new `mccode-speed-diff` developer tool uses it to compare the speed of two McCode versions. Idea by Daniel Lomholt Christensen (@Lomholy), implemented by @willend.
* **Table lookups fixed, library tests added**: `read_table-lib` now handles decreasing tables, constant-step lookups and one-row or one-column 2D tables correctly. *Note: results can change for such tables; the bundled data has no decreasing tables.* Every shared library now has a `Test_lib_*` test instrument (22 for McStas, 13 for McXtrace). By Daniel Lomholt Christensen (@Lomholy) and @willend.
* **Robustness**: a sweep with the undefined-behaviour sanitiser fixed uninitialised values, out-of-range indices and unsafe string copies in the runtime, shared libraries and about 20 components. Components that read input files now find them in the instrument directory and the McStas data folders, like the rest of McStas. By @willend and Daniel Lomholt Christensen (@Lomholy).
* **Nightly test sites**: the McStas and McXtrace test suites, and the ESS instruments tested against McCode main, now publish their results every night as web pages with history and overview plots. By @willend.
* **Smoother `mcgui` and `mcdisplay-webgl`**: in `mcdisplay-webgl(-classic)`, double-click a component to focus on it. `mcgui` shows simulation errors in red with a colour legend, and labels instruments without a `SITE` tag as "No SITE defined". By @willend.

## What's Changed

### Common to McStas and McXtrace

#### Visualisation and GUI
* `mcdisplay-webgl(-classic)`: double-click a component to focus the view on it, by @willend in #3075
* `mcgui`/`mxgui`: simulation errors (stderr) shown in red, with a colour legend (#3078), and instruments without a `SITE` tag listed as "No SITE defined" (#3081), by @willend

#### Runtime and libraries
* Wall-clock timing of `INITIALIZE`, `TRACE` and `FINALLY` written to `mccode.sim` and NeXus output, with a new `Test_Timing_record` instrument and a design note, by @willend in #3047 (idea by Daniel Lomholt Christensen (@Lomholy) in #3041)
* `read_table-lib`: correct `Table_Value`/`Table_Rebin` on decreasing tables and constant-step lookups, and `Table_Value2d` on one-row and one-column tables, ported from Daniel Lomholt Christensen's (@Lomholy) #3043. *Results can change for such tables.* By @willend in #3048
* New `Test_lib_*` test instruments for every shared library: 22 for McStas and 13 for McXtrace, by @willend in #3048
* Fixes from code scanning: `read_table-lib` NULL check (#3036), `cov-lib`, `interoff-lib` and `read_table-lib` (#3027), `printf` arguments in `monitor_nd-lib` and `supermirror-lib` (#3030), `Flex_monitor_*` (#3031), and too permissive patterns in `mcdoc`, `mcgui`, `mcrun` and `mctest` (#3026), by @willend
* Undefined behaviour fixed in the runtime and shared libraries: unterminated string copies and the NeXus `--format` name in `mccode-r`, out-of-range bin indices in `monitor_nd-lib` and `conic.h`, and an uninitialised value in the ESS cold spectrum, by @willend in #3112
* `read_table-lib`: the table is initialised before reading can fail (#3096), and the line buffer of `Table_Read_Handle` is no longer leaked (#3099), by Daniel Lomholt Christensen (@Lomholy)
* `mccode-complex-lib` wrappers are `static inline`, by @willend in #3121
* Code generator: the last line of a C code file without a final newline is no longer lost, by @willend in #3089
* NeXus MPI runs print how to turn the event-list output into a self-contained `mccode.h5` with `h5repack`, by @willend in #3077

#### Python instruments (`mcstas-pygen`)
* Generated McStasScript code uses `add_METADATA` for component `METADATA` (previously the non-existent `append_METADATA`) and keeps the units of instrument parameters, by @willend in #3129

#### Testing (`mctest`)
* `mctest --comp` also selects instruments using components that `INHERIT` the given ones (#3093), and the no-MPI run passes `--openacc` (#3120), by @willend
* `mctest` accepts an upper-case `E` in `%Test` target values (#3038), truncates long filter names in test directory names (#3042), and `--nohtml` makes PDF-only plots (#3028). `mcviewtest` rounds the percentage in its 20 % check as `mctest` does (#3037), by @willend

#### Installation, platforms and packaging
* Debian/Ubuntu suite metapackages also install the webgl-classic display tools (#3018), and so does the McXtrace MinGW metapackage on Windows (#3019), by @willend
* `mcdisplay-webgl`: Vite updated to 6.4.5 (security alert), by @willend in #3023
* New `mcmeshreduce`/`mxmeshreduce` tool to reduce OFF, STL, OBJ and PLY meshes by percentage (`-r`), to a face count (`-n`) or losslessly (`--lossless`), keeping open-mesh boundaries fixed. It uses the optional `trimesh` and `fast-simplification` packages. By @willend in #3117, packaged for Debian/Ubuntu and Windows in #3130
* For contributors: nightly test sites for McStas and McXtrace with history, dashboard and PDF overviews (#3024, #3032, #3033, #3039, #3058, #3060, #3067), a nightly ESS instrument test site using McCode main (#3068–#3074), the new `mccode-speed-diff` tool to compare the speed of two refs (#3056), a `RELEASE-CHECKLIST.md` (#3034, #3035, #3045), and `AGENTS.md` rules for AI coding agents (#3046, #3050), by @willend
* For contributors: OpenACC reference tests on a simulated NVIDIA H100 in CI, by @saqibkh in #3118, extended to McXtrace with instrument and component inputs by @willend in #3119. A manual pre-release workflow (#3098, #3101, #3102), GitHub Actions updates with dependabot (#3127), `mccode-clangformat` keeping preprocessor lines intact (#3124), and `AGENTS.md` asking for `exit(-1)` on errors (#3097), by @willend
* For contributors: CI workflows get read-only default permissions (#3025), basic tests skip builds unless simulation code changed (#3052, #3059), a slow Ubuntu mirror is bypassed (#3051), the test-suite schedules run 4 hours earlier (#3063), and the Debian test jobs use the new McCode apt repository (#3021). The install docs move to each new release automatically (#3014, #3015), by @willend

### McStas only

#### Components
* `geometry-mesh-lib` replaces `interoff-lib` in all McStas components: `Guide_anyshape`, `Guide_anyshape_r`, `PowderN`, `Single_crystal`, `Single_magnetic_crystal`, `Single_crystal_inelastic`, `Isotropic_Sqw`, `Incoherent`, `Refractor`, `Shape`, `Shape_simple` and `Lens`. The library gets straight-line tracing without gravity and a BVH with 8 triangles per leaf, and a new `Test_lib_geometry_mesh_lib` instrument. By Daniel Lomholt Christensen (@Lomholy) and @willend in #3090
* `Monitor_nD`: detector `geometry=` from STL, OFF or PLY files via `geometry-mesh-lib` instead of `interoff-lib`, He3 absorption for mesh detectors, and a warning when Mantid mode is used with a closed mesh. Union loggers and processes updated to match. By Daniel Lomholt Christensen (@Lomholy) and @willend in #3029, briefly reverted (#3065) and re-landed in #3066
* `Monitor_nD_noacc` now uses the shared `monitor_nd-lib`, picking up its NeXus, allocation and initialisation fixes, by @willend in #3061
* `Single_crystal_inelastic`: exact per-neutron (hkl, E) selection without the order-dependent wavevector caches, so results no longer depend on neutron order or MPI split, by @willend in #3020
* `Brilliance_monitor`: the squared weight of the 2D monitor is fixed, so its error bars are correct (intensities unchanged), by @willend in #3044
* `PSD_monitor_4PI`: bin indices kept inside the arrays at the edges, by @willend in #3044
* New `FlipChopFlipper` component, a resonant RF spin flipper for FlipChop at SNS, with the `FlipChop_Hyspec` example instrument, by @ngeerits95 in #3107
* `Phonon_BvK_PG`: dynamical matrix updated to Kim Lefmann's current model (the C/D layer gets its own block, in-plane phases from unit-cell origins), a scaled eigenvalue tolerance so the solver no longer fails to converge, defaults matching the example and an updated test, by @lefmann and @willend in #2344
* `Isotropic_Sqw`: coherent scattering is always selected when incoherent is not, by @willend in #3086 (fixes #3085)
* Components read their input files with `Open_File`, so files are also found in the instrument directory and the McStas data folders (`SNS_source`, `ISIS_moderator`, `ViewModISIS`, `SNS_source_analytic`, `Source_multi_surfaces`, `Guide_tapering`, `Mask`, `PSD_monitor_Filter`, `SANSCurve`, `SANS_benchmark2`, Union `IncoherentPhonon_process` and `Texture_process`), by @willend in #3108, #3109
* Undefined behaviour fixed in 17 contributed components (the `Conics_*` mirrors, the Bispectral mirrors, `Monochromator_bent`, `Phonon_BvK_PG`, `Pol_bender_tapering`, `SNS_source_analytic`, `Source_multi_surfaces`, `Source_pulsed`, `SiC`, `multi_pipe` and several TOF monitors), by @willend in #3114
* `Monochromator_bent_complex` checks its parameters, by @willend in #3115
* `ESS_moderator`: compiles again with valid defaults for `sourcedef="2015"`, and moves to the obsolete components (`ESS_butterfly` replaces it), by @willend in #3113
* `Detector_pn` and `Pol_bender_tapering`: missing `NULL` terminator in `Table_ParseHeader` calls added, by @farhi in #3076

#### Examples
* `ESS_butterfly_MCPL_test`: the MCPL input is cached and only downloaded when missing, by @willend in #3053
* Outdated instrument README files regenerated, by @willend in #3064
* `Test_Sans_spheres` test updated, by @willend in #3083

### McXtrace only
* Lens fixes, by @farhi in #3049. *Photons that miss a lens aperture are now absorbed instead of passing through.*
  * `Lens_CRL_RTM`: exact absorption formula (factor 2 error in the fast mode), correct slow-mode amplitude and phase, `yheight` used as the full aperture height, photons outside the aperture absorbed, and a correct display box. New `Test_Lens_CRL_RTM` instrument
  * `Lens_parab`: photons outside the circular (previously square) aperture are absorbed, and intersections behind the photon are no longer picked
  * `Lens_parab_Cyl`: correct surface normal for refraction, and photons outside the aperture absorbed
  * `Lens_elliptical`: energy unit, material data columns, out-of-bounds table lookup, propagation direction and absorption path length fixed, and generic names renamed to avoid clashes with other components
  * `Lens_simple`: the default `radius` no longer overrides `xwidth`/`yheight`, and the phase term is fixed
  * `ZonePlate`: circular and linear modes no longer mix, the linear plate is displayed, and inputs are checked
  * All lenses handle back-propagating photons, and the `Test_CRL`, `Test_CRL_Be` and `MAXIV_FemtoMAX` test values are updated
* Fluorescence fixes, by @farhi. *Results change substantially.*
  * `Fluorescence`: cross-section normalisation, sampling, the `SPLIT` cache and file handling fixed (#3088), with test values updated (#3092)
  * `FluoPowder` and `FluoCrystal` follow the `Fluorescence` changes (#3094). `FluoPowder` restores the line selection and fixes the `d_phi` focusing and polarisation, `FluoCrystal` gets a polarisation factor and cross-sections in barns per unit cell, and `Fluo_detector` no longer depends on `Fluorescence` (#3105)
* `Bragg_crystal`: minor fixes, and test values updated for `MAXII_811` and `MAXIV_FemtoMAX`, by @farhi and @willend in #3126
* Undefined behaviour fixed in `perfect_crystals-lib` (uninitialised scale factors for `Mx_crystal_explicit`), `reflectivity-lib` and the `Single_crystal` warnings, by @willend in #3111
* `SAXSCurve` reads its curve file with `Open_File`, by @willend in #3110
* `PSD_monitor_4PI`: bin indices kept inside the arrays at the edges, by @willend in #3044

## New Contributors
* @ngeerits95 made their first contribution in #3107
* @lefmann made their first contribution in #2344
* @saqibkh made their first contribution in #3118

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.10.2...v3.10.3

