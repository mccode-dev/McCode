# The `SOLEIL_PSICHE` Instrument

*McXtrace: A simple model of the PSICHE tomography/diffraction beam-line at Synchrotron SOLEIL.*

## Identification

- **Site:** SOLEIL
- **Author:** Emmanuel FARHI <emmanuel.farhi@synchrotron-soleil.fr>
- **Origin:** SOLEIL
- **Date:** Apr 26th 2023

## Description

```text
The PSICHÉ beamline is installed on a short straight section of the SOLEIL
(I03c), The source is a under vacuum multi-pole wiggler (2.1 T) which delivers
a white beam with a large photon energy range (15-100+ keV).
To perform the various experiments performed on the beamline, 4 different
operating modes are available.

This model allows the white beam mode for energy dispersive x-ray diffraction,
and the monochromatic beam tomography.

The sample handles absorption, with edge contrast, as well as fluorescence.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Nominal energy at the Wiggler. Sets the monochromator angle when DCM_present | 31 |
| dE | keV | Energy half-bandwidth at the Wiggler | 1 |
| DCM_present | 1 | Allow the monochromator in place (1) or removed (0, white beam) | 0 |
| sample_theta | deg | Rotation of the sample stage | 0 |
| sample_material | str | Absorption data file for the sample | "Ag" |
| sample_geometry | str | OFF/PLY file name for sample 3D geometry, or emty for a box | "wire.ply" |
| dcm_theta | deg | Rotation angle of the DCM. 0=set from energy E0 | 0 |

## Links

- [Source code](SOLEIL_PSICHE.instr) for `SOLEIL_PSICHE.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/psiche
- A. King et al, Rev Sci Instrum 87, 093704 (2016), DOI: 10.1063/1.4961365

---
