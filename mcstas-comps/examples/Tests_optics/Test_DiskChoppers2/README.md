# The `Test_DiskChoppers2` Instrument

*McStas: Simple test instrument that compares DiskChoppers with a simple, rotating Slit.*

## Identification

- **Site:** Tests_optics
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** September 2018

## Description

```text
Simple test instrument that compares DiskChoppers with a simple, rotating Slit.
When ABSORBER is set, a slab of B4C is acts as absorbing medium.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| chopper | int | chopper=0 selects DiskChopper, chopper=1 selects a rotating Slit | 0 |
| lambda | AA | Mean wavelength produced from the source | 10 |
| dlambda | AA | Halfwidth of wavelenghts produced from the source | 9.9 |
| deltay | m | Position of centre of rotation vs. beam in slit case | 0.19 |
| dx |  |  | 0.016 |
| nu | Hz | Chopper frequency | 10 |
| phase | deg | Chopper phase | 0 |
| ABSORBER | 1 | Flag to indicate if slab is B4C(=1) or perfect(=0) | 0 |
| tz | m | Thickness of B4C slab | 0.001 |

## Links

- [Source code](Test_DiskChoppers2.instr) for `Test_DiskChoppers2.instr`.

---
