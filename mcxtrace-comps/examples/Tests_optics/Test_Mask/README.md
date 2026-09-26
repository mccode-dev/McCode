# The `Test_Mask` Instrument

*McXtrace: A unit test instrument of the Mask component*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** 12/03/2014

## Description

```text
A non-divergent beam impinges on a masking image, with 2d-detectors up- and downstream
of the mask.
```

## Examples

- **Test: Test_Mask.instr invert=0 Detector: psd1_I=0.223645**
- **Test: Test_Mask.instr invert=1 Detector: psd1_I=0.670667**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| invert |  | If nonzero the transparency/opacity sense of the masking image is reversed. | 0 |

## Links

- [Source code](Test_Mask.instr) for `Test_Mask.instr`.

---
