# The `PSI_DMC_simple` Instrument

*McStas: McStas model of the DMC powder diffractometer at PSI, CH.*

## Identification

- **Site:** PSI
- **Author:** Peter Willendrup (Risoe), Uwe Filges (PSI), Lukas Keller (PSI)
- **Origin:** PSI
- **Date:** May 7th, 2008

## Description

```text
McStas model of the DMC powder diffractometer at PSI, CH.

Please note that this instrument file does not include the radial collimator of DMC.
To generate the full 800-bin angle coverage at DMC, you should combine two simulations
using this instrumentfile, with SHIFT=0 and 0.1. This will displace the detector by half
a bin-width, which is a standard procedure at the DMC diffractometer
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Choice of wavelength, affects both monochromator and source component | 2.5666 |
| R | 1 | Reflectivity of non-curved guides | 0.87 |
| R_curve | 1 | Reflectivity of curved guides | 0.87 |
| filename | str | Choice of reflection list file, e.g. from McStas data dir | "Na2Ca3Al2F14.laz" |
| D_PHI | deg | Focusing 'd_phi' for PowderN, see mcdoc page | 6 |
| SHIFT | deg | Rotation of detector, set to 0.1 to displace by half a bin | 0 |
| PACK | 1 | Powder packing factor | 0.7 |
| Dw | 1 | Powder Debye-Waller factor | 0.8 |
| BARNS | 1 | Flag to define if powder reflection file \|F2\| is in Barns or fm | 1 |

## Links

- [Source code](PSI_DMC_simple.instr) for `PSI_DMC_simple.instr`.
- The <a href="http://lns00.psi.ch/montecarlo/">PSI Monte-Carlo</a> website.
- P Willendrup, U Filges, L Keller, E Farhi, K Lefmann: Validation of a realistic powder sample using data from DMC at PSI, ICNS 2005 (Physica B, 386, (2006), 1032.)

---
