Changes in McStas/McXtrace 3.9.2

*A bug-fix release covering everything since the previous "announced" release, 3.9.0, including minor changes already shipped quietly in 3.9.1 on conda-forge. For the main new features of the 3.9 series, see the [3.9.0 release notes](https://github.com/mccode-dev/McCode/releases/tag/v3.9.0).*

## Highlights

* **OFF/PLY geometry library overhauled**: polygons can now have any number of corners, and several geometry bugs are fixed: gravity with OFF samples, double hits on shared edges, and McXtrace `Mirror` in OFF mode, which did not reflect. Faces in the geometry file can now also carry their own properties. By @willend.
* **`Guide_anyshape` now reads per-face coatings** (`m`, `alpha`, `W`, ...) directly from the OFF/PLY file with the new `file_coatings=1` option. This replaces `Guide_anyshape_r`, which moves to the obsolete components. By @willend.
* **ESS Test Beamline (TBL) in `ESS_butterfly`**: selecting sector W, beamline 11 automatically switches to a TBL mode that views both cold wings of the moderator. Consolidated by @willend with Claude from the TBL team's local version.
* **Correct neutron ID and user variables in Union absorption loggers**: the `Monitor_nD`-based Union loggers now record the real neutron ID and `user0`–`user9` values instead of random numbers. By @tkittel.
* **SasView samples combine with Union and more**: instruments using a SasView sample together with `Union_master`, `Refractor` or the Bispectral mirrors now compile. By @tkittel.
* **`mcstas-chopper-lib` updated to v4.2.3**: the external chopper components `NXdisk_chopper`, `Masked_ESS_butterfly` and `Polygon_ESS_butterfly` now show up properly in `mcdoc`. By @g5t and @willend.
* **McXtrace data files found again on Debian/Ubuntu package installs** when instruments are run directly, without `mxrun`. By @willend.

## What's Changed

### Common to McStas and McXtrace

#### Geometry (OFF/PLY files)
* Overhauled OFF/PLY geometry library: no limit on the number of corners per polygon, faces can carry their own properties, and fixes for gravity mode with OFF samples, double hits on shared edges and corners, faces whose first corners lie on a line, and more robust file reading. Checked against an independent reference calculation and the example instruments. By @willend in #2755
* Clearer rules for how the library reports intersections, with all users of it in McStas and McXtrace checked, by @willend in #2755

#### Running pygen-based simulations
* Instrument parameters without a default value no longer crash `mcstas-pygen`, by @tkittel in #2762

#### Installation, platforms and packaging
* McXtrace binaries now find their data files when run directly on a Debian/Ubuntu package install (where `$MCXTRACE` is not set), by @willend in #2761 (fixes #2759 from @farhi)
* Debian/Ubuntu packages now declare all their dependencies themselves, so installing single packages works without the full "suite" metapackages. `mcstas-comps` now depends on MCPL and NCrystal, and `mcxtrace-comps` on MCPL. By @willend in #2761
* More reliable cross-compilation of the Windows installers by @willend in #2751
* Release builds are now packaged as a single archive by @willend in #2758
* Release-notes updates by @willend in #2749, #2750
* For contributors: new CI test of Debian/Ubuntu package builds and of running binaries directly, by @willend in #2761. Basic CI tests are faster because plots are now made only on request, by @willend in #2763

### McStas only

#### Components
* `Guide_anyshape`: new `file_coatings=1` option to read `m`, `alpha`, `W`, `R0` and `Qc` per face from the OFF/PLY file. `Guide_anyshape_r` is now obsolete (it still works, with a deprecation warning): use `Guide_anyshape(file_coatings=1, ...)` instead. By @willend in #2755
* `ESS_butterfly`: automatic Test Beamline (TBL) mode for sector W, beamline 11, viewing both cold wings of the moderator. AI-assisted (Claude), by @willend in #2757
* `Vertical_Bender`: reflections on the curved walls were missed without gravity, so much less was transmitted than expected. Fixed by @tkittel in #2756
* `Union_master`, `Refractor`, `Mirror_Curved_Bispectral` and `Mirror_Elliptic_Bispectral` can now be combined with SasView samples in one instrument. Previously this failed to compile because of a clash with the name `I` (fixes #2546). By @tkittel in #2752
* `mcstas-chopper-lib` updated to v4.2.3: `NXdisk_chopper`, `Masked_ESS_butterfly` and `Polygon_ESS_butterfly` (with their test instruments) now render correctly in `mcdoc`, by @mccode-registrar[bot] in #2764 (upstream fixes by @willend, released by @g5t)

#### Union
* `Union_abs_logger_nD`, `Union_abs_logger_nD_scintillator`, `Union_abs_logger_1D_space_event` and `Union_abs_logger_event` now pass the real neutron to `Monitor_nD`. The neutron ID and user-variable columns previously held random values. By @tkittel in #2760

### McXtrace only
* `Mirror`: OFF/PLY geometry mode now reflects correctly, by @willend in #2755

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.9.0...v3.9.2

