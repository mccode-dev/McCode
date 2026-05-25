# The `Test_Sources` Instrument

*McXtrace: Test instrument to show that the source components work*

## Identification

- **Site:** Tests_sources
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Fysik
- **Date:** Feb 1st 2013

## Description

```text
This instrument is a unit test for the source components.

The following sources are handled:
Source_gaussian SRC=0 (with gauss=0)
Source_gaussian SRC=1
Source_pt       SRC=2
Source_pt       SRC=3
Source_flat     SRC=4
Source_flat     SRC=5
Source_div      SRC=6
Source_div      SRC=7
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SRC |  | Integer parameter picks a source model. | 0 |

## Links

- [Source code](Test_Sources.instr) for `Test_Sources.instr`.

---
