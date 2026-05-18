# The `Powder_validation` Instrument

*McStas: Validation of Union powder against standard PowderN component.*

## Identification

- **Site:** Union_validation
- **Author:** Mads Bertelsen
- **Origin:** Johns Hopkins University, Baltimore
- **Date:** May 2016

## Description

```text
Validation of Union powder against standard PowderN component.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp_select | 1 | 1=Union components, 2=PowderN | 1 |
| material_data_file | string | Powder sample definition | "Al.laz" |
| E0 | meV | Source mean energy | 100 |
| dE | meV | Source energy spread | 2 |
| sample_radius | m | Sample radius | 0.01 |
| sample_height | m | Sample height | 0.01 |
| pack |  | Sample packing factor | 1 |
| sigma_inc | barns | Sample incoherent cross-section | 0.0328 |
| sigma_abs | barns | Sample absorption cross-section | 0.924 |
| Vc | AA | Sample unit cell volume | 66.4 |
| geometry_interact |  | p_interact for the sample | 0.5 |
| incoherent_fraction |  | Fraction of statistics assigned for incoherent scattering | 0.2 |
| div | deg | Source divergence (angular w and h) | 1.5 |
| d_phi | deg | Restriction of Debye-Scherrer cones around scattering plane 0=full illumination | 30 |

## Links

- [Source code](Powder_validation.instr) for `Powder_validation.instr`.

---
