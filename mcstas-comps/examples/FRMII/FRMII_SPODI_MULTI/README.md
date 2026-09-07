# The `FRMII_SPODI_MULTI` Instrument

*McStas: Simple monochromator diffractometer for powders*

## Identification

- **Site:** FRMII
- **Author:** V. Kochetov, C. Hauf, M. Hoelzel, A. Senyshyn
- **Origin:** MLZ, Garching, Germany
- **Date:** August 24, 2026

## Description

```text
SPODI is a high-resolution thermal neutron diffractometer at the research
reactor Heinz Maier-Leibnitz (FRM II) especially dedicated to structural
studies of complex systems. Unique features like a very large monochromator
take-off angle of 155 degrees and a 5 m monochromator-sample distance in its
standard configuration achieve both high-resolution and a good profile shape
for a broad scattering angle range of 160 degrees. Two-dimensional data are
collected by an array of 80 vertical position sensitive 3He detectors
typically during 40 resolution steps of 0.05 degrees.

List of available wavelengths in A delivered by germanium monochromator:
Reflection    Take-off 155 degrees, L2=5m      Take-off 135 degrees, L2=2.8m
Ge 331            2.536 A                           2.396 A
Ge 551            1.549 A                           1.463 A
Ge 771            1.111 A                           1.050 A

The implemented model corresponds to the standard setup with the take-off angle
of 155 degrees, Ge 551 reflection giving rise to the wavelength of ~1.5482 AA,
and L2 of 5 m.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | A | Wavelength; if 0, calculated from HKLmono and TOA | 1.5482 |
| HKLmono | str | Miller indices of the Ge-monochromator reflection | "551" |
| TOA | deg | Take-Off Angle; if 0, calculated from HKLmono and lambda | 0 |
| L2 | m | Distance from the monochromator to the sample | 5.0 |
| Dsample | m | Sample diameter | 8e-3 |
| Hsample | m | Sample height | 40e-3 |
| Powder | str | LAZ/HKL/PCR/CIF file for powder description | "Na2Ca3Al2F14.laz" |
| Holder |  | Flag to enable (1) / disable (0) the vanadium sample holder | 0 |
| StartTheta | deg | Start angle of the detector system relative to the beam | 0 |
| ResStep | deg | Resolution step in degrees (standard is 0.05 deg) | 0.05 |
| iResStep | deg | Index number of resolution step (between 0 and 39 for standard 0.05 deg) | 0 |

## Links

- [Source code](FRMII_SPODI_MULTI.instr) for `FRMII_SPODI_MULTI.instr`.
- M. Hoelzel, A. Senyshyn, N. Juenke, H. Boysen, W. Schmahl and H. Fuess, Nucl. Instr. and Meth. in Ph A 667 (2012) 32-37

---
