# The `Test_Sources` Instrument

*McStas: A test instrument to compare sources*

## Identification

- **Site:** Tests_optics
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** Aug 3, 2008

## Description

```text
A test instrument to compare sources and check they provide the valid
sprectrum and intensity. It shows that the first 4 flat sources are equivalent,
the 2 Maxwellian sources as well.

WARNING: Result of test no. 1 for Source_adapt.comp is not correct if MPI is used, as
that source component does not support MPI.

Example: source=1 Detector: m1_I=9.97273e+11
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| source | 1 | selection of the source to use in 1=Source_adapt, 2=Source_div, 3=Source_simple, 4=Source_gen (simple), 5=Source_gen, 6=Source_Maxwell_3, 7=ESS_butterfly, 8=Moderator | 0 |
| Lmin | AA | Minimum wavelength produced at source | 1 |
| Lmax | AA | Maximum wavelength produced at source | 11 |

## Links

- [Source code](Test_Sources.instr) for `Test_Sources.instr`.

---
