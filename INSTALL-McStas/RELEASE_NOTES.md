### Changes in McStas/McXtrace 3.10.1

*Covers everything merged to main since the previous "announced" release, 3.10.0.*

## Highlights

* **NeXus output on native HDF5**: McCode now writes NeXus files directly with HDF5, replacing the frozen NeXus API library (napi/libNeXus), which is no longer a dependency. NeXus support only needs HDF5. By @willend.
* **Planar monitors restore the ray on a miss**: about 40 simple monitors in McStas and McXtrace (`PSD_monitor`, `L_monitor`, `E_monitor`, `TOF_monitor`, ...) now leave rays that miss them unchanged, as `Monitor_nD` already did. Previously such rays could be lost. In McXtrace, `ALLOW_BACKPROP` is now honoured as in McStas, so some rays that used to be absorbed now survive. *Note: results can change slightly.* By @willend (fixes #2906).
* **NeXus output in the Python plotters**: `mcplot`, `mcplotdiff` and `mccoplot` now plot NeXus (`mccode.h5`) output, and `mcplot-html` gets per-step pages for scans. MPI NeXus scans now keep every step's events. By @willend.
* **Better Union display**: Union volumes can be drawn as solid polyhedra under `--trace=2`, cut exactly where one volume hides another, and large `Union_mesh` geometries display much faster, with an option to draw a coarse outline. McXtrace Union gains mesh geometry, focusing and display fixes ported from McStas. By Daniel Lomholt Christensen (@Lomholy) and @willend.
* **More flexible instrument display**: components can switch pen colour inside `MCDISPLAY` with `mcdis_pencolor("#rrggbb")`, `mcdisplay-webgl` and `mcdisplay-webgl-classic` get Surfaces/Lines toggles for `--trace=2` shapes, and overlapping transparent surfaces now render correctly. By @willend and @mads-bertelsen-agentic.
* **McXtrace sources and optics corrected, SOLEIL models realigned**: `Bending_magnet` now gives the correct flux and samples correctly, `Mirror_curved` reflects as intended (rays it misses are no longer let through when an instrument absorbs them), and `Single_crystal` absorption is fixed. Many SOLEIL beamline models were repaired: mirror radii and angles, the Si lattice constant (5.4309 Å, not 5.4909 Å) and DCM alignment, so the beams now reach the samples as designed. *Note: results for these components and instruments change, some substantially.* By @antoinepado, @farhi and @willend.
* **`Source_lab` (McXtrace) now uses xraylib**: characteristic lines, widths, intensities and anode absorption come from xraylib for any single-element anode, with no data file needed, and the Bremsstrahlung fraction is fixed. By @farhi.

## What's Changed

### Common to McStas and McXtrace

#### Components
* Planar monitors restore the ray on a miss, like `Monitor_nD`, and keep honouring `ALLOW_BACKPROP`. Test references updated accordingly. By @willend in #2907 (fixes #2906)

#### Visualisation and plotting
* `mcplot`, `mcplotdiff` and `mccoplot` plot NeXus (`mccode.h5`) output, and `mcplot-html` has per-step monitor pages for scans, also on a log scale, by @willend in #2897
* `mcplot-plopp`: `ui()` returns the data as a scipp DataGroup, by @willend in #2900
* New `mcdis_pencolor("#rrggbb")` to switch the pen colour within `MCDISPLAY`, supported in the pyqtgraph, plopp, webgl, CAD and Matlab displays, by @willend in #2919
* `mcdisplay-webgl(-classic)`: Surfaces/Lines toggles for `--trace=2` shapes (#2915), lines restored for translucent shapes (#2926), by @willend. Overlapping transparent surfaces fixed by @mads-bertelsen-agentic in #2921
* `mcdisplay` backends only accept the `--trace` levels they support, by @willend in #2935

#### Simulation output (NeXus)
* NeXus output written directly with HDF5, replacing the frozen napi (libNeXus), which is dropped from dependencies, toolchains and docs. Includes a NeXus output regression test. By @willend in #2895, with a design note on code-generator link hints in #2916
* MPI NeXus event lists keep each scan step's events, by @willend in #2901

#### Runtime
* `randminmax` now samples in [min, max) (previously [max, 2·max−min); no library component used it). The never-compiling `mirror()` macro and a dead complex-number fallback were removed. By @willend in #2924

#### Testing (`mctest`)
* `mctest -j N|auto` compiles instruments in parallel, and `-n0` means compile only, by @willend in #2918
* `mctest`/`mcviewtest`: plots, diffs and coplots of NeXus output (#2897). `mcviewtest` labels NeXus and ASCII output and skips non-test folders (#2903, #2904, #2905), by @willend

#### Installation, platforms and packaging
* `h5py` is now an explicit conda dependency, needed for NeXus `--scan_split` and NeXus plotting, by @willend in #2894
* FreeBSD: builds from source, with a new CI track for McStas and McXtrace, by @willend in #2914, #2932
* For contributors: compile-only OpenACC (NVIDIA nvhpc) CI leg, by @willend in #2918

### McStas only

#### Union
* Union volumes can be displayed as polyhedra under `--trace=2`, with triangles cut exactly at visibility boundaries, and a `display_mode` parameter on `Union_master` to force lines, by @willend in #2927
* `Union_mesh` display: much faster for large meshes (fixed quadratic cost and leaks), with the option to draw a coarse bounding-box outline, by Daniel Lomholt Christensen (@Lomholy) and @willend in #2893
* `IncoherentPhonon_process`: ray weight normalisation fixed, by @mads-bertelsen-agentic in #2934

#### Components and examples
* `cov-lib`: parameter `I` renamed `I_cov`, so it no longer clashes with `<complex.h>`, by @willend in #2923
* `%Example`/`%Scan` tests added to 12 previously untested example instruments (#2908). The RITA-II Pb(200) `%Scan` is disabled for now (#2933), by @willend

### McXtrace only
* Union: mesh geometry, focusing and display fixes ported from McStas, including the polyhedron display, by @willend in #2929
* `Source_lab` now uses xraylib for all material data (any single-element anode, no data file needed; the old data-file mode is still available), and the fraction of Bremsstrahlung is fixed, by @farhi in #2925
* `Bending_magnet`: correct flux and sampling, `E0`/`dE` energy options with the `lambda` mode kept, and more tests, by @antoinepado, @farhi and @willend in #2902
* `Mirror_curved`: reflection fixed by @antoinepado in #2936, and rays it misses keep being absorbed when an instrument's `EXTEND` asks for it, by @willend in #2941
* `Single_crystal`: absorption fixed (wrong multiplication factor) by @antoinepado in #2880, and now computed directly from mu/rho times the mass density, by @willend in #2937, #2938
* SOLEIL and other example instruments repaired: mirror grazing angles given in rad where degrees were needed, mirror radii and takeoff directions, the Si lattice constant (5.4309 Å, not 5.4909 Å), DCMs set at the refraction-corrected Bragg angle, and updated test targets, by @willend in #2941
* `Undulator` minor fixes (#2913) and a `Wiggler` sanity check (#2917), by @farhi
* `Mirror_toroid`: corrected call to the reflectivity function, by @antoinepado in #2930
* Runtime fixes: `reflecq` returns R0 for constant reflectivity, form-factor table searches stay within bounds, and `PROP_DT` now works for x-rays, by @willend in #2922
* `ALLOW_BACKPROP` is honoured by `PROP_DL`, as in McStas, by @willend in #2907
* AstroX optics: valid nearby intersections are no longer rejected as self-intersections, by @willend in #2912
* `Air`: non-scattered rays are propagated to the exit surface, by @willend in #2910
* Rays propagated to a plane land exactly on it, and `Monitor_nD` checks forward propagation by time, by @willend in #2911
* `%Example` tests added to the AstroX examples `Test_shells`, `ATHENA_1pore` and `ATHENA_1ring`, by @willend in #2909

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.10.0...v3.10.1
