# The `Test_Mirror_conic` Instrument

*McStas: Compares the conic.h reflection routines with Mirror.comp*

## Identification

- **Site:** Tests_optics
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** October 2026

## Description

```text
A collimated, polychromatic beam hits a single flat mirror at grazing angle theta.
The mirror is either Mirror.comp or Mirror_conic.comp, which uses the reflection
routines from conic.h (as used by Conics_*, FlatEllipse_finite_mirror and NMO).
Reflected and transmitted beams are recorded separately as function of wavelength.
For a given theta, pairs of COMP settings should give identical results:

COMP=0: Mirror, transmit=1           (reference for COMP=1)
COMP=1: Mirror_conic, conic=0        (FlatSurf, reflectNeutronFlat)
COMP=2: Mirror, transmit=0           (reference for COMP=3)
COMP=3: Mirror_conic, conic=1        (ConicSurf, reflectNeutronConic)

COMP=0 and COMP=1 should agree exactly, and so should COMP=2 and COMP=3 when
no figure error is applied (sigma_m=sigma_s=0).
With sigma_m comparable to the grazing angle (e.g. theta=0.1 sigma_m=1.745e-3),
COMP=3 uses the waviness model of Hansen et al., arXiv:1612.03763 (see conic.h).
```

## Examples

- **Test: COMP=0 Detector: L_refl_I=8.98208e-07**
- **Test: COMP=1 Detector: L_refl_I=8.98208e-07**
- **Test: COMP=0 Detector: L_trans_I=7.01789e-07**
- **Test: COMP=1 Detector: L_trans_I=7.01789e-07**
- **Test: COMP=2 Detector: L_refl_I=8.9763e-07**
- **Test: COMP=3 Detector: L_refl_I=8.9763e-07**
- **Test: COMP=3 theta=0.1 sigma_m=1.745e-3 Detector: L_refl_I=1.35886e-06**

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
| sigma_m | rad | RMS meridional slope (figure) error, COMP=3 only | 0 |
| sigma_s | rad | RMS sagittal slope (figure) error, COMP=3 only | 0 |

## Links

- [Source code](Test_Mirror_conic.instr) for `Test_Mirror_conic.instr`.

---
