# The `ILL_H142` Instrument

*McStas: The H142 S-curved cold guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** April 7, 2004

## Description

```text
The H142 S-curved cold guide at the ILL feeding IN12
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| m | 1 | m-value of whole guide coating. 0 absorbing, 1 for Ni, 1.2 for Ni58, 2-4 for SM | 1 |
| lambda | AA | central wavelength | 10 |
| dlambda | AA | wavelength half width. Use e.g. 0.8*lambda for white beam. | 9.9 |
| gH | m | height of 2nd H142 curved section | 0.12 |
| mip | 1 | m-value of in-pile guide coating | 1 |

## Links

- [Source code](ILL_H142.instr) for `ILL_H142.instr`.
- The <a href="http://www.ill.fr/YellowBook/IN12">IN12 TAS</a> at the ILL

---
