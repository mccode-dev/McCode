# The `Unittest_SPLIT_sample` Instrument

*McStas: SPLIT unittest including samples*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 28th, 2021

## Description

```text
SPLIT unittest.

One unit of intensity is emitted from a 1x1 m.
```

## Examples

- **Test: SAMPLE=1 SPLITS=1 Detector: PSD_I=1**
- **Test: SAMPLE=1 SPLITS=10 Detector: PSD_I=10**
- **Test: SAMPLE=1 SPLITS=100 Detector: PSD_I=100**
- **Test: SAMPLE=2 SPLITS=1 Detector: PSD_I=1**
- **Test: SAMPLE=2 SPLITS=10 Detector: PSD_I=10**
- **Test: SAMPLE=2 SPLITS=100 Detector: PSD_I=100**
- **Test: SAMPLE=3 SPLITS=1 Detector: PSD_I=0.999001**
- **Test: SAMPLE=3 SPLITS=1 Detector: PSD_transmission_I=0.00071068**
- **Test: SAMPLE=3 SPLITS=1 Detector: PSD_scattered_I=0.00182805**
- **Test: SAMPLE=3 SPLITS=10 Detector: PSD_I=9.99001**
- **Test: SAMPLE=3 SPLITS=10 Detector: PSD_transmission_I=0.007122**
- **Test: SAMPLE=3 SPLITS=10 Detector: PSD_scattered_I=0.0183156**
- **Test: SAMPLE=3 SPLITS=100 Detector: PSD_I=99.9001**
- **Test: SAMPLE=3 SPLITS=100 Detector: PSD_transmission_I=0.070845**
- **Test: SAMPLE=3 SPLITS=100 Detector: PSD_scattered_I=0.182056**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SAMPLE | 1 | 1=Incoherent, 2=PowderN(Al.lau), 3=Single_crystal(Al.lau) | 1 |
| SPLITS | 1 | Number of SPLIT to do | 10 |
| Pp0 | 1 | Dummy input parameter used internally for ensuring unit evennt weight | 1 |

## Links

- [Source code](Unittest_SPLIT_sample.instr) for `Unittest_SPLIT_sample.instr`.
- <reference/HTML link>

---
