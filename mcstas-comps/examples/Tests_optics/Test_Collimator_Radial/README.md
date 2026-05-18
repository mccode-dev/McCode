# The `Test_Collimator_Radial` Instrument

*McStas: Cross comparison of radial collimator components*

## Identification

- **Site:** Tests_optics
- **Author:** E. Farhi [farhi@ill.fr]
- **Origin:** ILL
- **Date:** Sept 1st, 2008

## Description

```text
Cross comparison of radial collimator components, using McStas and
contributed components. It shows that all implementations are equivalent.
The Exact_radial_coll contributed component also takes into account the absorbing
blade thickness between slits, which decreases slightly intensity.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Collimator | 1 | Choice of radial collimator component to test, with 1=Collimator_Radial, 2=Collimator_ROC, 3=Exact_radial_coll. | 1 |
| Powder | string | Reflection list for powder-sample | "Na2Ca3Al2F14.laz" |

## Links

- [Source code](Test_Collimator_Radial.instr) for `Test_Collimator_Radial.instr`.

---
