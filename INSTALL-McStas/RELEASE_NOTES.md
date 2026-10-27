Changes in McStas/McXtrace 3.10.0

*Covers everything since the previous "announced" release, 3.9.2.*

## Highlights

* **New Scipp/Plopp-based plotting and display tools (preview)**: `mcplot-plopp` and `mcdisplay-plopp` are early versions of plotting and 3D display built on [Scipp](https://scipp.github.io) and [Plopp](https://scipp.github.io/plopp). `mcplot-plopp` reads both McCode and NeXus output, including `Monitor_nD` event data shown as a pixel map. Both are offered as choices in `mcgui`/`mxgui`. By @willend.
* **NeXus output scales with MPI**: in MPI runs, event lists are now written to one HDF5 file per process and joined into a single view (HDF5 "virtual data sets"), instead of being funnelled through one process. NeXus files are now compressed by default, `--scan_split` scans give a single NeXus file, and the requested ray count is stored in the file. List-mode `Monitor_nD`s that also bin in wavelength or time no longer crash in NeXus mode, and Mantid-mode monitors binned only in x or y now load in Mantid. The NeXus writer is also more robust: text containing `%` is stored as given, very long monitor file names no longer crash the run or corrupt the file, and large event lists are no longer silently dropped. By @willend.
* **Better-quality random numbers in parallel runs**: with the default KISS generator, rays from different seeds, MPI processes and GPU batches could repeat each other's random sequences. Every ray now gets its own independent seed, and neutron IDs are unique across GPU batches and MPI processes. *Note: results for a given seed change compared to earlier versions.* Mersenne Twister runs are unaffected. By @willend (fixes #2807).
* **Much improved instrument display**: `mcdisplay-webgl-classic` now shows the detailed `--trace=2` component shapes by default, shaded and with a wireframe toggle. Hollow boxes and cylinders are drawn as the cavities they are in the physics, polyhedra with faces of any size display correctly, and `mcdisplay-cad` exports to STEP, PLY and plain OFF, including particle rays. By @willend and @farhi.
* **New `Text` component in McStas and McXtrace**: puts 3D text labels into your instrument drawings. With the new `Text2OFF` template instrument, any text can also be turned into OFF geometry for components that read OFF files. By @farhi and @willend.
* **More accurate conic mirrors**: the `conic.h` library behind the `ConicSurf`/`FlatEllipse` mirror family now applies the reflectivity once and with the correct scattering vector, no longer re-hits the surface it just reflected from, and has a configurable figure error (waviness model of Hansen et al.). New `Mirror_conic` test component and test instruments. By @willend (fixes #2787).
* **Statistically smarter testing with `mctest`**: new `%TestScan:` (aka `%Scan:`) lines run a test over a whole parameter scan. Tests can now pass on Monte Carlo error bars (`--nsigma`/`--pvalue`) instead of only a fixed ±20 % window, and `--statfactor` scales the statistics of every test. `%Example:` lines are now called `%Test:`. By @willend.
* **Union framework refactored**: the inner scattering loop of Union was reorganised for clarity and maintainability, and `Union_mesh` now uses a new general triangle-mesh library (`geometry-mesh-lib`) that reads STL, OFF and PLY files and ray-traces them efficiently. By Daniel Lomholt Christensen (@Lomholy), @mads-bertelsen-agentic and @willend.
* **More SOLEIL in McXtrace**: new PX1, NANOSCOPIUM and GALAXIES beamlines, plus a model of the full SOLEIL synchrotron assembling the beamlines, mainly for rendering. By @farhi.
* **`MCPL_input` consolidated**: `MCPL_input` now includes the MPI splitting of `MCPL_input_once` yet allows to repeat by `repeat_count`. `MCPL_input_once` is moved to the obsolete components. `MCPL_output` now uses `weight_mode=1` by default, which records the initial ray count and rescales weights, adapting with the [`stat:sum` convention](https://mctools.github.io/mcpl/format_statsum/) from MCPL. By @willend.

## What's Changed

### Common to McStas and McXtrace

#### Visualisation and plotting
* New `mcplot-plopp` and `mcdisplay-plopp` prototypes based on Scipp and Plopp by @willend in #2799, with event pixel maps (#2870), NeXus attribute fixes (#2878) and `mcgui` menu entries (#2879)
* `mcdisplay-webgl-classic`: `--trace=2` shapes by default with the new `Test_mcdisplay_shapes` example (#2836, #2840), shaded shapes with wireframe toggle (#2848), dead code removed and `-n 0`/`--invcanvas` fixed (#2833), all by @willend
* `mcdisplay-webgl`: polyhedron faces of any size (#2847), legacy code removed and the viewer rebuilt (#2832), by @willend
* `mcdisplay`: hollow boxes and cylinders drawn as closed cavities (#2844, documented with tilted test cases in #2845), simple `--trace=1` box/cylinder/cone drawings now match the detailed ones (#2837, #2802), correct polygon winding in `--trace=2` output (#2843), instrument parameters given before `-n` are kept (#2854, fixes #747), closing the pyqtgraph window no longer raises an error (#2800), by @willend
* `mcdisplay-cad`: much faster and more complete, with STEP output, particle rays, `--tubes` and `-t/--trace` (#2824, by @farhi and @willend), plain OFF output and PLY files readable by the OFF/PLY library (#2868), `Annulus` drawn with the correct ring width (#2846)
* Python tools now quote file paths when opening them in a browser, by @willend in #2836
* The text-based `mcplot-terminal` was added and then deferred to a later release, by @willend in #2785, #2822

#### Running simulations (`mcrun`/`mxrun`, `mcgui`/`mxgui`)
* `mcrun`: `--scan_split` scans give one NeXus file, laid out as for a serial NeXus scan, by @willend in #2804, #2891
* `mcrun`: `--verbose` now also applies to compilation, by @farhi in #2821
* `mcrun`: setting `--autoplotter` now opens the plot automatically, by @willend in #2825
* `mcgui`/`mxgui`: the run dialogue scrolls for instruments with many parameters and fits the screen, by @farhi and @willend in #2829. Unused interface files removed in #2831

#### Simulation output (NeXus) and random numbers
* MPI NeXus event lists written per process and combined with HDF5 virtual data sets, compression on by default, requested ray count stored, safe saving on `SIGTERM`, by @willend in #2830
* `Monitor_nD`: the NeXus `BINS`/pixel map is now sized over the spatial axes only, so list-mode monitors that also bin in e.g. wavelength or time no longer run out of memory and crash, with the new `Test_Monitor_nD_BINS` regression test, by @willend in #2882 (fixes #2484)
* `Monitor_nD`: NeXus `BINS`/pixel map ordered like the event pixel ID (#2877), and `intensity`/`flux` options no longer add an extra variable (#2856, fixes #2445), by @willend
* NeXus runtime parameter values also accept the type names used by `mccode-antlr`, by @willend in #2835
* More robust NeXus writer: string values, titles and parameter literals containing `%` are stored as given (previously garbled), `Monitor_nD` `BINS` output no longer contains uninitialised values, monitor file names too long for the NeXus library are skipped with a warning instead of crashing the run and corrupting `mccode.h5`, chunks are kept below the 4 GiB HDF5 limit so large event lists are written, and several smaller file-handling fixes. New `Test_NeXus_output` regression test in McStas and McXtrace, run in CI with and without MPI. By @willend in #2889
* Decorrelated per-ray random seeds and unique neutron IDs across MPI processes and GPU batches, by @willend in #2808 (fixes #2807). Test references updated accordingly.

#### Code generator & grammar
* A round of code-generator fixes: expressions in `{...}` vector parameters, `INHERIT` sections (a section written in the child, even empty, replaces the parent's), `USERVARS` handling as in `mccode-antlr`, with new grammar unit tests, by @willend in #2767
* `JUMP` to an undefined component now gives a clear error instead of a crash, by @farhi in #2819. Forward `JUMP`s are allowed, by @willend in #2823
* Obsolete `DETECTOR_OUT_3D` macro removed from components and docs, by @willend in #2859 (fixes #1573)
* Fixes for intermittent results from header parsing in data files and other undefined behaviour, by @willend in #2770
* Data file cache (`read_table-lib`) kept consistent, and errors in MPI runs abort cleanly, by @willend in #2784

#### Testing (`mctest`)
* New `%TestScan:` (aka `%Scan:`) test lines, used in many example instruments, by @willend in #2772
* Statistical acceptance (`--nsigma`, `--pvalue`) and `--statfactor`, with a new documentation page. `%Example:` lines renamed `%Test:` (old name still accepted), by @willend in #2776
* `-n` on `%Test:`/`%Example:` lines is now honoured, as for `%TestScan:`/`%Scan:`, by @willend in #2820, #2841

#### Documentation
* `mcdoc` renders TeX with a locally installed MathJax (#2871) and writes search results to a separate page (#2855), by @willend
* McStas manual: the Installing chapter is back and points to the current install instructions, by @willend in #2873
* Clearer external-contributions guide, by @willend in #2860 (fixes #2771). Component example-coverage lists regenerated (#2861)

#### GPU / OpenACC
* OpenACC fixes for instruments, the SOLEIL models and test components, by @willend in #2797, #2881. `Dispersion_relation` GPU race fixed in #2769, `Radial_div` monitor accumulation made GPU-safe in #2890

#### Installation, platforms and packaging
* `cif2hkl` is now built from the upstream SOLEIL release and required at version 1.5.0 or newer (now 1.5.1) for conda and Debian packages, by @willend in #2864, #2873
* macOS: workaround for an OpenMPI issue caused by a long `$TMPDIR`, by @willend in #2775
* Debian install instructions use the new `mccode.sources` file, by @willend in #2873
* For contributors: new `devel/bin` scripts to get dependencies and build both McStas and McXtrace (#2773), conda environment update (#2795), and many CI improvements: logs of failed tests as artefacts, opt-in result comparisons, `mccode-antlr`/`mcxtrace-antlr` testing from the current checkout (#2826, #2827, #2849, #2850, #2852, #2875). Stray simulation output removed from the repository (#2872)

### McStas only

#### Components
* `Monitor_nD` in Mantid mode: square monitors binned only in x or y now get the event columns Mantid expects (previously Mantid silently loaded no events), `y ... x` no longer gives a rotated detector, and `mcdisplay-mantid` places pixels at their centres instead of half a pixel off, by @willend in #2892 (fixes #2178)
* `MCPL_input` consolidated with `MCPL_input_once` (MPI splitting, `repeat_count`). `MCPL_input_once` moved to obsolete. By @willend in #2780. `MCPL_output` defaults to `weight_mode=1`, following the MCPL [`stat:sum` convention](https://mctools.github.io/mcpl/format_statsum/) (#2779)
* Conic mirrors (`conic.h`): reflectivity applied once with the correct scattering vector (#2789), configurable figure error with a waviness model, `StdReflecFunc` reflectivity only, `FlatEllipse_finite_mirror` `sourceDist` fix (#2793), no spurious re-hit of the surface just reflected from (#2794). New `Mirror_conic` test component. By @willend, fixes #2787
* New `Text` component for 3D text labels, used to label the ILL H5 instruments, by @farhi and @willend in #2782. New `Text2OFF` template instrument (#2869)
* `NCrystal_sample` draws its shape from the component parameters, by @tkittel in #2777
* `FermiChopper`: warns when phase and delay are both zero (#2818) and documents its length default and that rays missing the cylinder pass unchanged (#2816), by @henrikjacobsenfys
* ESS 2015 sources now pass the moderator height to the pulse-shape functions (#2814), and `ESS_IN5_reprate` phases its counter-rotating disk correctly (#2812), by @henrikjacobsenfys
* `Res_monitor` and `TOFRes_monitor` no longer free arrays they did not create, by @henrikjacobsenfys in #2810
* `Radial_div`: obsolete grammar removed, new `Test_Radial_div`, by @willend in #2857 (fixes #2678)
* `Annulus`: clearer parameters, by @farhi in #2801
* `Tomography` template: the `tomo_recon.py` reconstruction script now shows a 3D isosurface, reads and writes NeXus and offers SIRT and filter options, with a full-rotation `%TestScan:` test, by @willend in #2772, #2803
* `mcstas-chopper-lib` updated to v4.3.0, v4.4.0 and v4.4.1: transmission weighted by how likely a neutron gets through, the depth of moderator emission surfaces, parked discs open while any of the beam crosses an opening, and more. By @g5t, registered in #2805, #2842, #2867

#### Union
* Refactored scattering loop, by Daniel Lomholt Christensen (@Lomholy), @mads-bertelsen-agentic and @willend in #2571
* `Union_mesh` now built on the new `geometry-mesh-lib`: STL (ASCII/binary), OFF and PLY files, polygons of any size, duplicate vertices and faces cleaned up, and a bounding volume hierarchy for fast ray tracing of large meshes. Each triangle gets its own surface entry. By Daniel Lomholt Christensen (@Lomholy) and @willend in #2866

#### Python instruments (`mcstas-pygen`)
* Generated scripts no longer contain the McCode system directory, by @tkittel in #2774

### McXtrace only
* New instruments by @farhi: `SOLEIL_PX1` (#2740), `SOLEIL_NANOSCOPIUM` (#2742), `SOLEIL_GALAXIES` (#2689), and a full `SOLEIL` synchrotron model assembling the beamlines (#2783, #2865)
* New components ported from McStas by @farhi: `Text` (#2781), and `Annulus`, `Circle`, `Cone`, `Disc` and `Legacy_circle` with a new `Demo_shape_primitives` template (#2801). New `Text2OFF` template instrument (#2869)
* `Mirror_toroid` and `Mirror_toroid_pothole`: ellipsoid semi-axis and `xmax` fixed, by @antoinepado in #2838
* `Shape` component fixed, by @farhi in #2828. `Annulus` uses radius/thickness as in McStas (#2837)
* Single-crystal samples are ignored when their geometry has no volume or the reflection list is invalid, and test values updated for `cif2hkl` structure factors now given in barns, by @farhi in #2839, #2863
* Component fixes for `mcxtrace-antlr` compatibility, by @willend in #2851

## New Contributors
* @henrikjacobsenfys made their first contribution in #2810

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.9.2...v3.10.0
