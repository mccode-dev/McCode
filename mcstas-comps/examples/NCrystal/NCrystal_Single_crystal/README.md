# The `NCrystal_Single_crystal` Instrument

*McStas: Example instrument for Single_crystal with an NCrystal material*

## Identification

- **Site:** NCrystal
- **Author:** T. Kittelmann
- **Origin:** ESS
- **Date:** 2026

## Description

```text
A white beam hits a Single_crystal sample, whose crystal structure is taken
from an NCrystal material, and the scattered neutrons are recorded on a
spherical detector (Laue pattern) and on a banana detector. Optionally, a
non-crystalline phase is added with the parameters extra_phase and
extra_frac, e.g. giving the NCrystal cfg-string

"phases<0.9*stdlib::Al_sg225.ncmat&0.1*stdlib::Epoxy_Araldite506_C18H20O3.ncmat>"

for extra_frac=0.1.
```

## Examples

- **Test: omega=0 Detector: laue_I=1.12**
- **Test: crystal="stdlib::Al_sg225.ncmat;density=0.5x" Detector: laue_I=0.653**
- **Test: crystal="stdlib::Ge_sg227.ncmat" omega=10 Detector: laue_I=4.87**
- **Test: extra_frac=0.1 Detector: laue_I=9.46**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| crystal | string | NCrystal cfg-string of the crystal. | "stdlib::Al_sg225.ncmat" |
| extra_phase | string | NCrystal cfg-string of an additional non-crystalline phase (only used if extra_frac>0). | "stdlib::Epoxy_Araldite506_C18H20O3.ncmat" |
| extra_frac | 1 | Volume fraction of the additional phase. | 0 |
| omega | deg | Rotation of the crystal around the vertical axis. | 0 |
| mosaic | arcmin | Isotropic mosaic of the crystal. | 30 |

## Links

- [Source code](NCrystal_Single_crystal.instr) for `NCrystal_Single_crystal.instr`.
- https://github.com/mctools/ncrystal/wiki/

---
