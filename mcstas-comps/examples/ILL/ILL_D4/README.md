# The `ILL_D4` Instrument

*McStas: D4 Diffractometer for liquids at the ILL.*

## Identification

- **Site:** ILL
- **Author:** E. Farhi
- **Origin:** LLB/ILL
- **Date:** 13 Apr 2006

## Description

```text
D4 Diffractometer for liquids at the ILL, installed on the hot source.

The dedicated liquid and amorphous instrument D4 is a two-axis diffractometer
equipped with nine 1D position sensitive microstrip detectors. In this model,
a PSD banana detector is also present.

The monochromator take-off angle is 2Theta_M ~ 20-25 deg, variable. The available
monochromators are all vertically focusing, Cu (200) for 0.7 Angs, Cu (220)
for 0.5 Angs.

Cu       002 DM=1.807 AA
Cu       220 DM=1.278 AA
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator | 0.7 |
| DM | Angs | d-spacing of monochromator | 1.807 |
| sample | str | File name for powder/liquid description LAU/LAZ/qSq/Sqw | "SiO2_liq.qSq" |
| RV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| L1 | m | Source-Monochromator distance | 6.4 |
| L2 | m | Monochromator-Sample distance | 2.61 |
| L3 | m | Sample-Detector distance | 1.148 |
| verbose | 1 | Print DIF configuration. 0 to be quiet | 1 |

## Links

- [Source code](ILL_D4.instr) for `ILL_D4.instr`.
- The <a href="http://www.ill.eu/d4">D4 diffractometer</a> at the ILL

---
