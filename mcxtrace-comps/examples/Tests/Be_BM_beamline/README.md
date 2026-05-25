# The `Be_BM_beamline` Instrument

*McXtrace: Be-lens Bending Magnet, Low Budget Monochromatic (pink) X-ray beamline*

## Identification

- **Site:** Tests
- **Author:** E. Knudsen (erkn@risoe.dtu.dk)
- **Origin:** Ris&oslash; DTU
- **Date:** Sept. 15th, 2009

## Description

```text
This is a proof of concept beamline to test an idea of how to build a low-cost
monochromtic beamline. Idea is to let a polychromatic beam impinge on a focusing
Be-lens and put a slit in the focal pt. of the lens for the desired wavelength. Any
other wavelengths present in the beam will not be focussed prefectly and may be
by the slit.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L1 | m | Distance between source and lens | 10 |
| L2 | m | Distance between lens and and slit | 1 |
| dS | m | Slit opening. Slit opening is quadratic. | 1e-2 |
| N | 1 | NUmber of Lenses in the compund lens arrangement | 1 |

## Links

- [Source code](Be_BM_beamline.instr) for `Be_BM_beamline.instr`.

---
