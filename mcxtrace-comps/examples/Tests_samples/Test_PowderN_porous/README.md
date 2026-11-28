# The `Test_PowderN_porous` Instrument

*McXtrace: Test of PowderN_porous: Si matrix with pore channels, water below fill_y*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Nov 2026

## Description

```text
Si cylinder with pore channels from pores.off (make with make_pores.py, e.g.
python3 make_pores.py --radius 5e-4 --yheight 2e-3 --diameter 6e-5
--diameter_sigma 1e-5 --length 3e-4 --length_sigma 1e-4 --seed 1),
water in the pores below fill_y. Water is approximated by its oxygen
(O.txt at 0.888 g/cm^3), hydrogen adds a few % of its attenuation.
pores="NULL" gives a plain PowderN-like sample.
```

## Examples

- **Test: Test_PowderN_porous.instr -y Detector: Banana_I=1.42463e-10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Mean energy | 15 |
| pores | str | Pore mesh file, "NULL" for none | "pores.off" |
| fill_y | m | Water level in the pores (local y), sample spans +-0.001 | 0 |
| liquid | str | Attenuation table of the liquid in the pores below fill_y, "NULL": void pores | "O.txt" |

## Links

- [Source code](Test_PowderN_porous.instr) for `Test_PowderN_porous.instr`.

---
