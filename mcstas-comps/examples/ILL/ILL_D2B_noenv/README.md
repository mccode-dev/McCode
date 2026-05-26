# The `ILL_D2B_noenv` Instrument

*McStas: Simple monochromator Diffractometer for powders*

## Identification

- **Site:** ILL
- **Author:** C. M. I Enrique, K. Lieutenant, E. Farhi and L. Cussen
- **Origin:** ILL
- **Date:** 13 Apr 2006

## Description

```text
The diffractometer D2B is characterised by the very high take-off angle (135 deg)
for the monochromator, which has a relatively large mosaic spread of 20' to
compensate for the corresponding intensity (dl/l) loss. It is 300 mm high,
focusing vertically onto about 50 mm; this large incident vertical divergence
is matched by 200 mm high detectors and collimators. A complete diffraction
pattern is obtained after about 100 steps of 0.025 deg in 2theta, since the 64
detectors are spaced at 2.5 deg intervals. Such scans take typically 30 minutes;
they are repeated to improve statistics.

D2B was designed for work on samples and high resolution of very large
d-spacings using wavelengths of between 2.4 Å and 6 Å. Wavelengths can easily be
changed under computer control, since they are all obtained by a simple rotation
within the Ge[hhl] plane. A large graphite filter can be switched in to provide
a very clean beam at 2.4 Angs, and a cold Be-filter can be used for longer
wavelengths.

This model implements as well the Caglioti UVW equations, that give estimates
of the instrument resolution.

Monochromator lattice parameter
Ge       111 DM=3.266 AA
Ge       311 DM=1.714 AA
Ge       511 DM=1.089 AA
Ge       533 DM=0.863 AA
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator, computed from DM and THETA_M if left as 0. | 1.594 |
| DM | Angs | d-spacing of monochromator, computed from lambda and THETA_M if left as 0. | 0 |
| Powder | str | File name for powder description | "Na2Ca3Al2F14.laz" |
| RV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| L1 | m | Source-Monochromator distance | 16.05 |
| L2 | m | Monochromator-Sample distance | 2.645 |
| L3 | m | Sample-Detector distance | 1.3 |
| verbose | 1 | Print DIF configuration. 0 to be quiet | 1 |
| ALPHA1 | min | Horizontal collimator divergence for L1 arm (before monochromator) | 18 |
| ALPHA2 | min | Horizontal collimator divergence for L2 arm (monochromator-sample) | 11 |
| ALPHA3 | min | Horizontal collimator divergence for L3 arm (sample-detector) | 5 |
| ETA | min | Monochromator horizontal mosaic (gaussian) | 12 |
| THETA_M | deg | Monochromator take-off angle, computed from lambda and DM if left as 0. | 67.5 |
| TILT | deg | Monochromator additional tilt, for rocking curves | 0 |
| SM | 1 | Scattering sense of beam from Monochromator. 1:left, -1:right | 1 |
| Dheight | m | Banana detector height | 0.3 |
| coating | 1 | Super-mirror in-beam tube guide coating | 0 |

## Links

- [Source code](ILL_D2B_noenv.instr) for `ILL_D2B_noenv.instr`.
- G. Caglioti, A. Paoletti, F.P. Ricci, Nucl. Instr. and. Meth. 3 (1958) 223
- L.D. Cussen, Nucl. Instr. and. Meth. A 554 (2005) 406
- M. Morhac, NIM A 600 (2009) 478

---
