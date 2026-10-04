# The `NCrystal_PowderN_multiphase` Instrument

*McStas: Example instrument for PowderN with a (multiphase) NCrystal material*

## Identification

- **Site:** NCrystal
- **Author:** T. Kittelmann
- **Origin:** ESS
- **Date:** 2026

## Description

```text
Simple powder diffractometer, with a PowderN sample whose reflections are
taken from a two-phase NCrystal material, with the cfg-string built from the
instrument parameters, e.g.

"phases<0.3*stdlib::Al_sg225.ncmat&0.7*stdlib::Cu_sg225.ncmat>"

for the default parameters (the fractions are volume fractions).
```

## Examples

- **Test: frac1=0.3 Detector: banana_I=6.03**
- **Test: frac1=1 Detector: banana_I=1.28**
- **Test: phase1="stdlib::Al_sg225.ncmat;density=0.5x" frac1=1 Detector: banana_I=0.649**
- **Test: phase2="stdlib::Epoxy_Araldite506_C18H20O3.ncmat" frac1=0.5 Detector: banana_I=22.3**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| phase1 | string | NCrystal cfg-string of the first phase. | "stdlib::Al_sg225.ncmat" |
| phase2 | string | NCrystal cfg-string of the second phase. | "stdlib::Cu_sg225.ncmat" |
| frac1 | 1 | Volume fraction of the first phase (the second phase gets 1-frac1). With frac1=1 (or 0), only the first (or second) phase is used. | 0.3 |
| lambda | AA | Mean wavelength of the incident neutrons. | 2.0 |

## Links

- [Source code](NCrystal_PowderN_multiphase.instr) for `NCrystal_PowderN_multiphase.instr`.
- https://github.com/mctools/ncrystal/wiki/

---
