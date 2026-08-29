# The `Test_SX_standalone` Instrument

*McXtrace: Unit-test instrument for the Single_crystal sample component.

Simply a model source illuminating a SX sample.
The default sample itself is a Mo bulk crystal.*

## Identification

- **Site:** Tests_samples
- **Author:** E. Farhi
- **Origin:** Synchrotron Soleil
- **Date:** Sept 26th 2019

## Description

```text

```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| reflections | str | List of powder reflections, LAU/CIF format. | "Mo.lau" |
| E0 | keV | Source mean energy of xrays. | 7 |
| dE | keV | Source energy half spread of x-rays. | 6.9 |

## Links

- [Source code](Test_SX_standalone.instr) for `Test_SX_standalone.instr`.

---
