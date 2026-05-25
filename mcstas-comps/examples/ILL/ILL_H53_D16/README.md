# The `ILL_H53_D16` Instrument

*McStas: The D16 diffractometer/reflectometer on the H53 curved cold guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** Oct 7, 2008

## Description

```text
The H53 (120 x 60 mm2) curved cold guide feeds D16 (after IN14, and IN16).
D16 is a two-circle diffractometer. The primary white beam is reflected by a
focussing pyrolytic graphite monochromator (122 x 60 mm2 with mosaicity 0.7
deg) providing an important flux at the sample. The monochromator housing has
two beam holes at take-off angles of 90 deg and 115 deg, corresponding to 4.7
Angs and 5.6 Angs beams and incorporates the slit systems.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator, computed from DM and THETA_M if left as 0. | 4.7 |
| DM | Angs | d-spacing of monochromator, computed from lambda and THETA_M if left as 0. | 3.355 |
| dlambda | AA | wavelength half width. | 0.05 |
| Powder | str | File name for powder description. | "Na2Ca3Al2F14.laz" |
| RV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| L1 | m | Guide-Monochromator distance | 0.1 |
| L2 | m | Monochromator-Sample distance | 2.8 |
| L3 | m | Sample-Detector distance | 1.0 |
| THETA_M | deg | Monochromator take-off angle, computed from lambda and DM if left as 0. | 44.46 |
| TwoTheta | deg | Detector rotation (in plane) | 0 |
| RadiusDet | m | Detector entrance window radius | 0.36 |
| DetEthick | m | Detector entrance window thickness | .01 |
| DetEgap | m | Detector entrance window gap to flat detector inner window | 0.08 |
| DetVthick | m | Detector active volume thickness | 5e-3 |
| verbose | 1 | Print DIF configuration. 0 to be quiet | 1 |
| TILT | deg | Monochromator additional tilt, for rocking curves | 0 |

## Links

- [Source code](ILL_H53_D16.instr) for `ILL_H53_D16.instr`.
- The <a href="http://www.ill.fr/d16">D16 diffractometer</a> at the ILL

---
