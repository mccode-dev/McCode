# The `Test_Guides_Curved` Instrument

*McStas: Cross comparison of curved Guide components*

## Identification

- **Site:** Tests_optics
- **Author:** P. Willendrup, DTU Fysik
- **Origin:** DTU Fysik
- **Date:** Nov 1st, 2013

## Description

```text
Cross comparison of curved Guide components, using McStas and
contributed components. It shows that all implementations are to good approximation equivalent.
```

## Examples

- **Test: Guide=1 Detector: Monitor2_xy1_I=0.00224746**
- **Test: Guide=2 Detector: Monitor2_xy2_I=0.00177338**
- **Test: Guide=3 Detector: Monitor2_xy3_I=0.00186028**
- **Test: Guide=4 Detector: Monitor2_xy4_I=0.00168562**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Guide | 1 | Choice of Guide component to test, with 1=Guide_curved 2=Elliptic_guide_gravity, 3=Pol_bender, 4=Bender. | 1 |
| curvature | m | Radius of curvature | 1000 |
| length | m | Length of the guide | 100 |

## Links

- [Source code](Test_Guides_Curved.instr) for `Test_Guides_Curved.instr`.

---
