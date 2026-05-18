# The `SOLEIL_DISCO` Instrument

*McXtrace: DISCO beam-line at SOLEIL, imaging branch*

## Identification

- **Site:** SOLEIL
- **Author:** Stephane Bac, Emmanuel Farhi, Antoine Padovani
- **Origin:** SOLEIL
- **Date:** 01/08/2022

## Description

```text
DISCO beamline description here: https://hal.archives-ouvertes.fr/hal-01479318/document
You may scan the grating monochromator with e.g.:
mxrun -n 1e5 SOLEIL_DISCO.instr -N84 x_screw=20,103 scan=1
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| grazing_angle_one | deg | Mirrors M1/M2 rotation angle | 22.5 |
| HFP_shift | m | z translation from the HFP position (horizontal focal point detector) | 0 |
| VFP_shift | m | z translation from the VFP position (vertical focal point detector) | 0 |
| second_HFP_shift | m | z translation second HFP position | 0 |
| x_screw | mm | length of the screw | 50 |
| scan |  | if there is an x_screw scan [1], calculate the Wavelength(Angstroms) as a function of x_screw(mm) | 0 |

## Links

- [Source code](SOLEIL_DISCO.instr) for `SOLEIL_DISCO.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/disco

---
