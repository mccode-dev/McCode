# The `SOLEIL_SWING` Instrument

*McXtrace: A simple model for the SWING beam-line at SOLEIL (small angle scattering, SAXS).*

## Identification

- **Site:** SOLEIL
- **Author:** E. Farhi, S. Bac
- **Origin:** SOLEIL
- **Date:** 2025

## Description

```text
The SWING beamline targets soft condensed matter, conformation of
macro-molecules in solution (BioSAXS) and material sciences. Our experimental
set up allows simultaneous small-angle X-ray scattering (SAXS) and wide-angle
X-ray scattering measurements (WAXS) in the 5-16 keV energy range. Anomalous
scattering experiments can also be performed. A very large variety of types of
samples can be studied, e.g., solutions, gels, amorphous solids, crystallized
solids, thanks to the diversity of the proposed sample environments.

This model may use any PDB file as sample, or an ideal sphere colloid for testing.

Position | Element
---------|--------------------------------------------------------------------
0        | U20 undulator
11.7     | Diaphragm providing HxV=1x0.5 mm beam
20       | Si(111) DCM, 40x40x10 mm^3 E0=5-16 keV
22.5     | Focusing KB (HFM + VFM)
31       | Linear H-CRL (f=81 cm): 31 m
32       | Sample location
32.5-39  | Detector 162.5 x 155.2 mm

Example: E0=13 Detector: Eiger4M_I=7.5752e+06
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Nominal energy at the Wiggler. | 13 |
| dE | keV | Energy half-bandwidth at the Wiggler | 0.1 |
| dcm_theta | deg | Rotation angle of the DCM. 0=set from energy E0 | 0 |
| hfm_radius | m | Horizontally focusing mirror radius. | 495 |
| vfm_radius | m | Vertically focusing mirror radius. | 859 |
| mirror_grazing_angle | deg | Tilt angle of the mirrors. | 4e-3 |
| sample | str | Sample given as a PDB file, or NULL for a 100A dilute Sphere model. | "6lyz.pdb" |
| sample_det | m | Sample to detector distance in m. | 2 |

## Links

- [Source code](SOLEIL_SWING.instr) for `SOLEIL_SWING.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/swing

---
