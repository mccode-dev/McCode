# The `SOLEIL_SIXS` Instrument

*McXtrace: A simple model for the SIXS beam-line at SOLEIL (surface diffraction).*

## Identification

- **Site:** SOLEIL
- **Author:** E. Farhi, A. Resta, A. Coati
- **Origin:** SOLEIL
- **Date:** 2023

## Description

```text
SixS (Surface Interface X-ray Scattering) is a beamline dedicated to the study
of X-ray scattering from surfaces and interfaces of hard and soft matter in
various environments in the 5-20 keV energy range. To be sensitive to the
surface all the studies are performed in grazing-incidence geometry. The
beamline is equipped with two experimental hutches, which ae dedicated to the
study of surfaces, interfaces and nano-objets prepared:

- in-situ under UHV (Ultra High Vacuum, i.e. 10-10 mbar) conditions. A
diffractometer allows to measure X-ray scattering from samples under UHV;
- in various environments (catalysis chambers, soft matter, electrochemical
cells). A diffractometer coupled with exchangeable chambers will be able to
measure X-ray scattering from sample surfaces both in vertical or horizontal
geometries.

Grazing Incidence X-ray Diffraction (GIXD), Grazing Incidence Small Angle
X-ray Scattering (GISAXS), anomalous surface X-ray scattering, X-Ray
Reflectivity (XRR), magnetic surface X-ray scattering and coherent scattering
experiments are performed on both the facilities.

In this implementation, the multipurpose diffractometer is used, with a thin
single crystal layer on top of a Si bulk.

Position | Element
---------|--------------------------------------------------------------------
0        | the U20 undulator
15       | Slit S1 2x0.7 mm^2
16.5     | a Si(111) DCM, 40x40x10 mm^3 E0=5-20 keV
26       | Mirror M1 Si coated with Pd, elliptically vertical focusing 20x350x20 mm^3 Incident angle 0.175 deg (3 mrad)
28       | Mirror M2 at 20x550x20mm^3. (tilted 45 deg exchange horz/vert components for UHV)
33       | multipurpose diffractometer
42       | UHV diffractometer (not modelled here)

Example: E0=10 Detector: mon_spl_fluo_I=3.66859e+14
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Nominal energy at the Wiggler. | 13 |
| dE | keV | Energy half-bandwidth at the Wiggler | 0.1 |
| dcm_theta | deg | Rotation angle of the DCM. 0=set from energy E0 | 0 |
| M1_angle | mrad | Rotation angle of M1/M2 mirrors. When left as 0, it is set automatically from E0. | 3 |
| M1_radius | m | Curvature radius of M1 mirror (Rh, 1300x100) longitudinal. Positive=mirror is focusing. 0=flat. | 1400 |
| M2_radius | m | Curvature radius of M2 mirror (Rh, 1300x100) sagittal. Positive=mirror is focusing. 0=flat. | 1400 |
| sample_coating | str | Reflection/structure data file for the single crystal thin layer coating | "Mo.lau" |
| sample_bulk | str | Reflection/structure data file for the single crystal bulk | "Si.lau" |
| sample_thickness | m | The single crystal thin layer coating thickness | 50e-10 |
| sample_angle | deg | The sample tilt angle (around X axis) | 0.175 |

## Links

- [Source code](SOLEIL_SIXS.instr) for `SOLEIL_SIXS.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/sixs

---
