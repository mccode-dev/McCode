# The `Test_PowderN` Instrument

*McStas: Test output of PowderN, NCrystal and Single_crystal on a spherical monitor / PSD.*

## Identification

- **Site:** Tests_samples
- **Author:** M. Bertelsen and P. Willendrup
- **Origin:** ESS DMSC
- **Date:** November 2024

## Description

```text
A test instrument for Powder output from different sample components.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp | 1 | 0=PowderN-with-NCMAT, 1=PowderN, 2=Single_crystal, 3=NCrystal | 1 |
| lambda | Angs | Wavelength emitted from source, 1% wl-interval around. | 2.5 |
| L1 | m | Source-sample distance | 10 |
| directbeam | 1 | Suppress direct beam or not | 0 |
| material | str | Material string for picking up CIF (PowderN/Single_Crystal) and ncmat (NCrystal) | "Ge" |
| SPLITS | 1 | Number of SPLIT's before sample | 1 |
| frac_t | 1 | Fraction of stats assigned to unscattered, "direct beam" | 0.1 |
| sxmos | arcmin | Mosaicity to use in Single_crystal case | 60 |
| twotheta | deg | Angle for 2\theta detector for detailed view | 94 |

## Links

- [Source code](Test_Powders.instr) for `Test_Powders.instr`.

---
