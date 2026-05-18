# The `ILL_H22` Instrument

*McStas: The H22 curved thermal guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** April 7, 2004

## Description

```text
The H22 curved thermal guide at the ILL feeding D1A/D1B, SALSA and VIVALDI
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| m | 1 | m-value of whole guide coating. 0 absorbing, 1 for Ni, 1.2 for Ni58, 2-4 for SM | 2 |
| lambda | AA | central wavelength | 4 |
| dlambda | AA | wavelength half width. Use e.g. 0.8*lambda for white beam. | 3.9 |
| mip | 1 | m-value of in-pile guide coating | 2 |

## Links

- [Source code](ILL_H22.instr) for `ILL_H22.instr`.

---
