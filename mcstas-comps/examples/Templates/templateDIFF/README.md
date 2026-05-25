# The `templateDIFF` Instrument

*McStas: Simple monochromator Diffractometer for powders*

## Identification

- **Site:** Templates
- **Author:** E. Farhi
- **Origin:** LLB/ILL
- **Date:** 13 Apr 2006

## Description

```text
A template powder diffractometer used as a tutorial at LLB.
Default geometry is from D20@ILL.

If included after a guide, use ALPHA1=0 and L1=0 (the first collimator is then
the guide with effective ALPHA1=m*0.1*lambda). If you prefer to include
explicitely a collimator before the monochromator, use L1=1.2.

Monochromator lattice parameter
PG       002 DM=3.355 AA (Highly Oriented Pyrolythic Graphite)
PG       004 DM=1.607 AA
Heusler  111 DM=3.362 AA (Cu2MnAl)
CoFe         DM=1.771 AA (Co0.92Fe0.08)
Ge       111 DM=3.266 AA
Ge       311 DM=1.714 AA
Ge       511 DM=1.089 AA
Ge       533 DM=0.863 AA
Si       111 DM=3.135 AA
Cu       111 DM=2.087 AA
Cu       002 DM=1.807 AA
Cu       220 DM=1.278 AA
Cu       111 DM=2.095 AA
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator, computed from DM and THETA_M if left as 0. | 1 |
| DM | Angs | d-spacing of monochromator, computed from lambda and THETA_M if left as 0. | 3.355 |
| Powder | str | File name for powder description | "Na2Ca3Al2F14.laz" |
| RV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| L1 | m | Source-Monochromator distance | 17 |
| L2 | m | Monochromator-Sample distance | 3.2 |
| L3 | m | Sample-Detector distance | 1.471 |
| ALPHA1 | min | Horizontal collimator divergence for L1 arm (before monochromator) | 5 |
| ALPHA2 | min | Horizontal collimator divergence for L2 arm (monochromator-sample) | 60 |
| ALPHA3 | min | Horizontal collimator divergence for L3 arm (sample-detector) | 5 |
| ETA | min | Monochromator horizontal mosaic (gaussian) | 12 |
| verbose | int | Print DIF configuration. 0 to be quiet | 1 |
| THETA_M | deg | Monochromator take-off angle, computed from lambda and DM if left as 0. | 0 |
| SM | int | Scattering sense of beam from Monochromator. 1:left, -1:right | 1 |

## Links

- [Source code](templateDIFF.instr) for `templateDIFF.instr`.

---
