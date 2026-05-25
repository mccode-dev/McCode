# The `McStas_Isotropic_Sqw` Instrument

*McStas: Wrapper instrument for use of Isotropic_Sqw in SIMRES*

## Identification

- **Site:** SINE2020
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** December 2018

## Description

```text
This instrument provides an MCPL-based interface for the use of Isotropic_Sqw in the SIMRES package.
The instrument has been developed in the context of WP8 in the SINE2020 project and is part of
deliverable D8.8.

(EU Horizon 2020 research and innovation programme under grant agreement No 654000).

The default material is liquid Rb as a cylinder of radius 0.01 m x height 0.07 m

Example: McStas_Isotropic_Sqw Sqw_coh=Rb_liq_coh.sqw  Sqw_inc=Rb_liq_inc.sqw radius=0.01 yheight=0.07
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Sqw_coh | string | Name of the file containing the values of Q, w and S(Q,w) Coherent part; Q in Angs-1, E in meV, S(q,w) in meV-1. Use 0, NULL or "" to disable. | "Rb_liq_coh.sqw" |
| Sqw_inc | string | Name of the file containing the values of Q, w and S(Q,w). Incoherent (self) part. Use 0, NULL or "" to scatter isotropically (V-like). | "Rb_liq_inc.sqw" |
| geometry | string | Name of an Object File  Format  (OFF)  or  PLY file for complex geometry. | "" |
| radius | m | Sample radius | 0.01 |
| xwidth | m | Sample width along x | 0 |
| yheight | m | Sample height along y | 0.07 |
| zdepth | m | Sample depth along z | 0 |
| thickness | m | Thickness of hollow sample Negative value extends the hollow volume outside of the box/cylinder. | 0 |
| threshold | m | Value under which S(Q,w) is not accounted for. to set according to the S(Q,w) values, i.e. not too low. | 1e-20 |
| T | K | Temperature of sample, detailed balance. Use T=0 to disable it. and T=-1 to automatically set it from non-classical S(q,w). | 0 |
| d_phi | deg | Vertical focusing limit [deg] | 0 |
| verbose | 1 | Verbosity level (0:silent, 1:normal, 2:verbose, 3:debug). A verbosity>1 also computes dispersions and S(q,w) analysis. | 1 |
| classical | 1 | Assumes the S(q,w) data from the files is a classical S(q,w), and multiply that data by exp(hw/2kT) on up/down energy sides. Use 0 when obtained from raw experiments, 1 from molecular dynamics. Use -1 to guess from a data set including both energy sides. | -1 |
| powder_barns | 1 | 0 when \|F2\| data in powder file are fm^2, 1 when in barns (barns=1 for laz, barns=0 for lau type files). | 1 |
| quantum_correction | str | Specify the type of quantum correction to use "Boltzmann"/"Schofield","harmonic"/"Bader" or "standard"/"Frommhold" (default) | "standard" |
| norm | 1 | Normalize S(q,w) when -1 (default). Use raw data when 0, multiply S(q,w) when norm>0. | -1 |
| rot_x | deg | Sample rotation around x | 0 |
| rot_y | deg | Sample rotation around y | 0 |
| rot_z | deg | Sample rotation around z | 0 |

## Links

- [Source code](McStas_Isotropic_Sqw.instr) for `McStas_Isotropic_Sqw.instr`.
- Website for the <a href="https://mctools.github.io/mcpl">MCPL</a> particle exchange format
- Website for the <a href="http://neutron.ujf.cas.cz/ restrax/">SIMRES</a> package
- Website for <a href="https://www.sine2020.eu/randd-activities/e-tools.html">WP8 in EU-SINE2020</a>

---
