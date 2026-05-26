# The `ISIS_OSIRIS` Instrument

*McStas: A simulation of the indirect TOF geometry part of the OSIRIS instrument.*

## Identification

- **Site:** ISIS
- **Author:** Peter Christiansen with input from Mark Telling, updates by P Willendrup
- **Origin:** DTU Fysik, RISOE and ISIS
- **Date:** November 2015

## Description

```text
If LAMBDA = 0, the choppers are disabled, else the choppers are
centered to chop around this wavelength and the source is studied in the
interval LAMBDA +- DLAMBDA.

IMPORTANT NOTES:

1) Monchromator is not realistic in the sense that it does NOT
smear the beam out according to the mosaicity, only does the N=1
reflection, and the d-spread is handled in a simple way

2) The instrument is not validated or certified by ISIS. Is only put in
McStas for completeness.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| LAMBDA | AA | Mean wavelength at source | 6.66 |
| DLAMBDA | AA | Wavelength spread at source | 0.1 |
| GUIDEREFLECTIVITY | 1 | R0-reflectivity of guide | 1.0 |

## Links

- [Source code](ISIS_OSIRIS.instr) for `ISIS_OSIRIS.instr`.

---
