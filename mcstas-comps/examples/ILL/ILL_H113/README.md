# The `ILL_H113` Instrument

*McStas: The H113 supermirror ballistic curved cold guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** April 7, 2004

## Description

```text
The H113 supermirror ballistic curved cold guide at the ILL feeding PF1b
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| m | 1 | m-value of whole guide coating. 0 absorbing, 1 for Ni, 1.2 for Ni58, 2-4 for SM | 2 |
| lambda | AA | central wavelength | 10 |
| dlambda | AA | wavelength half width. Use e.g. 0.8*lambda for white beam. | 9.9 |
| mip | 1 | m-value of in-pile guide coating (3.5 first meters of guide) | 1.2 |

## Links

- [Source code](ILL_H113.instr) for `ILL_H113.instr`.
- The <a href="http://www.ill.fr/YellowBook/PF1">PF1b beam line</a> at the ILL

---
