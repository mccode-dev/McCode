# The `SOLEIL_DIFFABS` Instrument

*McXtrace: SOLEIL DIFFABS Combining X‐ray diffraction and absorption to study a large variety of materials*

## Identification

- **Site:** SOLEIL
- **Author:** E. Farhi and D. Thiaudiere
- **Origin:** SOLEIL
- **Date:** May 2023

## Description

```text
This is model of the DIFFABS beam-line at synchrotron SOLEIL, combining diffraction and absorption.

The range of instrumental techniques that can be used on this line concerns
numerous sectors of fundamental research and finalized applied research (oil
industry, nuclear field , metallurgy) among which the science of materials and
chemistry hold a predominant position. In particular, in situ studies of the
transformations in materials at ultra-high temperature will be the domain of
excellence of this line. The interest of coupled absorption, or coupled -
diffraction measurements on powders or monocristalssingle crystals, is to ensure
that both experiments are carried out on the same zone of the sample, in
absolutely identical physico-chemical conditions (temperature, pressure,
reactive atmosphere around the sample), which is very important for establishing
correlations between the information provided by both types of measurements in
the case of complex materials or materials under extreme conditions.

Position | Element
---------|--------------------------------------------------------------------
0        | the BM D13-1 Bender B=1.71 T in range 3-23 keV, e-beam cross-section 55.1x20.6 µm2 123.7x1.6 µrad. Other specs give 60.1x24.9 µm and 134.8x2.1 µrad. Critical energy 8.6 keV.
11.85    | primary slit
14.92    | M1 bent mirror with Rh/Si coating. Focuses vertically the beam on the DCM. length 1300 mm, width 100 mm. 2.4 mrad incidence above 19 keV, 3 mrad in 12.5-19 keV, and 5 mrad in 6.5-12.5 keV, and 6 mrad below.
17.462   | a Si(111) DCM. First crystal is 200x100 mm flat at 2.54 m from M1; 2nd is bent 70x100 mm to focus the beam horizontally.
19.28    | M2 bent mirror with Rh coating, 1300x100 mm. Focuses vertically the beam. At 1.817 from DCM.
27.752   | secondary slits when using the KB. We ignore them here.
31.227   | a KB mirror set M3/M4 for micro focusing. We ignore them here.
31.45    | sample stage with e.g. Fluorescence and/or PowderN components. At 13.99 from DCM.
32.09    | a set of detectors (transmission, XRD and XRF), radius 654 mm
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central energy of the interval to be looked at | 13 |
| dE | keV | Half-width     of the energy interval | 1 |
| M1_angle | mrad | Rotation angle of M1/M2 mirrors. When left as 0, it is set automatically from E0. | 0 |
| M1_radius | m | Curvature radius of M1 mirror (Rh, 1300x100) longitudinal. Positive=mirror is focusing. 0=flat. | 1400 |
| M2_radius | m | Curvature radius of M2 mirror (Rh, 1300x100) sagittal. Positive=mirror is focusing. 0=flat. | 1400 |
| DCM_theta | deg | Rotation angle of DCM crystals. When left as 0, it is set automatically from E0. | 0 |
| sample | str | Sample structure file, LAU/CIF format. | "LaB6_660b_AVID2.hkl" |

## Links

- [Source code](SOLEIL_DIFFABS.instr) for `SOLEIL_DIFFABS.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/diffabs
- Gallard, PhD, (2019) https://www.theses.fr/2019AIXM0037.pdf

---
