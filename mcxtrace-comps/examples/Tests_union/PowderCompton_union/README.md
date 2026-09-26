# The `PowderCompton_union` Instrument

*McXtrace: An example using Union with Compton scattering*

## Identification

- **Site:** Templates
- **Author:** E. B. Knudsen (erkn@united-neux.eu)
- **Origin:** United Neux
- **Date:** Aug 13th 2021

## Description

```text
An example instrument showing the union system with a sample geometry consisting
of a sphere and a box
```

## Examples

- **Test: E0=12 refs=Al2O3_Corundum_COD_9007634.cif.hkl Detector: fpi_I=6.55324e-10**
- **Test: E0=12 refs=Pb_Lead_COD_9008477.cif.hkl Detector: fpi_I=3.4529e-10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | incident energy | 12 |
| refs | string | Reflection-list for powder-process | "Al2O3_Corundum_COD_9007634.cif.hkl" |
| YY | m | Vertical slit-position | 0 |

## Links

- [Source code](PowderCompton_union.instr) for `PowderCompton_union.instr`.
- <reference/HTML link>

---
