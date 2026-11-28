# The `PSI_ICON_porous_Union_NCrystal` Instrument

*McStas: PSI ICON imaging beamline with a porous quartz sample (Union + NCrystal), water in the pores below fill_y*

## Identification

- **Site:** PSI
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Nov 2026

## Description

```text
As PSI_ICON_porous_Union, but matrix and water are Union_NCrystal_material:
full NCrystal physics (Bragg, incoherent, inelastic, absorption) for both.
Pore channels from pores.off (in the data folder, made with Tests_samples/Test_PowderN_porous/make_pores.py).
All PSI_ICON_beamline parameters are available as well.
```

## Examples

- **Test: PSI_ICON_porous_Union_NCrystal.instr -y Detector: VarDetectorZoom_I=750841**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| matrix | str | NCrystal cfg-string of the porous matrix | "stdlib::SiO2-alpha_sg154_AlphaQuartz.ncmat" |
| liquid | str | NCrystal cfg-string of the pore liquid | "stdlib::LiquidWaterH2O_T293.6K.ncmat" |
| pores | str | Pore mesh file, "NULL" for none | "pores.off" |
| fill_y | m | Liquid level in the pores, local y (sample spans +-0.005) | 0 |
| water | 1 | 1: liquid below fill_y, 0: void pores | 1 |

## Links

- [Source code](PSI_ICON_porous_Union_NCrystal.instr) for `PSI_ICON_porous_Union_NCrystal.instr`.

---
