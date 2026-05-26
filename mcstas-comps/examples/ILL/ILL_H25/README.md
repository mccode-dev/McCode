# The `ILL_H25` Instrument

*McStas: The H25 supermirror curved thermal guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 2006

## Description

```text
The H25 supermirror curved thermal guide at the ILL feeding S18, D23 and IN22
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

- [Source code](ILL_H25.instr) for `ILL_H25.instr`.

---
