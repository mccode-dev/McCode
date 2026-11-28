# The `ILL_NeXT_Xray_porous_Union` Instrument

*McXtrace: NeXT (ILL) X-ray arm with a porous Si sample (Union), water in the pores below fill_y*

## Identification

- **Site:** ILL
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Nov 2026

## Description

```text
ILL_NeXT_Xray_source and ILL_NeXT_Xray_detector (see %include below) with the
sample of ILL_NeXT_Xray_porous built from Union components: Si cylinder,
water/void cylinders below/above fill_y (1 um inside the matrix), masked by the
pore mesh pores.off (in the data folder).

McXtrace Union materials have one constant absorption coefficient, so the
attenuation is taken from the tables at a single effective energy E_eff:
Si (2.33 g/cm^3, Si.txt): photoelectric -> my_absorption,
coherent + incoherent -> Incoherent_process (isotropic, Bragg not separate)
water (1 g/cm^3, H2O.txt): total attenuation -> my_absorption, no scattering
The default E_eff=34.4 keV reproduces the dry transmission (0.21) of ILL_NeXT_Xray_porous
at 150 kV without filter; beam hardening is not described.
Unlike PowderN_porous this has multiple scattering.
All ILL_NeXT_Xray_source parameters are available as well.
```

## Examples

- **Test: ILL_NeXT_Xray_porous_Union.instr -y Detector: Xray_counts_I=1.80367e+11**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SOD | m | Source - object (rotation axis) distance | 0.05 |
| E_eff | keV | Energy at which the attenuation coefficients are taken | 34.4 |
| pores | str | Pore mesh file (Tests_samples/Test_PowderN_porous/make_pores.py) | "pores.off" |
| fill_y | m | Water level in the pores, local y (sample spans +-0.005) | 0 |
| water | 1 | 1: water below fill_y, 0: void pores | 1 |

## Links

- [Source code](ILL_NeXT_Xray_porous_Union.instr) for `ILL_NeXT_Xray_porous_Union.instr`.

---
