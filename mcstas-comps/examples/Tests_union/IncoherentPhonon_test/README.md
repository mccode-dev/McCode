# The `IncoherentPhonon_test` Instrument

*McStas: Test of IncoherentPhonon_process*

## Identification

- **Site:** Tests_union
- **Author:** Mads Bertelsen
- **Origin:** Johns Hopkins University, Baltimore
- **Date:** May 2016

## Description

```text
Test instrument for the IncoherentPhonon_process physics process from
V. Laliena, Uni Zaragoza.

Example: comp_select=1 Detector: energy_mon_2_I=1156.84
```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp_select |  | 1: Union components, 2: Incoherent | 1 |
| sample_radius | m | Radius of sample | 0.01 |
| sample_height | m | Height of sample | 0.03 |
| pack |  | Packing factor | 1 |
| sigma_inc_vanadium | barns | Incoherent cross-section | 5.08 |
| sigma_abs_vanadium | barns | Absorption cross-section | 5.08 |
| Vc_vanadium | AA^3 | Unit cell volume | 13.827 |
| geometry_interact |  | p_interact for the Union sample | 0.5 |
| nphe_exact | 1 | Number of terms in the phonon expansion taken exact. Has to be between 1 and 3 | 1 |
| nphe_approx | 1 | Number of terms in the phonon expansion taken approximate | 0 |
| approx | 1 | Approximation type: 0 gaussian, 1 saddle point | 0 |
| mph_resum | 0/1 | Resumate the remaining terms of the phonon expansion via a saddle point: 0 No, 1 Yes | 0 |
| T | K | Temperature | 294 |
| density | g/cm^3 | Material density | 6.0 |
| M | amu | Ion mass | 50.94 |
| sigmaCoh | barns | Coherent scattering cross section | 0.0184 |
| sigmaInc | barns | Incoherent scattering cross section | 5.08 |
| dosfn | string | Path to the file that contains the DoS | "dos_meV.txt" |
| nxs | 1 | Number of energy points at which the total cross sections are precomputed | 1000 |
| kabsmin | AA^-1 | Lower cut-off for the neutron wave-vector k | 0.1 |
| kabsmax | AA^-1 | Higher cut-off for the neutron wave-vector k | 25 |
| interact_fraction | 1 | How large a part of the scattering events should use this process 0-1 (sum of all processes in material = 1) | -1 |

## Links

- [Source code](IncoherentPhonon_test.instr) for `IncoherentPhonon_test.instr`.

---
