# The `Test_Source_quasi` Instrument

*McXtrace: Test instrument to show that the quasi-stcohastic source component works*

## Identification

- **Site:** Tests_sources
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Fysik
- **Date:** Feb 1st 2013

## Description

```text
This instrument is a unit test for the quasi-stochastic source component.
```

## Examples

- **Test: Test_Source_quasi.instr SRC=0 Detector: psd_I=1**
- **Test: Test_Source_quasi.instr SRC=1 Detector: psd_I=1**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SRC |  | Integer parameter picks a source model. 0=normal Source_div, 1=Quasi-stochastic | 1 |

## Links

- [Source code](Test_Source_quasi.instr) for `Test_Source_quasi.instr`.

---
