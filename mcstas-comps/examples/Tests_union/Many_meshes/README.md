# The `Many_meshes` Instrument

*McStas: Instrument testing placement of overlapping mesh components.*

## Identification

- **Site:** Tests_union
- **Author:** Daniel Lomholt Christensen
- **Origin:** UCPH@NBI, Funded by ACTNXT
- **Date:** 15:54:48 on January 20, 2026

## Description

```text
A small test instrument testing the placement of many overlapping
mesh components.
Test with:
mcdisplay -y -c
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
| crack_width | m | Height masking crack should be raised from the original crack | 0.003 |

## Links

- [Source code](Many_meshes.instr) for `Many_meshes.instr`.

---
