# The `Test_PowderN_porous_Union` Instrument

*McXtrace: Union version of Test_PowderN_porous: Si matrix with pore channels, water below fill_y*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Nov 2026

## Description

```text
Same geometry and materials as Test_PowderN_porous, built from Union parts:
Si cylinder (Powder_process), water and void cylinders below/above fill_y
(1 um inside the matrix, no coincident surfaces) masked by the pore mesh
(pores.off, see Test_PowderN_porous for make_pores.py).
McXtrace Union absorption is a constant my_absorption, so the attenuation
coefficients are those PowderN_porous computes at 15 keV (narrow band only):
Si:    photoelectric 2207.14 /m (Si.txt col 3, 2.33 g/cm^3), Bragg from Si.laz,
incoherent 107.247 /m (Si.txt col 4 minus Bragg, isotropic)
water: O.txt at 0.888 g/cm^3: total 158.917 /m, of which 27.7025 /m scatters
isotropically (Incoherent_process), 131.2145 /m absorbed
Unlike PowderN_porous this has multiple scattering.
```

## Examples

- **Test: Test_PowderN_porous_Union.instr -y Detector: Banana_I=1.43865e-10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Mean energy (keep near 15 keV, see above) | 15 |
| pores | str | Pore mesh file | "pores.off" |
| fill_y | m | Water level in the pores (local y), sample spans +-0.001 | 0 |
| water | 1 | 1: water in the pores below fill_y, 0: void pores | 1 |

## Links

- [Source code](Test_PowderN_porous_Union.instr) for `Test_PowderN_porous_Union.instr`.

---
