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

## Examples

- **Test: Guide=1 Detector: Monitor2_xy_I=0.00726875**
- **Test: Guide=2 Detector: Monitor2_xy_I=0.00725213**
- **Test: Guide=3 Detector: Monitor2_xy_I=0.00727304**
- **Test: Guide=4 Detector: Monitor2_xy_I=0.00721742**
- **Test: Guide=5 Detector: Monitor2_xy_I=0.00721742**
- **Test: Guide=6 Detector: Monitor2_xy_I=0.00721742**
- **Test: Guide=7 Detector: Monitor2_xy_I=0.00724912**
- **Test: Guide=8 Detector: Monitor2_xy_I=0.00683072**
- **Test: Guide=9 Detector: Monitor2_xy_I=0.00724912**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Guide | 1 | Choice of Guide component to test, with 1=Guide, 2=Guide_channeled, 3=Guide_gravity, 4=Guide_wavy, 5=Guide_curved 6=Elliptic_guide_gravity 7=Guide_honeycomb | 1 |

## Links

- [Source code](Test_Guides.instr) for `Test_Guides.instr`.

---
