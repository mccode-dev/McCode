# The `ILL_H5` Instrument

*McStas: The full H5 cold guide at the ILL, with IN14, D16, Super-Adam, IN15, D22
This is the geometry before the major H5 guide hall upgrade (up to 2013).*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** May, 2011

## Description

```text
This model decribes the full H5 cold guide at the ILL, with IN14, IN16, D16,
Super-Adam, IN15, D22.

The IN14 Cold neutron three-axis spectrometer IN14
The IN16 Cold neutron backscattering spectrometer IN16
simulated down to the 2nd deflector, and does not include the full
backscattering geometry.
The D16 Small momentum transfer diffractometer D16 is realistic
The SuperADAM reflectometer is used in low angle diffraction mode.
The IN15 Spin-echo spectrometer
is simulated with an incoming polarized beam, but not with a full spin-echo
description.
The D22 Large dynamic range small-angle diffractometer
is fully simulated
The CryoEDM with its polarized beam

For each instrument, a sample can be specified (liquid/powder/amorphous),
with monitoring of the scattering in angular (diffraction) and energy modes
(for spectroscopy).
```

## Examples

- **Test: lambda=5 Detector: H5_I=7.46e+13**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | central wavelength band for guide illumination | 5 |
| dlambda | AA | half width of guide wavelength band | 4.5 |
| IN14_lambda | AA | IN14 monochromator setting wavelength. Usual 2.4 and 4.2 | 4.2 |
| IN16_lambda | AA | IN16 monochromator setting wavelength. Usual 3.3 and 6.3 | 6.3 |
| D16_lambda | AA | D16  monochromator setting wavelength. Usual 4.7 and 5.6 | 5.6 |
| SADAM_lambda | AA | SuperADAM monochromator setting wavelength. Usual 4.4 | 4.4 |
| IN15_lambda | AA | IN15 velocity selector setting wavelength | 6.5 |
| D22_lambda | AA | D22  velocity selector setting wavelength | 4.5 |
| D22_collimation | m | D22 collimation length and sample-detector distance | 2 |
| IN14_sample | string | IN14 liquid/powder/amorphous sample | "Rb_liq_coh.sqw" |
| IN16_sample | string | IN16 liquid/powder/amorphous sample | "Rb_liq_coh.sqw" |
| D16_sample | string | D16  liquid/powder/amorphous sample | "H2O_liq.qSq" |
| SADAM_sample | string | SuperADAM liquid/powder/amorphous sample | "SiO2_quartza.laz" |
| D22_sample | string | D22  liquid/powder/amorphous sample | "H2O_liq.qSq" |
| IN14_RMV | m | IN14 monochromator vertical curvature radius, -1 for automatic setting | -1 |
| IN16_RMV | m | IN16 monochromator vertical curvature radius, -1 for automatic setting | -1 |
| D16_RMV | m | D16 monochromator vertical curvature radius, -1 for automatic setting | -1 |
| SADAM_RMV | m | SuperADAM monochromator vertical curvature radius, -1 for automatic setting | -1 |

## Links

- [Source code](ILL_H5.instr) for `ILL_H5.instr`.
- The NoteDPT11 at the ILL
- Daily notes from K. Andersen about the H5 project
- Mirotron drawing MR-0656-000 for the IN15 V-mirror geometry

---
