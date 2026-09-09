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

## Examples

- **Test: Test_Sources.instr SRC=0 Detector: emon_I=1.99918**
- **Test: Test_Sources.instr SRC=1 Detector: psd_I=1.59652**
- **Test: Test_Sources.instr SRC=2 Detector: lmon_I=7.95755e-06**
- **Test: Test_Sources.instr SRC=3 Detector: emon_I=7.95649e-06**
- **Test: Test_Sources.instr SRC=4 Detector: lmon_I=7.95748e-06**
- **Test: Test_Sources.instr SRC=5 Detector: psd_I=7.95748e-06**
- **Test: Test_Sources.instr SRC=6 Detector: emon_I=1**
- **Test: Test_Sources.instr SRC=7 Detector: lmon_I=0.993603**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SRC |  | Integer parameter picks a source model. | 0 |

## Links

- [Source code](Test_Sources.instr) for `Test_Sources.instr`.

---
