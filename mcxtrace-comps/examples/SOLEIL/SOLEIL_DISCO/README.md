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
You may DISCO_scan the grating monochromator with e.g.:
```

## Examples

- **TestScan: mxrun SOLEIL_DISCO.instr -N21 DISCO_x_screw=20,103 DISCO_scan=1 Detector: Wavelength_I={21 values}**
- 
- **Test: DISCO_grazing_angle_one=22.5 -n 1e5 Detector: DISCO_second_HFP_I=2.08292e+11**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DISCO_grazing_angle_one | deg | Mirrors M1/M2 rotation angle | 22.5 |
| DISCO_HFP_shift | m | z translation from the HFP position (horizontal focal point detector) | 0 |
| DISCO_VFP_shift | m | z translation from the VFP position (vertical focal point detector) | 0 |
| DISCO_second_HFP_shift | m | z translation second HFP position | 0 |
| DISCO_x_screw | mm | length of the screw | 50 |
| DISCO_scan |  | if there is an DISCO_x_screw DISCO_scan [1], calculate the Wavelength(Angstroms) as a function of DISCO_x_screw(mm) | 0 |
| splits |  |  | 10 |

## Links

- [Source code](SOLEIL_DISCO.instr) for `SOLEIL_DISCO.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/disco

---
