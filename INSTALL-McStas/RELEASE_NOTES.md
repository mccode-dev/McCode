Changes in McStas/McXtrace 3.10.4

## Highlights

* **Instrument display with tours and monitor data**: `mcdisplay-webgl` and `mcdisplay-webgl-classic` get a tour mode that flies along the beam for making movies, with a speed slider, pause, recording, a start component and a `--campath` file. With `--simdir` (and `--simbins`) the monitor data of a simulation is drawn on the monitors and plotted next to the display, and the new `mcrun`/`mcgui` option `--autodisplay` shows it right after a run. Drawing is faster for large instruments, and `mcdisplay-cad` also exports monitor data. By @willend.
* **McXtrace mirrors and gratings corrected**: `Mirror_curved` is rewritten and now supports double curvature, and `Mirror_elliptic`, `Mirror_parabolic`, `Grating_reflect` and `Grating_trans` get major physics fixes. `Filter` refracts for all shapes, including OFF files. Instruments with KB mirrors were repaired. *Note: results for these components and instruments change, some substantially.* By @farhi and @willend.
* **Powder and crystal data cleaned up**: the bundled `.lau` powder files are regenerated with NCrystal, broken `.laz` files are removed, and `PowderN`, `Powder_process`, `Single_crystal`, `Isotropic_Sqw` and `Texture_process` read the |F|² unit (barn or fm²) from the file header. *Note: results can change for powder and crystal samples using the bundled data.* By @willend (fixes #2112).
* **Porous samples**: the new `PowderN_porous` component (McStas and McXtrace) describes a powder matrix with mesh pores, partly filled with a liquid. The `PSI_ICON` and `ILL_NeXT_Xray` imaging instruments are split into shared beamline and detector parts, with new porous-sample variants. By @willend.
* **New `mcmerge`/`mxmerge` tool**: merges simulation datasets, including MCPL files via `mcpltool`. By @willend.
* **Mean values written correctly**: polarisation monitors and `Monitor_nD` with `signal=` now output mean values through a central `DETECTOR_OUT_*_MEAN` routine, correct also with MPI and on intermediate saves. Data headers state `mean: yes` or `no`. *Note: the totals of these monitors change.* By @willend.
* **Instrument-level `METADATA`**: instruments can carry `METADATA` blocks, as in `mccode-antlr`, also written by `mcstas-pygen`. By @g5t and @willend.

## What's Changed

### Common to McStas and McXtrace

#### Visualisation and GUI
* `mcdisplay-webgl(-classic)` tour mode for movies: introduced in #3133 and #3150, smooth tour with a speed setting (#3152), recording and start from a component (#3148), `--campath` file with a SOLEIL example (#3154), speed slider and pause button (#3164, #3179), by @willend
* `mcdisplay-webgl(-classic)`: `--simdir` shows monitor data on the monitors and adds it to an existing display (#3155), `--simbins` sets its binning (#3160), monitor plots next to the display (#3197), flat 1D monitor curves along the instrument's x axis (#3199) and monitor data in the plane of the drawing (#3202), by @willend
* `mcdisplay-webgl(-classic)`: merged geometry for faster drawing (#3187), and double-click turns the view gradually (#3188), by @willend
* `mcdisplay` keeps up to 1e5 rays and caps `-n` at 1000 (#3155), and `-y` is forwarded to `mcrun` (#3177), by @willend
* `mcdisplay-cad`: `--simdir` monitor data and transparency, and fixed VTK output (#3182), faster GLB/glTF/OFF export of large models (#3184), by @willend
* `mcdisplay-mantid`: `--embed` writes the IDF into an existing NeXus file (#3178), and an error without `sourceMantid`/`sampleMantid` (#3177), by @willend
* Code generator: each `SPLIT` branch is traced as its own ray, so the display no longer draws a line back to the `SPLIT` point, by @willend in #3159
* `Arm`: its arrows can be left out of the display with `DROP_ARM_ARROWS`, by @willend in #3138

#### Running simulations (`mcrun`/`mxrun`, `mcgui`/`mxgui`)
* New `--autodisplay` option in `mcrun` and `mcgui` to show the display with monitor data after a run (#3202), with clearer `--autodisplayer` help (#3203), by @willend
* `mcrun` starts MPI with `-n`, so `srun` can be used as the launcher, by @willend in #3204
* `mcrun --no-cflags` no longer crashes, by @willend in #3135

#### Simulation output and runtime
* Central `DETECTOR_OUT_*_MEAN` output of mean values for polarisation monitors and `Monitor_nD` `signal=`, correct with MPI and on intermediate saves, with `mean: yes/no` in all data headers. `Test_Magnetic_Rotation` gets a new target. By @willend in #3193
* Instrument-level `METADATA` accepted, as in `mccode-antlr`, with table accessors, a `Test_lib_metadata` test and `mcstas-pygen` support, by @g5t and @willend in #3180
* Code generator: the case-insensitive component search closes its directory, by @willend in #3136

#### Tools
* New `mcmerge`/`mxmerge` to merge datasets, including MCPL files with `mcpltool`. `Monitor_nD` event lists now write their number of events in the data values. By @willend in #3196

#### Testing (`mctest`)
* `mctest --displayncount` sets the number of rays for the display runs, by @willend in #3162

#### Installation, platforms and packaging
* Debian/Ubuntu suite metapackages depend on the correct `*-clustertools` package name, by @willend in #3132
* `mcstas-chopper-lib` updated to v4.4.2, by @g5t, registered in #3190
* For contributors: new `CONTRIBUTING.md` with an AI-declaration rule (#3153), a simpler PR template and `CLAUDE.md` (#3183, #3189), more `AGENTS.md` rules (#3194), one nightly release archive per platform (#3163), and the v3.10.3 install docs (#3131), by @willend

### McStas only

#### Samples
* Powders 1/2: `.lau` files regenerated with NCrystal, broken `.laz` files removed, `PowderN`/`Powder_process` `.lau` merge fixed, and a warning when the Debye-Waller factor is ignored for NCrystal files. Examples use `barns=0` with `.lau` files and have updated `%Test` values. By @willend in #3174
* Powders 2/2: `PowderN`, `Powder_process`, `Single_crystal`, Union `Single_crystal_process`, `Isotropic_Sqw` and `Texture_process` read the |F|² unit (barn or fm) from the file header, overriding the `barns` parameter, by @willend in #3176 (fixes #2112)
* New `PowderN_porous` component: a powder matrix with mesh pores, partly filled with an incoherent, absorbing liquid, with a `Test_PowderN_porous` example, by @willend in #3146
* `Texture_process`: cell vectors for any lattice, in the same crystal frame, by @willend in #3171
* `SasView_rectangular_prism`: the sasmodels kernel is included directly, by @willend in #3169
* `SpinWave_BCO` stops with a clear message for B < 0 with `FM=1`, and Union NCrystal uses the per-particle random numbers, by @willend in #3165

#### Union
* Union polyhedron display: only volumes that overlap are checked, and the surrounding vacuum is left out, by @lomholy-agentic in #3192

#### Other components
* Cylindrical monitors (`Cyl_monitor`, `Cyl_monitor_PSD`, `Cyl_monitor_TOF`, `PSDcyl_monitor`, `TOF2Q_cyl_monitor`, `TOF_cylPSD_monitor`): binning and array bounds overhauled, with a new `Test_Cyl_monitors` case, by @willend in #3167, #3200
* `MCPL_output`: no command-line buffer overflow with many instrument parameters (#3134), `initial_ray_count` left at -1 when a run is interrupted, with signals handled after `TRACE` (#3087, fixes #3181), and `MCPL_output_noacc` defaults to `weight_mode=1` (#3198), by @willend
* `Monochromator_bent(_complex)` stops on an unknown `plane_of_reflection` (#3167), `Source_simplex` interpolation and grid edge fixed (#3175), and `Guide_gravity` documents its `G` parameter (#3170), by @willend
* `Elliptic_guide_gravity` stable under `mccode-clangformat`, by @willend in #3177

#### Examples
* `PSI_ICON` split into shared beamline and camera parts, with foil and porous-sample variants (Union and NCrystal), by @willend in #3146

### McXtrace only
* `Mirror_curved`: new implementation with double curvature, fixed intersections and phase, `yheight` preferred to `width`. `Test_KB`, `MAXII_811` and `JJ_SAXS` rewritten with working KB mirrors, `SOLEIL_PUMA` simplified, and test values updated. By @farhi in #3137, #3151, #3156
* `Mirror_elliptic`: only the lower half of the ellipsoid reflects, back reflections for de-focusing, a safe grazing angle and the phase applied on reflection, by @farhi in #3186
* `Mirror_parabolic`: rays hit the curved surface and reflections are kept, with fixes for one-sided mirrors and the display, by @farhi and @willend in #3191
* `Grating_reflect`: lost sign in the interference phase, non-conical assumption, zero-weight rays and normalisation fixed, by @farhi in #3168
* `Grating_trans`: 0th order and double-counted absorption fixed, NIST data files checked, by @farhi in #3161
* `Filter`: refraction for all shapes, including OFF files, by @willend in #3172
* `PowderN`: incoherent cross-section units and state fixed (#3144). New `PowderN_porous` component, and `ILL_NeXT_Xray` split into source and detector parts with porous-sample variants (#3158), by @willend
* `Source_lab`: code clean-up and messages, by @farhi in #3157
* `Isotropic_Sqw` stable under `mccode-clangformat`, by @willend in #3177

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.10.3...v3.10.4

