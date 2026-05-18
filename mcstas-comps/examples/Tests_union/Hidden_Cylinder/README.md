# The `Hidden_Cylinder` Instrument

*McStas: Small test instrument testing placement of low-priority Union cylinder within higher-priority Union cylinder.*

## Identification

- **Site:** Tests_union
- **Author:** Daniel Lomholt Christensen
- **Origin:** UCPH@NBI, Funded by ACTNXT
- **Date:** 15:54:48 on January 20, 2026

## Description

```text
A small test instrument testing the placement of one cylinder with a low priority
fully inside another cylinder with a higher priority. This was added to test
for an mcdisplay error that used to occur, when a cylinder is completely
enclosed in another cylinder.
To test this simply do
a "mcdisplay Hidden_Cylinder.instr -c -y"
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| pin_rad | m | Radius of source | 0.0025 |
| d | m | Distance between source and sample center | 10 |
| detector_x | m | Detector Width | 0.01 |
| detector_y | m | Detector Height | 0.04 |
| det_dist | m | Distance between detector and sample center | 0.005 |

## Links

- [Source code](Hidden_Cylinder.instr) for `Hidden_Cylinder.instr`.

---
