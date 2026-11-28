# The `ILL_NeXT_Xray_porous` Instrument

*McXtrace: NeXT (ILL) X-ray arm with a porous Si sample, water in the pores below fill_y*

## Identification

- **Site:** ILL
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Nov 2026

## Description

```text
ILL_NeXT_Xray_source and ILL_NeXT_Xray_detector (see %include below) with a
porous Si cylinder (PowderN_porous, Si.laz / Si.txt), radius 4 mm, height 10 mm,
pore channels from pores.off (in the data folder, made with Tests_samples/Test_PowderN_porous/make_pores.py,
same mesh as the McStas PSI_ICON_porous models), water (H2O.txt, absorption
only) in the pores below fill_y.
SOD is redeclared to 0.05 m (M=10, 56 um pixel at the object with det_binning=4).
All ILL_NeXT_Xray_source parameters are available as well.
```

## Examples

- **Test: ILL_NeXT_Xray_porous.instr -y Detector: Xray_counts_I=1.77676e+11**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SOD | m | Source - object (rotation axis) distance | 0.05 |
| pores | str | Pore mesh file (Tests_samples/Test_PowderN_porous/make_pores.py), "NULL" for none | "pores.off" |
| fill_y | m | Water level in the pores, local y (sample spans +-0.005) | 0 |
| water | 1 | 1: water below fill_y, 0: void pores | 1 |

## Links

- [Source code](ILL_NeXT_Xray_porous.instr) for `ILL_NeXT_Xray_porous.instr`.

---
