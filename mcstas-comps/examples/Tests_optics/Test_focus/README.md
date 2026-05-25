# The `Test_focus` Instrument

*McStas: Focus testing, comparing Single_crystal and Monochromator_curved*

## Identification

- **Site:** Tests_optics
- **Author:** Tobias Weber (tweber@ill.fr)
- **Origin:** ILL
- **Date:** 3-Feb-2018

## Description

```text
For mono_ideal=0 the Single_crystal component is used as monochromator,
for mono_ideal=1 Monochromator_curved is used with the same settings.

The curvature of the monochromator can be set using mono_curvh and mono_curvv
for horizontal and vertical focusing, respectively.
For mono_curvh=0, mono_curvv=0 the monochromator is flat.
For mono_curvh=-1, mono_curvv=-1 optimal horizontal and vertical focusing is chosen.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| src_lam | AA | Source mean wavelength | 4.5 |
| src_dlam | AA | Source wavelength spread | 1.0 |
| mono_ideal |  | Selection of mono-model 0=Single_crystal, 1=Monochromator_curved | 0 |
| mono_curvh | m | Monochromator horizontal curvature. -1: optimal, 0: flat | -1 |
| mono_curvv | m | Monochromator vertical curvature -1: optimal, 0: flat | -1 |

## Links

- [Source code](Test_focus.instr) for `Test_focus.instr`.

---
