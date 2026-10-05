Changes in McStas/McXtrace 3.9.0

*Covers everything since the previous "announced" release, 3.8.4.*

## Highlights

* **MPI is now enabled by default** when compiling instruments. Multi-core runs work out of the box, and a long-standing hang in MPI runs with empty event lists is fixed. By @willend.
* **NCrystal everywhere**: NCrystal materials can now be used in Union, and `PowderN` / `Single_crystal` accept multiphase and single-crystal NCrystal materials. New components: `Union_NCrystal_material` and `NCrystal_filter`. By @tkittel and @mads-bertelsen-agentic.
* **Union framework simplified**: `Union_init` and `Union_stop` are no longer needed in your instruments, the framework is now self-contained  (@mads-bertelsen-agentic and @g5t). Also new: `Union_abs_logger_nD_scintillator` (@MilanKlausz, with @tkittel).
* **MCViNE sample kernels in McStas**: 16 sample kernels from MCViNE (S(Q), S(Q,E), phonon, SANS and more), each available both as a standalone sample component and as a Union process. Contributed by @Fahima-Islam.
* **New `Dispersion_relation` sample component** for phonon or magnon scattering from tabulated (numerical) dispersion relations, tested against `Phonon_simple` and `SpinWave_BCO`. By Daniel Lomholt Christensen (@lomholy-agentic) and Kim Lefmann.
* **Full component settings in NeXus output**: in NeXus output mode, every component variable is now embedded in the output file with its actual runtime setting, so each simulation fully documents how its components were configured. By @willend, with related fixes by @tkittel.
* **Much faster `mcdisplay-webgl`**: the 3D instrument viewer no longer depends on Vite/npm and now starts up much faster. By @willend.
* **Models of NeXT, the ILL neutron + X-ray imaging instrument, in both packages**: `ILL_H521_NeXT` (neutron branch) in McStas and `ILL_NeXT_Xray` (X-ray branch) in McXtrace. Both were "vibe-coded" by @willend with Claude, starting from the instrument's webpage and the existing ILL information in McStas.
* **New SOLEIL beamlines for McXtrace**: SIRIUS, SAMBA and PUMA, plus fixes to CRISTAL. By @farhi, with PUMA written by Uxue Saez Fernandez.
* **Another big documentation round**: refreshed manuals, new Union chapters, web versions of the manuals, and updated documentation for every component and instrument. By @willend, AI-assisted.

## What's Changed

### Common to McStas and McXtrace

#### Documentation
* Full manual refresh against the current component code and tool documentation, plus installation instructions updated for 3.9.0, by @willend (branch `manual-refresh-before-release`, PR pending)
* Further manual refreshes and AI-assisted corrections by @willend in #2661, #2735, #2736
* New web (HTML) versions of the manuals by @willend in #2670
* New Union chapters in both the McStas and McXtrace manuals by @willend in #2675
* Component and instrument headers reviewed: missing parameter descriptions and units filled in by @willend in #2747
* Updated instrument README files by @willend in #2702, #2733
* Elliptic guide component header fixed by @ebknudsen in #2719
* As in 3.8, this documentation work was **AI-assisted** and reviewed by @willend. Please report any errors you spot via a GitHub issue.

#### Running simulations (`mcrun`/`mxrun`, `mcgui`/`mxgui`)
* Instruments are now **compiled with MPI enabled by default** by @willend in #2669, with follow-up fixes in #2671 and #2672 (conda installations)
* Fixed MPI runs hanging when a process ended up with no events by @willend in #2726
* Only values made of numbers are treated as scan ranges, so text parameters containing `:` are no longer mistaken for scans, by @tkittel in #2722
* `--yes` now keeps parameter values given on the command line by @willend in #2723
* `mcgui`/`mxgui` now warns clearly if the code editor component (qscintilla2) is missing by @willend in #2699

#### Visualisation and plotting
* `mcdisplay-webgl` no longer needs Vite/npm to run, so it now starts up much faster, by @willend in #2676
* Display fixes: field boxes by @ebknudsen in #2667, Conics components and particle-trace reading by @mads-bertelsen-agentic in #2677
* `mccoplot`: option to hide legend and title by @willend in #2660

#### `mcdoc`/`mxdoc`
* `mcdoc PowderN.comp` (and other unique matches) now opens the component directly by @willend in #2663
* Search-result output now appears in the right place by @willend in #2659
* Parameter descriptions spanning several lines are now supported by @willend in #2746
* Cleaner generated component headers by @willend in #2662

#### Testing (`mctest`)
* New `--noplots` option by @tkittel in #2734
* Instruments with `%Example` lines without parameters are now supported by @tkittel in #2708
* More reliable detection of compilation, runtime and plotting failures by @tkittel in #2706

#### Python instruments (`mcstas-pygen`)
* New `--instrument-name` option, and `%Example` lines are turned into tests, by @tkittel in #2707
* Search functionality by @tkittel in #2718
* Fixed handling of expressions in generated Python code by @tkittel in #2730

#### Simulation output and code generation
* In NeXus output mode, all component variables are now embedded in the output file together with their actual runtime setting (the value after initialisation), so you can always see exactly how each component was configured, by @willend in #2724
* Parameter expressions are now stored correctly in NeXus files by @tkittel in #2729
* C character literals in parameter expressions are handled correctly by @tkittel in #2732

#### GPU / OpenACC
* OpenACC fixes after many component and instrument edits by @willend in #2693, #2737
* Workaround for `Monitor_nD` with NVIDIA compiler 26.5 and newer by @mads-bertelsen in #2690

#### Installation, platforms and packaging
* macOS: no more unnecessary Rosetta prompts on Apple Silicon by @willend in #2700, #2704, #2705
* macOS: workaround for OpenMPI 5 issues by @willend in #2728
* macOS: `${SDKROOT}` is respected if set by @willend in #2656
* Minimum CMake version raised from 3.17 to 3.19 by @g5t in #2668
* External contributions (such as `mcstas-chopper-lib`) reorganised by @g5t in #2666, with chopper-lib updates v4.2.1 and v4.2.2 in #2695, #2710
* Removed obsolete files and the separate test-config packages by @willend in #2673, #2674
* Code style and minor fixes by @ebknudsen in #2681 and @willend in #2731
* For contributors: new `AGENTS.md` guide for AI coding agents by @willend in #2738, and smarter CI that only runs tests when component code actually changes, in #2747

### McStas only

#### NCrystal
* New `Union_NCrystal_material` component: NCrystal materials for Union, by @mads-bertelsen-agentic in #2698
* `PowderN` and `Single_crystal` support multiphase and single-crystal NCrystal materials by @tkittel in #2715
* NCrystal data files are also found next to your instrument file (`localdata/`) by @tkittel in #2714
* New `NCrystal_filter` component: a fast, GPU-friendly filter or window (box or cylinder) of any NCrystal material that only attenuates the beam, by @tkittel in #2727

#### Union
* `Union_init` and `Union_stop` are no longer required by @mads-bertelsen-agentic in #2655
* Union is now self-contained by @g5t in #2712
* MCViNE sample kernels as Union processes by @Fahima-Islam in #2716 (standalone versions listed under *MCViNE samples* below):
  * `MCViNE_SQ_process`, `MCViNE_SvQ_process`, `MCViNE_SQE_process`
  * `MCViNE_E_Q_process`, `MCViNE_E_vQ_process`, `MCViNE_Broadened_E_Q_process`, `MCViNE_LorentzianBroadened_E_Q_process`
  * `MCViNE_ConstantQE_process`, `MCViNE_ConstantvQE_process`, `MCViNE_ConstantEnergyTransfer_process`
  * `MCViNE_Phonon_CoherentInelastic_PolyXtal_process`, `MCViNE_Phonon_CoherentInelastic_SingleXtal_process`, `MCViNE_Phonon_IncoherentElastic_process`, `MCViNE_Phonon_IncoherentInelastic_process`
  * `MCViNE_DGSSXRes_process`, `MCViNE_SANS2D_ongrid_process`
* New `Union_abs_logger_nD_scintillator` component with test instrument by @MilanKlausz in #2687, with a flexible data location option (`table_dir`) by @tkittel in #2720
* `union_history.dat` is now saved in the output directory by @NoeB in #2697

#### MCViNE samples
* 16 sample kernels from MCViNE, now available as standalone McStas sample components, by @Fahima-Islam in #2716:
  * `MCViNE_SQ`, `MCViNE_SvQ`, `MCViNE_SQE`
  * `MCViNE_E_Q`, `MCViNE_E_vQ`, `MCViNE_Broadened_E_Q`, `MCViNE_LorentzianBroadened_E_Q`
  * `MCViNE_ConstantQE`, `MCViNE_ConstantvQE`, `MCViNE_ConstantEnergyTransfer`
  * `MCViNE_Phonon_CoherentInelastic_PolyXtal`, `MCViNE_Phonon_CoherentInelastic_SingleXtal`, `MCViNE_Phonon_IncoherentElastic`, `MCViNE_Phonon_IncoherentInelastic`
  * `MCViNE_DGSSXRes`, `MCViNE_SANS2D_ongrid`

#### Components and instruments
* New `Dispersion_relation` sample component, tested against `Phonon_simple` and `SpinWave_BCO`, by @lomholy-agentic in #2694
* New `ILL_H521_NeXT` model of the neutron branch of the NeXT imaging instrument at the ILL, AI-assisted (Claude), by @willend in #2703, #2711, #2713
* `Single_crystal` fix for an issue found by Iurii Kibalin by @willend in #2709
* `Guide_gravity` warning output fixed by @willend in #2701
* `NMO` alias component now works by @willend in #2725
* KDSource visualisation fixed by @jorobledo in #2692
* New gravity-free propagation functions for component developers by @mads-bertelsen-agentic in #2642

### McXtrace only
* New `ILL_NeXT_Xray` model of the X-ray branch of the NeXT imaging instrument at the ILL, AI-assisted (Claude), by @willend in #2703, #2711
* New instruments: SOLEIL SIRIUS by @farhi in #2685, #2744 and SOLEIL SAMBA in #2743
* New `SOLEIL_PUMA` instrument: the PUMA hard X-ray beamline for heritage science (XRF, XANES, XRD and XEOL, 4–22 keV), written by Uxue Saez Fernandez and added by @farhi in #2741
* SOLEIL CRISTAL compiles again (corrected undulator parameters) by @farhi in #2688
* SOLEIL instruments prepared for inclusion in a full storage-ring model by @farhi in #2739

## New Contributors
* @NoeB made their first contribution in #2697
* @lomholy-agentic made their first contribution in #2694
* @mccode-registrar[bot] made its first contribution in #2695

**Full Changelog**: https://github.com/mccode-dev/McCode/compare/v3.8.4...v3.9.0
