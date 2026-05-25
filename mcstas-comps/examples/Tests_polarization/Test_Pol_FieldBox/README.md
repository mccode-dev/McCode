# The `Test_Pol_FieldBox` Instrument

*McStas: Unit test instrument for Pol_FieldBox*

## Identification

- **Site:** Tests_polarization
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk>
- **Origin:** DTU Physics
- **Date:** Jan 20

## Description

```text
An instrument with a single field box with a 1 mT magnetic field along the y-axis.
A neutron beam polarized aling the x-axis is emitted with central wavelength such
that the  polarization is rotated pihalfturns radians in the field.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| pihalfturns |  | beam polarization (for the cetral wavelength) is rotated pihalfturns by the field | 1 |
| BOX |  | flag to select between 0:Pol_FieldBox and 1:Pol_constBfield | 0 |

## Links

- [Source code](Test_Pol_FieldBox.instr) for `Test_Pol_FieldBox.instr`.

---
