# The `ILL_H16` Instrument

*McStas: The H16 cold guide (feeding IN5)*

## Identification

- **Site:** ILL
- **Author:** E. Farhi, J. Ollivier, Celia Castan Guerrero
- **Origin:** ILL
- **Date:** Jan 2004-2009

## Description

```text
The H16@ILL curved guide sending cold neutrons from the VCS to IN5.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | mean incident wavelength. | 10 |
| dlambda | AA | wavelength half width. Use e.g. 0.8*lambda for white beam. | 9.9 |
| m | 1 | m-value of whole guide coating. 0 absorbing, 1 for Ni, 1.2 for Ni58, 2-4 for SM | 1 |

## Links

- [Source code](ILL_H16.instr) for `ILL_H16.instr`.

---
