# MCViNE sample kernels in McStas

This note documents the McStas components ported from the sample scattering kernels of MCViNE (https://github.com/mcvine/mcvine, mccomponents/lib/kernels/sample, and the SANS2D_ongrid kernel of https://github.com/mcvine/acc).

Each kernel comes in two forms that run the same kernel code (`share/mcvine-lib.c`):

- **Union process** `contrib/MCViNE_<kernel>_process.comp`: geometry, containers, absorption and multiple scattering are handled by Union (`Union_make_material`, `Union_box/cylinder/sphere/mesh`, `Union_master`).
- **Standalone sample** `contrib/MCViNE_<kernel>.comp`: a box, (hollow) cylinder or sphere with its own ray tracing (MCViNE's HomogeneousNeutronScatterer).

Test instruments: `examples/Tests_samples/Test_MCViNE_<kernel>/`. Each runs the same sample either as the Union process (`comp_select=1`) or as the standalone component (`comp_select=2`), so the two `%Example` values can be compared directly. Test data is in `data/MCViNE/`.

## 1. Kernels and their McStas status

| MCViNE kernel | Closest McStas component(s) | Status | Port |
|---|---|---|---|
| IsotropicKernel | `Incoherent`, `Isotropic_Sqw` (no S(q,w) file) | covered | – |
| SQEkernel + GridSQE | `Isotropic_Sqw` | covered | (also in `MCViNE_SQE`) |
| SimplePowderDiffractionKernel | `PowderN`, `Powder1`, Union `Powder_process` | covered | – |
| SingleCrystalDiffractionKernel | `Single_crystal`, Union `Single_crystal_process` | covered | – |
| SANSSpheresKernel, SANSSphereModelKernel | `Sans_spheres`, `SANS_spheres2`, SasView models | covered | – |
| MultiPhonon_Kernel (S(Q,E) from DOS → SQEkernel) | Union `IncoherentPhonon_process`, `NCrystal_sample` | covered | – |
| ConstantEnergyTransferKernel | none (`Res_sample` is a uniform band) | **missing** | `MCViNE_ConstantEnergyTransfer` |
| ConstantQEKernel | none (`Spot_sample` fixes 2θ, not \|Q\|) | **missing** | `MCViNE_ConstantQE` |
| ConstantvQEKernel | none | **missing** | `MCViNE_ConstantvQE` |
| E_Q_Kernel (analytic E(\|Q\|), S(Q)) | none | **missing** | `MCViNE_E_Q` |
| Broadened_E_Q_Kernel (Gaussian) | none | **missing** | `MCViNE_Broadened_E_Q` |
| LorentzianBroadened_E_Q_Kernel | none | **missing** | `MCViNE_LorentzianBroadened_E_Q` |
| E_vQ_Kernel (analytic E(**Q**), S(**Q**)) | only fixed models (`Phonon_simple`, `Magnon_bcc`, `SpinWave_BCO`, `Phonon_BvK_PG`) or 4D grids (`Single_crystal_inelastic`) | **missing** | `MCViNE_E_vQ` |
| SQkernel (+ GridSQ / SQ_fromexpression) | none (elastic isotropic S(\|Q\|)) | **missing** | `MCViNE_SQ` |
| SvQkernel (+ GridSvQ) | none (elastic S(**Q**)) | **missing** | `MCViNE_SvQ` |
| SQE_fromexpression, SQE_EnergyFocusing_Kernel | `Isotropic_Sqw` has neither analytic S nor Ef focusing | **missing** | `MCViNE_SQE` |
| DGSSXResKernel | `TOFRes_sample` (same idea, different weighting) | partial | `MCViNE_DGSSXRes` |
| phonon IncoherentElastic (Debye–Waller) | `Incoherent` (no DW); NCrystal | partial | `MCViNE_Phonon_IncoherentElastic` |
| phonon IncoherentInelastic (+ EnergyFocusing) | Union `IncoherentPhonon_process`, NCrystal (not standalone one-phonon, no Ef focusing) | partial | `MCViNE_Phonon_IncoherentInelastic` |
| phonon CoherentInelastic_PolyXtal (dispersion + polarizations on grid) | none | **missing** | `MCViNE_Phonon_CoherentInelastic_PolyXtal` |
| phonon CoherentInelastic_SingleXtal (dispersion + polarizations on grid) | `Single_crystal_inelastic` needs a precomputed 4D S(q,w), not eigenvectors | **missing** | `MCViNE_Phonon_CoherentInelastic_SingleXtal` |
| SANS2D_ongrid (mcvine/acc) | none (McStas SANS components are isotropic in \|Q\|) | **missing** | `MCViNE_SANS2D_ongrid` |
| EPSCDiffractionKernel | – | not ported: unfinished in MCViNE (the .cc contains stray Python and is not in the CMake build) | – |


## 2. Files

| Location | Content |
|---|---|
| `share/mcvine-lib.h/.c` | kernel physics, run-time expression evaluator, table/grid readers, DOS and Debye–Waller helpers, MCViNE IDF phonon readers, shapes and standalone transport |
| `share/mcvine-union-lib.h/.c` | Union glue: a single process type `MCViNE` used by all kernels |
| `share/union-lib.h` | declaration of the `MCViNE` process type (enum entry, `data_transfer_union` member, empty default dispatch cases) |
| `contrib/MCViNE_*_process.comp` | 16 Union processes |
| `contrib/MCViNE_*.comp` | 16 standalone samples |
| `examples/Tests_samples/Test_MCViNE_*` | 16 test instruments (Union and standalone) |
| `data/MCViNE/` | test inputs: toy fcc phonon IDF set and atoms file, Debye DOS, S(q,w) grid, SANS map |

All components are `NOACC` (CPU only).

## 3. Using the Union processes

```
COMPONENT phon = MCViNE_Phonon_CoherentInelastic_SingleXtal_process(
    idf_dir="MCViNE/fcc_toy_phonons", atoms="MCViNE/fcc_toy_atoms.dat", T=300)
  AT (0,0,0) ABSOLUTE ROTATED (0, 30, 0) ABSOLUTE     // crystal orientation
COMPONENT mat  = Union_make_material(my_absorption=1.39, process_string="phon") AT (0,0,0) ABSOLUTE
COMPONENT samp = Union_box(xwidth=0.01, yheight=0.01, zdepth=0.01, priority=1, material_string="mat") AT (0,0,5) ABSOLUTE
COMPONENT master = Union_master() AT (0,0,5) ABSOLUTE
```

- **Absorption** is a material property: `Union_make_material(my_absorption=...)` (inverse penetration depth at 2200 m/s). The coherent phonon processes print the value implied by their atoms file.
- **Parameters:** processes take `packing_factor` and `interact_fraction` like the other Union processes, and `scattering_coefficient` [1/m] or `sigma_scat` [barn] with `Vc` [Å³] (phonon kernels: `sigma_inc` and `Vc`, or the atoms file).
- **Orientation:** the anisotropic kernels (ConstantvQE, E_vQ, SvQ, SANS2D_ongrid, CoherentInelastic_SingleXtal) follow the ROTATED placement of the process component, as `Single_crystal_process` does.
- **DGSSXRes** aims with the Union focusing of the geometry (`target_index` or `target_x/y/z`, and `focus_r`, `focus_xw/focus_xh` or `focus_aw/focus_ah`). The other kernels sample their own final directions, as in MCViNE.

### Implementation note on the Union core

Union selects the physics functions with a `switch` on `enum process` (no function pointers, for GPU builds). One new entry, `MCViNE`, is added. Its storage struct carries a pointer to the kernel and to its sampling function, so further MCViNE kernels can be added without touching the Union core again. In `share/union-lib.h` the change adds the enum entry, the `data_transfer_union` member and empty default dispatch macros (`UNION_CASE_PHYSICS_MY_MCVINE`, `UNION_CASE_PHYSICS_SCATTERING_MCVINE`); `share/mcvine-union-lib.h` replaces those defaults with the two `case MCViNE:` entries, so they are only active in instruments that use an MCViNE process. Like the other Union components, each process takes the shared Union state with `union_acquire()` in INITIALIZE and returns it with `union_release()` in FINALLY, and accepts the deprecated, unused `init` parameter. It does not affect existing processes.

## 4. Standalone components: transport, expressions and data formats

- **Geometry:** box (`xwidth,yheight,zdepth`), cylinder (`radius,yheight`), hollow cylinder (+`thickness`), or sphere (`radius`). Q vectors, targets, reciprocal vectors and atom positions are in the component frame; rotate the component to orient a crystal.
- **Cross sections:** `absorption_coefficient` (at 2200 m/s) and `scattering_coefficient` in 1/m, as in MCViNE, or `sigma_abs`, `sigma_scat` [barn] with `Vc` [Å³].
- **Transport:** forced scattering at a uniformly chosen depth, weighted by `L·exp(-(μ+σ)x)·σ` and the exit attenuation, exactly like MCViNE's `interact_path1`. `p_transmit` keeps a fraction as attenuated direct beam, `order>1` adds further (analog) scatterings, and `pack` scales μ and σ.

### Expressions

`E_Q`, `S_Q`, `sigma_Q`, `gamma_Q`, `SQ`, `SvQ` and `SQE` are strings evaluated at run time, like MCViNE's fparser. The supported syntax is:
- operators `+ - * / ^ ** %`, comparisons, `&& ||`
- `if(c,a,b)`
- functions `sin cos tan asin acos atan sinh cosh tanh exp log log10 log2 sqrt abs floor ceil int sign cbrt pow atan2 min max hypot fmod`
- constants `pi`, `e`

The variables are `Q` (Å⁻¹), `E` (meV), and `Qx Qy Qz`.

### Data formats

- **S(Q) table:** two columns, Q and S.
- **S(Q,E) grid:** the `Isotropic_Sqw` layout: a row of q values, a row of ω values, then nq rows of nω values.
- **S(**Q**) grid:** three rows of `min max n`, then nx·ny·nz values with Qx slowest.
- **SANS2D map:** a row of Qx, a row of Qy, then ny rows of nx values.
- **DOS:** two ASCII columns, E [meV] and g. A `#...THz` comment switches the energy unit to THz. The MCViNE IDF binary `DOS` file is also read.
- **Phonons:** an MCViNE IDF directory containing `Qgridinfo`, `Omega2`, `Polarizations` and optionally `DOS`, e.g. from phonopy through MCViNE's tools. It needs an atoms file with columns `x y z [Å] mass [amu] b_coh [fm] σ_inc σ_abs [barn]`, in the same atom order as the IDF files.


## 5. Where the port differs from MCViNE

1. **Kinematically forbidden events are absorbed.** In MCViNE, the neutron carries on unchanged but keeps the scattering weight.
2. **Grids are interpolated.** Grids are point values with linear, bilinear or trilinear interpolation, and give 0 outside the grid. MCViNE's `Grid*` functors use nearest-bin lookup and throw an error out of range.
3. **Three MCViNE sampling loops are biased, and each has an `unbiased=1` switch.** The default (`unbiased=0`) reproduces MCViNE:
   - **E_Q:** retries and divides the weight by the number of attempts, which is about +1.7 % in the test below.
   - **Broadened / Lorentzian E_Q:** retry without an acceptance correction.
   - **CoherentInelastic_SingleXtal:** retries directions until a root exists. The error was small in the tests.
   - **CoherentInelastic_PolyXtal:** uses an empirical accessible-volume formula, which comes out about +8 % in the tests.
4. **PolyXtal ×2 factor:** the factor for "two choices of E_f" now tests Ei against the phonon energy. MCViNE tests it against the signed energy transfer, which doubles the weight even when only annihilation is possible.
5. **SANS2D_ongrid:** the port uses vz = √(vi² − vx² − vy²). mcvine/acc has `vx²·vy²` there, which is a bug.
6. **b_coh** is read with its sign from the atoms file. MCViNE derives |b| from σ_coh using `periodictable`.
7. **Multiple scattering:** MCViNE splits each event into transmitted, absorbed and scattered copies (`mcweights`, default 1:1:1). McStas can't split events, so the absorbed branch is folded into the weight and further scatterings are sampled analogically. The expected values are the same.


## 6. Validation

Scripts (attached to the pull request) built dedicated test instruments and compare the ports against analytic integrals, McStas's own components, or independent Python calculations. Ratios are relative to McStas `Incoherent` with the same σ, and ± is the Monte Carlo statistical error.

| Test | Result | Expected |
|---|---|---|
| SQ, S=1 vs `Incoherent` | 1.0008 ± 0.0024 | 1 |
| SQ, S=Q² (expression / table) | 57.89 ± 0.17 / 58.04 ± 0.17 | 57.91 (=2ki²) |
| E_Q, MCViNE mode / unbiased | 0.8735 / 0.8593 ± 0.0021 | 0.8734 (MCViNE algorithm) / 0.8587 (exact) |
| Broadened σ=3, Lorentzian γ=1 (unbiased) | 0.8537 / 0.8443 | 0.8541 / 0.8434 (2D quadrature) |
| ConstantEnergyTransfer, ConstantQE | 1.0000, 0.9995; E and \|Q\| exact to 1e-7 | 1 |
| E_vQ with flat E=15 | 0.8660 ± 0.0019 | √(45/60)=0.8660 |
| E_vQ, dispersive | max \|E−E(**Q**)\| = 7e-6 meV | 0 |
| SvQ, S=Qx² (expression / 3D grid) | 9.658 / 9.673 ± 0.026 | ki²/3 = 9.652 |
| SQE (expression / grid / Ef focusing) | 2.761 / 2.729 / 2.396 | 2.769 / 2.769 / 2.402 |
| Phonon IncoherentElastic, DW core from DOS | 0.004309 Å² | 0.004316 Å² (independent quadrature) |
| Phonon IncoherentInelastic, Debye DOS (plain / Ef focusing) | 0.1543 / 0.0130 | 0.1544 / 0.0130 |
| CoherentInelastic_SingleXtal, fixed orientation | 0.214 (grid kinematics: median 0.06 meV) | 0.207 (independent Python, exact eigenvectors) |
| CoherentInelastic_PolyXtal (unbiased / MCViNE mode) | 0.226 / 0.246 | ≈ incoherent approximation 0.247; average of 24 orientations 0.21 |
| DGSSXRes | arrival times within ±1.00e-5 s of target; angle 60.00° | ±1e-5 s |
| Transport, order=1 (cylinder / box / sphere) | = `Incoherent` within 0.3 % | |
| Transport, hollow cylinder | 0.18657 | 0.18659 (independent) |
| Transport, sphere order=2 | 0.4512 | 0.4520 (analog MC) |
| p_transmit | transmitted 0.5335 | exp(−μL) = 0.5327 |

### Union processes vs standalone components

A second set compared each process with its standalone component. Both use the same cylinder (r=5 mm, h=10 mm) and absorption, with full multiple scattering: Union_master on one side and `order=20` on the other. The table shows the scattered intensity ratio and the weighted mean E and |Q| of the scattered neutrons.

| Kernel | I_union / I_standalone | ⟨E⟩ standalone / Union (meV) | ⟨\|Q\|⟩ standalone / Union (Å⁻¹) |
|---|---|---|---|
| ConstantEnergyTransfer | 1.003 ± 0.004 | 15.71 / 15.72 | 6.714 / 6.707 |
| ConstantQE | 0.999 ± 0.004 | 15.76 / 15.77 | 4.045 / 4.045 |
| ConstantvQE | 0.999 ± 0.008 | 33.72 / 33.72 | 2.291 / 2.291 |
| E_Q | 0.998 ± 0.009 | 11.07 / 11.02 | 6.698 / 6.673 |
| Broadened_E_Q | 1.000 ± 0.009 | 11.06 / 11.13 | 6.693 / 6.708 |
| LorentzianBroadened_E_Q | 1.000 ± 0.008 | 10.33 / 10.30 | 6.671 / 6.676 |
| E_vQ | 0.979 ± 0.027 | 34.74 / 35.06 | 6.108 / 6.091 |
| SQ | 1.000 ± 0.005 | 0 / 0 | 7.127 / 7.131 |
| SvQ | 1.001 ± 0.004 | 0 / 0 | 7.153 / 7.157 |
| SQE (grid) | 0.964 ± 0.037 | 11.86 / 11.48 | 3.923 / 3.889 |
| SANS2D_ongrid | 0.996 ± 0.007 | 0 / 0 | 0.123 / 0.123 |
| Phonon_IncoherentElastic | 0.995 ± 0.009 | 0 / 0 | 6.366 / 6.378 |
| Phonon_IncoherentInelastic | 0.995 ± 0.004 | 1.77 / 1.75 | 8.250 / 8.246 |
| Phonon_CoherentInelastic_PolyXtal | 0.995 ± 0.022 | 3.53 / 3.41 | 7.943 / 7.893 |
| Phonon_CoherentInelastic_SingleXtal | 1.023 ± 0.102 | 3.19 / 2.80 | 7.920 / 8.011 |

With the process rotated 30° about y, ConstantvQE gives 1.003 ± 0.006 and the same mean final velocity vector to 0.1 m/s. SvQ and SingleXtal agree within their (large) errors. DGSSXRes with Union geometry focusing gives 1.003 ± 0.006, with the same mean final velocity as the standalone target.

McStas `Incoherent` disagrees for two cases. For a hollow cylinder, its hole has closed ends. With `order=2` it overshoots the analog-MC value, so it can't be used as the reference there.

The phonon test data is a toy fcc nearest-neighbour Born–von Kármán model, not real Al. `data/MCViNE/make_fcc_toy_phonons.py` regenerates it.
