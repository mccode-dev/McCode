# The `ILL_H53` Instrument

*McStas: The H53 curved cold guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** April 7, 2004

## Description

```text
The H53 curved cold guide at the ILL feeding IN14, IN16, D16, ADAM, CRYO-EDM
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| m | 1 | m-value of whole guide coating. 0 absorbing, 1 for Ni, 1.2 for Ni58, 2-4 for SM | 1.2 |
| lambda | AA | central wavelength | 10 |
| dlambda | AA | wavelength half width. Use e.g. 0.8*lambda for white beam | 9.9 |
| mip | 1 | m-value of in-pile guide coating | 1.2 |

## Links

- [Source code](ILL_H53.instr) for `ILL_H53.instr`.

---
