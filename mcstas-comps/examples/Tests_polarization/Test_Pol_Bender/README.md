# The `Test_Pol_Bender` Instrument

*McStas: Test Pol_bender.*

## Identification

- **Site:** Tests_polarization
- **Author:** Peter Christiansen
- **Origin:** RISOE
- **Date:** July 2006

## Description

```text
Test that Pol_bender polarizes an unpolarized beam, and allows one
to test the different options.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| GUIDELENGTH | m | Length of bender | 1.0 |
| GUIDERADIUS | m | Radius of curvature of bender | 10.0 |
| ENDOPTION |  | Setting for parallel start/end planes of bender | 0 |
| NSLITS |  | Number of channels in bender | 5 |
| WSPACER | m | Spacer dimension | 0.005 |
| DRAWOPTION |  | Bender drawing option 1: fine(all slits/90 points per arc), 2: normal (max 20/40), 3: rough (max 5/10) | 1 |

## Links

- [Source code](Test_Pol_Bender.instr) for `Test_Pol_Bender.instr`.

---
