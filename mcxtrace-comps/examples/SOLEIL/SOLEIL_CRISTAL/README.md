# The `SOLEIL_CRISTAL` Instrument

*McXtrace: SOLEIL CRISTAL beamline: diffraction with 2/4/6-circle diffractometers.*

## Identification

- **Site:** SOLEIL
- **Author:** FARHI Emmanuel with help of Claude (Anthropic), adapted from SOLEIL_MARS and SOLEIL_DIFFABS
- **Origin:** SOLEIL
- **Date:** September 2026

## Description

```text
CRISTAL is a diffraction beam-line covering 5-30 keV (dE/E ~1e-4), fed by an
in-vacuum U20 undulator (98 periods, 20 mm period). The optical layout, in
beam order, is: primary slit, Si(111) double-crystal monochromator (DCM,
second crystal sagittally bent for horizontal focusing - not modelled here
for simplicity), a pair of horizontally-deflecting Si mirrors (Rh/Pt coated,
first one bendable, for horizontal focusing and harmonic rejection), a
secondary slit, and finally the sample stage. Most powder-diffraction work
is done on the 2-circle diffractometer, equipped with a curved Mythen-II
pixel detector (radius 720 mm). Only components that act on the beam are
modelled; hutch diagnostics such as xbpm, beam imagers and attenuators are
omitted for simplicity.
```

## Examples

- **Test: E0=12 Detector: detector_diffraction_I=700000.0**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central energy of the beam | 12 |
| dE | keV | Energy half-bandwidth at the photon source | 1e-4 |
| M1_angle | mrad | Grazing angle of the first (bendable) horizontal mirror | 3 |
| M2_angle | mrad | Grazing angle of the second horizontal mirror | 3 |
| M1_radius | m | Bending radius of mirror M1 (0=flat) | 0 |
| M2_radius | m | Bending radius of mirror M2 (0=flat) | 0 |
| M_coating | str | Mirror coating (Rh or Pt stripe), e.g. "Rh.txt" or "Pt.dat" | "Rh.txt" |
| reflections | str | Sample structure file, LAU/CIF format | "LaB6_660b_AVID2.hkl" |

## Links

- [Source code](SOLEIL_CRISTAL.instr) for `SOLEIL_CRISTAL.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/cristal

---
