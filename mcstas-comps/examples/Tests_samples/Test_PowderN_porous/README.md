# The `Test_PowderN_porous` Instrument

*McStas: Test of PowderN_porous: Al matrix with pore channels, water below fill_y*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Nov 2026

## Description

```text
Al cylinder with pore channels from pores.off (make with make_pores.py),
water in the pores below fill_y. pores="NULL" gives a plain PowderN-like sample.
```

## Examples

- **Test: Test_PowderN_porous.instr -y Detector: Transm_I=2.75452e-07**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength | 2.5 |
| pores | str | Pore mesh file, "NULL" for none | "pores.off" |
| fill_y | m | Water level in the pores (local y), sample spans +-0.01 | 0 |
| water | 1 | 1: water in the pores below fill_y, 0: void pores | 1 |

## Links

- [Source code](Test_PowderN_porous.instr) for `Test_PowderN_porous.instr`.

---
