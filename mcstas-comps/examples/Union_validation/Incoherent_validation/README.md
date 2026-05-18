# The `Incoherent_validation` Instrument

*McStas: Validation of Union components against incoherent scattering component*

## Identification

- **Site:** Union_validation
- **Author:** Mads Bertelsen
- **Origin:** Johns Hopkins University, Baltimore
- **Date:** May 2016

## Description

```text
Validation of Union components against incoherent scattering component
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp_select |  | 1: Union components, 2: Incoherent | 1 |
| sample_radius | m | Radius of sample | 0.01 |
| sample_height | m | Height of sample | 0.01 |
| pack |  | Packing factor | 1 |
| sigma_inc_vanadium | barns | Incoherent cross-section | 5.08 |
| sigma_abs_vanadium | barns | Absorption cross-section | 5.08 |
| Vc_vanadium | AA^3 | Unit cell volume | 13.827 |
| geometry_interact |  | p_interact for the Union sample | 0.5 |

## Links

- [Source code](Incoherent_validation.instr) for `Incoherent_validation.instr`.

---
