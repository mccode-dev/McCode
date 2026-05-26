# The `ILL_H22_D1B_noenv` Instrument

*McStas: The D1B diffractometer on the H22 curved thermal guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr) and SANCHEZ Javier (sanchez-montero@ill.fr)
- **Origin:** ILL
- **Date:** June, 2008

## Description

```text
The D1B diffractometer on the H22 curved thermal guide at the ILL.

D1B is a two-axis spectrometer dedicated to diffraction experiments requesting a
high neutron flux. A great number of experiments performed on D1B concern the
determination of magnetic structures. At small angles where the magnetic peaks
are expected, a high spatial resolution can be achieved, the FWHM reaches 0.2°
(for a sample with = 8 mm). Three pyrolitic graphite monochromators focusing
onto the sample position provide a flux of 6.5·106 n cm-2s-1. A second
wavelength with = 1.28 Å is available by using a germanium monochromator. D1B is
equipped with 3He/Xe position-sensitive detector composed of a system of
multi-electrodes with 400 cells, which span a 2 range of 80°. The detector can
be moved so that an angular range of 2°<2<130° can be covered. The specially
designed cryostat is known for its low background crucial for some experiments
with small intensity changes. Because of its high flux at = 2.52 Å together with
the large multi-detector, surface studies such as adsorbed phases as well as
real-time diffraction experiments are possible. Collecting a diffraction pattern
with sufficient statistics in minutes (1-5 min) even seconds allows in situ
studies of reaction kinetics. A fast detection of phase transitions can also be
obtained by scanning the temperature. A complete thermal variation of the
diffraction patterns (1.5 - 300 K) can be achieved in few hours (3-5h).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | mean incident wavelength. | 2.52 |
| dlambda | AA | wavelength full width. | 0.03 |
| DM | AA | Mono lattice spacing | 0 |
| Powder | str | File name for powder description, LAZ/LAU/Fullprof | "Na2Ca3Al2F14.laz" |
| RV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | 2.2 |
| L1 | m | Source-Monochromator distance | 0.25 |
| L2 | m | Monochromator-Sample distance | 3.0 |
| L3 | m | Sample-Detector distance | 1.500 |
| TRAS_X | m | Additional monochromator translation along X, left/righ w.r.t beam | -0 |
| TRAS_Z | m | Additional monochromator translation along Z, along guide | 0 |
| TILT | deg | Monochromator additional tilt, for rocking curves | 0 |
| THETA_M | deg | Mono takeoff angle | 22.11 |
| R_pitch | deg | Angular pitch between the absorbing blades | .42 |
| R_ri | m | Inner radius of the collimator | 0.324 |
| R_ro | m | Outer radius of the collimator | 0.419 |
| R_h | m | Height of the collimator | 0.090 |
| R_ttmin | deg | Lower scattering angle limit | -130 |
| R_ttmax | deg | Higher scattering angle limit | -2 |
| R_present | 1 | Presence flag for the radial collimator. 0: inactivate component. | 1 |
| verbose | 1 | Print DIF configuration. 0 to be quiet | 1 |
| Inc_Cryo | 1 | Cryostat incoherent fraction | 0.02 |
| Trans_Cryo | 1 | Cryostat event transmission | 0.85 |
| Trans_Spl | 1 | Sample event transmission | 0.2 |
| Inc_Spl | 1 | Sample incoherent fraction | 0.05 |

## Links

- [Source code](ILL_H22_D1B_noenv.instr) for `ILL_H22_D1B_noenv.instr`.

---
