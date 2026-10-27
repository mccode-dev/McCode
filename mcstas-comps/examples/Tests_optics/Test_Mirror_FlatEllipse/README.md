# The `Test_Mirror_FlatEllipse` Instrument

*McStas: Compares a (nearly flat) FlatEllipse_finite_mirror with Mirror.comp*

## Identification

- **Site:** Tests_optics
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** October 2026

## Description

```text
A collimated, polychromatic beam hits a single mirror at grazing angle theta.
The mirror is either Mirror.comp with transmit=1, or a single-mirror
FlatEllipse_finite_mirror (conic.h, mirror_width=0) with foci at +/- focus,
which for the default focus=10 km is flat to within ~1e-10 rad over the mirror.
Reflected and transmitted beams are recorded separately as function of wavelength,
and the two COMP settings should give the same result.

COMP=0: Mirror, transmit=1
COMP=1: FlatEllipse_finite_mirror, nummirror=1, mirror_width=0
```

## Examples

- **Test: COMP=0 Detector: L_refl_I=8.98208e-07**
- **Test: COMP=1 Detector: L_refl_I=8.98208e-07**
- **Test: COMP=0 Detector: L_trans_I=7.01789e-07**
- **Test: COMP=1 Detector: L_trans_I=7.01789e-07**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| COMP | 1 | Mirror implementation, see description | 0 |
| theta | deg | Grazing angle of the beam on the mirror | 1 |
| length | m | Mirror length | 1 |
| m | 1 | Mirror m-value | 2 |
| R0 | 1 | Low-angle reflectivity | 0.99 |
| Qc | AA^-1 | Critical scattering vector | 0.0217 |
| alpha | AA | Slope of reflectivity | 6.07 |
| W | AA^-1 | Width of supermirror cut-off | 0.003 |
| Lmin | AA | Minimum source wavelength | 1 |
| Lmax | AA | Maximum source wavelength | 11 |
| focus | m | Distance from the mirror centre to each of the ellipse foci | 1e4 |
| r0 | m | Distance from the FlatEllipse_finite_mirror optical axis to the mirror | 0.1 |

## Links

- [Source code](Test_Mirror_FlatEllipse.instr) for `Test_Mirror_FlatEllipse.instr`.

---
