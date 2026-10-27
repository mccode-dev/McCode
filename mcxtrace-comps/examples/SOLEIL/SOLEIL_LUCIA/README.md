# The `SOLEIL_LUCIA` Instrument

*McXtrace: A simple model for LUCIA at SOLEIL (fluorescence).*

## Identification

- **Site:** SOLEIL
- **Author:** E. Farhi
- **Origin:** SOLEIL
- **Date:** 2023

## Description

```text
The HU52 undulator, a DCM, a fluorescence sample, and monitors.
This model does not contain any focusing mirror.
The sample is a kind of biological cell.

Position | Element
---------|--------------------------------------------------------------------
0        | HU52 undulator "Apple II" type, 32 periods, gap 15-150mm
20       | Si(111) DCM, 40x40x10 mm^3 LUCIA_E0=5-16 keV
29       | Sample location
29.1     | Fluorescence Detector at 90 deg
```

## Examples

- **Test: -n 1e7 LUCIA_E0=3.09 Detector: LUCIA_mon_spl_SDD_I=9.69081e+09**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| LUCIA_E0 | keV | Nominal energy at the source, in 0.6-8 keV. | 3.09 |
| LUCIA_dE | keV | Energy half-bandwidth at the source. | 0.02 |
| LUCIA_dcm_theta | deg | Rotation angle of the DCM. 0=set from energy LUCIA_E0. | 38.9 |
| LUCIA_sample_material | str | Absorption data file for the sample. | "CaCO3AlP" |
| LUCIA_sample_geometry | str | OFF/PLY file name for sample 3D geometry, or NULL for a box. | "plant_cell.ply" |
| splits |  |  | 10 |

## Links

- [Source code](SOLEIL_LUCIA.instr) for `SOLEIL_LUCIA.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/lucia

---
