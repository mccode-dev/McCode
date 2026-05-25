# The `Test_DiskChoppers` Instrument

*McStas: Simple test instrument that compares the use of 2 DiskChoppers with one MultiDiskChopper*

## Identification

- **Site:** Tests_optics
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** September 2018

## Description

```text
Simple test instrument that compares the use of 2 DiskChoppers with one MultiDiskChopper
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| chopper | int | chopper=0 selects two DiskChoppers, chopper=1 selects one MultiDiskChopper | 0 |
| lambda | AA | Mean wavelength produced from the source | 10 |
| dlambda | AA | Halfwidth of wavelenghts produced from the source | 9.9 |
| deltay | m | Vertical displacement of MultiDiskChopper vertical centre of rotation (default is to be positioned like DiskChopper) | -0.19 |

## Links

- [Source code](Test_DiskChoppers.instr) for `Test_DiskChoppers.instr`.

---
