# The `Test_CRL` Instrument

*McXtrace: Unit test instrument for various lens components.*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Nov. 14

## Description

```text
This is a test with the
- LENS=0: Lens_parab
- LENS=1: Lens_parab_Cyl
- LENS=2: Lens_simple
- LENS=3: Lens_CRL_RTM
```

## Examples

- **Test: Test_CRL.instr LENS=0 L1=35.047 L2=17.523 Detector: line_I=1.20107e-12**
- **Test: Test_CRL.instr LENS=1 L1=35.047 L2=17.523 Detector: pt_I=3.01094e-13**
- **Test: Test_CRL.instr LENS=2 L1=35.047 L2=17.523 Detector: line_I=1.2957e-12**
- **Test: Test_CRL.instr LENS=3 L1=35.047 L2=17.523 Detector: line_I=1.20107e-12**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| LENS |  | Which lens to use: 0=>Lens_parab, 1=>Lens_parab_Cyl, 2=>Lens_simple | 0 |
| L1 |  | Distance from source to lens. | 1 |
| L2 |  | Distance from lens to image plane | 11 |

## Links

- [Source code](Test_CRL.instr) for `Test_CRL.instr`.

---
