# The `Test_Guides` Instrument

*McStas: Cross comparison of Guide components*

## Identification

- **Site:** Tests_optics
- **Author:** E. Farhi [farhi@ill.fr]
- **Origin:** ILL
- **Date:** Sept 1st, 2008

## Description

```text
Cross comparison of Guide components, using McStas and
contributed components. It shows that all implementations are equivalent,
except the Guide_honeycomb which has a different geometry.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Guide | 1 | Choice of Guide component to test, with 1=Guide, 2=Guide_channeled, 3=Guide_gravity, 4=Guide_wavy, 5=Guide_curved 6=Elliptic_guide_gravity 7=Guide_honeycomb | 1 |

## Links

- [Source code](Test_Guides.instr) for `Test_Guides.instr`.

---
