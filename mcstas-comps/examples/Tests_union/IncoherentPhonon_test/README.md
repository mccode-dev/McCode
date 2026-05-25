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
| nphe_exact |  |  | 1 |
| nphe_approx |  |  | 0 |
| approx |  |  | 0 |
| mph_resum |  |  | 0 |
| T |  |  | 294 |
| density |  |  | 6.0 |
| M |  |  | 50.94 |
| sigmaCoh |  |  | 0.0184 |
| sigmaInc |  |  | 5.08 |
| dosfn |  |  | "dos_meV.txt" |
| nxs |  |  | 1000 |
| kabsmin |  |  | 0.1 |
| kabsmax |  |  | 25 |
| interact_fraction |  |  | -1 |

## Links

- [Source code](IncoherentPhonon_test.instr) for `IncoherentPhonon_test.instr`.

---
