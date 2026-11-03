Changes in McStas/McXtrace 3.10.2

*Covers all changes since 3.10.0, including those already released in 3.10.1.*

## Highlights

* **NeXus output on native HDF5**: McCode now writes NeXus files directly with HDF5, replacing the frozen NeXus API library (napi/libNeXus), which is no longer a dependency. NeXus support only needs HDF5. NeXus output can now also be plotted with `mcplot`, `mcplotdiff` and `mccoplot`, and MPI NeXus scans keep every step's events. By @willend.
* **Planar monitors restore the ray on a miss**: about 40 simple monitors in McStas and McXtrace (`PSD_monitor`, `L_monitor`, `E_monitor`, `TOF_monitor`, ...) now leave rays that miss them unchanged, as `Monitor_nD` already did. Previously such rays could be lost. In McXtrace, `ALLOW_BACKPROP` is now honoured as in McStas, so some rays that used to be absorbed now survive. *Note: results can change slightly.* By @willend (fixes #2906).
* **Corrected sample physics**: `Phonon_simple` and the Union `PhononSimple_process` now scale the one-phonon cross-section as in Squires (eq. 3.120), giving about 12.7 times higher intensity than before. The Union `IncoherentPhonon_process` gets the correct ray weight normalisation and its three-phonon total cross-section, `Multilayer_Sample` substrate incoherent scattering now works, and `Single_crystal_inelastic`, `SANS_benchmark2` and `StatisticalChopper` are also corrected. *Note: results for these components change, some substantially.* By @henrikjacobsenfys, Daniel Lomholt Christensen (@Lomholy), @mads-bertelsen-agentic and @willend.
* **McXtrace sources and optics corrected, SOLEIL models realigned**: `Bending_magnet` now gives the correct flux and samples correctly, `Mirror_curved` reflects as intended, `Single_crystal` absorption is fixed, and `Source_lab` uses xraylib for any single-element anode, with no data file needed. Many SOLEIL beamline models were repaired: mirror radii and angles, the Si lattice constant (5.4309 Å, not 5.4909 Å) and DCM alignment, so the beams now reach the samples as designed. *Note: results for these components and instruments change, some substantially.* By @antoinepado, @farhi and @willend.
* **Component review: GPU, memory and robustness fixes**: a systematic review of the component libraries fixed GPU race conditions, out-of-bounds table and root indices, memory leaks, uninitialised values and name clashes between components used in the same instrument. The 16 standalone MCViNE components and most SANS/SAXS contributed samples now run on GPUs (OpenACC). By @willend.
* **Solid, coloured instrument drawings**: under `--trace=2`, guides, mirrors, monochromators, beamstops, collimators, choppers and many other components are now drawn as filled surfaces, and components can switch pen colour with `mcdis_pencolor("#rrggbb")`. Union volumes can be shown as polyhedra, large `Union_mesh` geometries display much faster, and `ESS_butterfly` draws colour-coded solid moderators. `mcdisplay-webgl(-classic)` gains Surfaces/Lines toggles and a surface opacity slider, and overlapping transparent surfaces render correctly. By @willend, Daniel Lomholt Christensen (@Lomholy) and @mads-bertelsen-agentic.
* **New home for packages and downloads**: the Debian/Ubuntu packages are now served from the McCode apt repository at [mccode-dev.github.io/debian](https://mccode-dev.github.io/debian/), replacing packages.mccode.org, and the macOS and Windows bundles are attached to the [GitHub releases](https://github.com/mccode-dev/McCode/releases). See the updated install instructions. By @willend.
* **`%Test:` and `%TestScan:` instrument headers**: all example instruments now use the new names for their test lines (`%Example:` and `%Scan:` are still accepted). `mctest` compiles in parallel with `-j N|auto`, compiles only with `-n0`, and reports compilation and simulation time separately. By @willend.

## What's Changed

### Common to McStas and McXtrace

#### Components
* Planar monitors restore the ray on a miss, like `Monitor_nD`, and keep honouring `ALLOW_BACKPROP`. Test references updated accordingly. By @willend in #2907 (fixes #2906)
* Simple components draw `--trace=2` primitives (boxes, cylinders, spheres, polygons) with pen colours: `Beamstop` as a filled disc or rectangle, collimators with dark blades, `DiskChopper` with its slit openings, `Monitor_nD` banana and sphere-section plates, and more, by @willend in #2943

#### Visualisation and plotting
* `mcplot`, `mcplotdiff` and `mccoplot` plot NeXus (`mccode.h5`) output, and `mcplot-html` has per-step monitor pages for scans, also on a log scale, by @willend in #2897
* `mcplot-plopp`: `ui()` returns the data as a scipp DataGroup, by @willend in #2900
* New `mcdis_pencolor("#rrggbb")` to switch the pen colour within `MCDISPLAY`, supported in the pyqtgraph, plopp, webgl, CAD and Matlab displays, by @willend in #2919
* `mcdisplay-webgl(-classic)`: Surfaces/Lines toggles for `--trace=2` shapes (#2915), lines restored for translucent shapes (#2926), grids centred on the instrument with 1 m axis arrows (#2948), compact instrument label and surface opacity slider (#2944), by @willend. Overlapping transparent surfaces fixed by @mads-bertelsen-agentic in #2921
* `mcdisplay-matplotlib`: equal-scale box fitted to the instrument, by @willend in #2949
* `mcdisplay` backends only accept the `--trace` levels they support, by @willend in #2935
* `mcgui`/`mxgui`: "New From Template" asks for the parent folder and handles existing copies, the McStas/McXtrace logo is the application icon, and crashes found by monkey-testing `mcgui`, `mcplot` and `mcdisplay` are fixed, by @willend in #2942

#### Simulation output (NeXus)
* NeXus output written directly with HDF5, replacing the frozen napi (libNeXus), which is dropped from dependencies, toolchains and docs. Includes a NeXus output regression test. By @willend in #2895, with a design note on code-generator link hints in #2916
* MPI NeXus event lists keep each scan step's events, by @willend in #2901

#### Runtime and libraries
* `randminmax` now samples in [min, max) (previously [max, 2·max−min); no library component used it). The never-compiling `mirror()` macro and a dead complex-number fallback were removed. By @willend in #2924
* Runtime libraries: missing return values, an uninitialised index and a wrong shift when popping the magnetic-field stack fixed (`interpolation-lib`, `pol-lib`, `supermirror-lib`), by @willend in #3001
* Generic global names prefixed to avoid clashes between components in one instrument: `conic.h` (`sign` → `conic_sign`) and the SasView components (`square`/`cube` → `sas_square`/`sas_cube`) in #3010, by @willend
* OpenACC: the OFF/PLY and mesh libraries no longer leak an `fprintf` redefinition into later code, by @willend in #2960

#### Testing (`mctest`)
* All example instrument headers use `%Test:` and `%TestScan:` (formerly `%Example:` and `%Scan:`, still accepted), by @willend in #2945
* `mctest -j N|auto` compiles instruments in parallel, and `-n0` means compile only, by @willend in #2918
* `mctest` reports total runtime, and compilation and simulation time separately (#2950, #2961). `mcviewtest` shows test runtimes in the column headers (#2957), by @willend
* `mctest`/`mcviewtest`: plots, diffs and coplots of NeXus output (#2897). `mcviewtest` labels NeXus and ASCII output and skips non-test folders (#2903, #2904, #2905), by @willend

#### Installation, platforms and packaging
* Debian/Ubuntu packages are served from the McCode apt repository on [mccode-dev.github.io/debian](https://mccode-dev.github.io/debian/) (replacing packages.mccode.org), and the per-platform bundles are attached to each GitHub release. The install docs point there and are moved to each new release automatically. By @willend in #3012
* `h5py` is now an explicit conda dependency, needed for NeXus `--scan_split` and NeXus plotting, by @willend in #2894
* FreeBSD: builds from source, with a new CI track for McStas and McXtrace, by @willend in #2914, #2932
* New security policy: please report vulnerabilities privately, as described in `SECURITY.md`, by @willend in #3008
* For contributors: compile-only OpenACC (NVIDIA nvhpc) CI leg (#2918, #2951), basic tests only simulate instruments when relevant code changed (#2946), and the mccode.org-style Debian CI leg was paused and then moved to the new apt repository (#3011, #3012), by @willend

### McStas only

#### Samples: physics corrections
* `Phonon_simple` and Union `PhononSimple_process`: one-phonon cross-section scaled as in Squires eq. 3.120, about 12.7 times higher intensity than before, by @henrikjacobsenfys, Daniel Lomholt Christensen (@Lomholy) and @willend in #2806
* Union `IncoherentPhonon_process`: ray weight normalisation fixed (#2934, by @mads-bertelsen-agentic), three-phonon term in the total cross-section and uninitialised Q² bounds fixed, debug files only on request (#2974, by @willend)
* `Multilayer_Sample`: substrate incoherent scattering now works and is physical, with absorption treated separately (#2991), GSL scratch space allocated once (#2983), by @willend
* `Single_crystal_inelastic`: unbiased weights with standard inverse-CDF sampling, per-neutron debug file removed, leaks and out-of-bounds indices fixed, by @willend in #2986
* `SANS_benchmark2`: scattering-branch weights normalised by the total cross-section, so weight is conserved (#2990), Bragg-peak Q index clamped in model 11 (#2981), by @willend
* `StatisticalChopper`: slit timing fixed (integer division, pulse offset, random-number bound), by @willend in #2993

#### Component review: GPU, memory and robustness fixes
* Samples: `PowderN` and Union `Powder_process` (line index clamped, cache-bin precedence; #2962, #2963), `Single_crystal` and Union `Single_crystal_process` (initialised mosaic coefficients; #2977, #2978), `Phonon_simple`, `Magnon_bcc`, `SpinWave_BCO`, `Dispersion_relation` and Union `PhononSimple_process` (bounded root lists and indices, no per-neutron allocation, no name clash between `Magnon_bcc` and `Phonon_simple`; #2964–#2967, #2969), `Isotropic_Sqw` (GPU race; #2980), `Single_magnetic_crystal` message (#2979), `Res_sample`/`TOFRes_sample` output (#2975, #2976), by @willend
* SANS samples: `SANSSpheres`, `SANSSpheresPolydisperse`, `SANSShells` and `SANSCurve` no longer modify their parameters per neutron or race on GPUs, `SANSCurve` no longer clashes with `<complex.h>`, `SANSPDB`/`SANSPDBFast` and `SANS_spheres2` without leaks and dead code (#2970–#2973, #2982, #2984, #2985). `SANSNanodiscs*`, `SANSCylinders`, `SANSEllipticCylinders` and `SANSPDBFast` run on GPUs and no longer need GSL (#2992), by @willend
* Optics: gravity guides (`Guide_gravity`, `Guide_gravity_psd`, `Guide_honeycomb`, `Pol_bender_tapering`; #2994), transmission polarisers and `Foil_flipper_magnet` (#2996), `PerfectCrystal` and `SupermirrorFlat` (#2997), `Monochromator_bent(_complex)` build clashes (#2999), `Pol_tabled_field` (#3001) and other optics and polarisation components (#2998) fixed for GPU races, shared state and build errors, by @willend
* The 16 standalone `MCViNE_*` sample components run on GPUs (OpenACC), by @willend in #3000

#### Display
* Filled `--trace=2` surfaces for guides and benders (#2952, #2953), contributed mirrors (#2956) and monochromators (#2958), by @willend
* `ESS_butterfly`: colour-coded drawing with solid moderators, proton-beam arrow and ESS emblem (#2947), also drawn in OpenACC builds (#2959), by @willend
* Union volumes can be displayed as polyhedra under `--trace=2`, with triangles cut exactly at visibility boundaries, and a `display_mode` parameter on `Union_master` to force lines, by @willend in #2927
* `Union_mesh` display: much faster for large meshes (fixed quadratic cost and leaks), with the option to draw a coarse bounding-box outline, by Daniel Lomholt Christensen (@Lomholy) and @willend in #2893

#### Other components and examples
* `cov-lib`: parameter `I` renamed `I_cov`, so it no longer clashes with `<complex.h>`, by @willend in #2923
* `%Test`/`%TestScan` tests added to 12 previously untested example instruments (#2908). The RITA-II Pb(200) `%TestScan` is disabled for now (#2933), by @willend

### McXtrace only
* `Bending_magnet`: correct flux and sampling, `E0`/`dE` energy options with the `lambda` mode kept, and more tests, by @antoinepado, @farhi and @willend in #2902
* `Mirror_curved`: reflection fixed by @antoinepado in #2936, and rays it misses keep being absorbed when an instrument's `EXTEND` asks for it, by @willend in #2941
* `Single_crystal`: absorption fixed (wrong multiplication factor) by @antoinepado in #2880, and now computed directly from mu/rho times the mass density, by @willend in #2937, #2938
* `Source_lab` now uses xraylib for all material data (any single-element anode, no data file needed; the old data-file mode is still available), and the fraction of Bremsstrahlung is fixed, by @farhi in #2925
* SOLEIL and other example instruments repaired: mirror grazing angles given in rad where degrees were needed, mirror radii and takeoff directions, the Si lattice constant (5.4309 Å, not 5.4909 Å), DCMs set at the refraction-corrected Bragg angle, and updated test targets, by @willend in #2941
* `Undulator` minor fixes (#2913) and a `Wiggler` sanity check (#2917), by @farhi
* `Mirror_toroid`: corrected call to the reflectivity function, by @antoinepado in #2930
* Filled `--trace=2` surfaces for mirrors and `Capillary`, by @willend in #2955
* Union: mesh geometry, focusing and display fixes ported from McStas, including the polyhedron display, by @willend in #2929
* SAXS samples: GPU race and memory-leak fixes ported from the McStas SANS components, and OpenACC enabled for the Nanodiscs, Cylinders, EllipticCylinders and PDBFast variants, by @willend in #2995
* Runtime fixes: `reflecq` returns R0 for constant reflectivity, form-factor table searches stay within bounds, and `PROP_DT` now works for x-rays, by @willend in #2922
* `ALLOW_BACKPROP` is honoured by `PROP_DL`, as in McStas, by @willend in #2907
* AstroX optics: valid nearby intersections are no longer rejected as self-intersections, by @willend in #2912
* `Air`: non-scattered rays are propagated to the exit surface, by @willend in #2910
* Rays propagated to a plane land exactly on it, and `Monitor_nD` checks forward propagation by time, by @willend in #2911
* `%Test` tests added to the AstroX examples `Test_shells`, `ATHENA_1pore` and `ATHENA_1ring`, by @willend in #2909

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.10.0...v3.10.2

