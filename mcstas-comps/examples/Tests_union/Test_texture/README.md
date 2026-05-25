# The `Test_texture` Instrument

*McStas: Simple test instrument for Texture functionality in Union framework.*

## Identification

- **Site:** Tests_union
- **Author:** 
- **Origin:** 
- **Date:** 

## Description

```text
Simple test instrument for texture sample component.

Example: lmax=0 Detector: monitor_I=8.08899e-05
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lmax | AA | Maximum wavelength treated by texture process | 0 |
| crystal_fn | string | Crystal structure file name | "Zr.laz" |
| fcoef_fn | string | Fourier component file name | "coef_Four_L2.txt" |
| barns |  | Flag to indicate if \|F 2\| from "crystal_fn" is in barns or fm2 (barns = 1 for laz, barns = 0 for lau type files). | 1 |

## Links

- [Source code](Test_texture.instr) for `Test_texture.instr`.

---
