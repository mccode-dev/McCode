# The `Test_Filter` Instrument

*McXtrace: Test instrument for checking the Filter.comp component*

## Identification

- **Site:** Tests_optics
- **Author:** Erik Knudsen (erkn@risoe.dtu.dk)
- **Origin:** Ris&oslash; DTU
- **Date:** July 5th 2011

## Description

```text
Test instrument for checking the Filter.comp component
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| filter_mat |  | Chemical symbol of the filter material | "Rh.txt" |
| thickness | m | thickness of the filter block | 100e-6 |
| L0 | AA | centre wavlength of the source | 1 |
| DL | AA | half width of the (uniform) wavelength distribution | 0.1 |
| F2 | 1 | add a 2nd filter component further away | 0 |

## Links

- [Source code](Test_Filter.instr) for `Test_Filter.instr`.

---
