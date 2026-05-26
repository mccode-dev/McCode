# The `Test_Magnetic_Constant` Instrument

*McStas: This instrument demonstrates how to use the Pol_Bfield
component with a constant field.*

## Identification

- **Site:** Tests_polarization
- **Author:** Peter Christiansen and Peter Willendrup
- **Origin:** RISOE
- **Date:** August 2006

## Description

```text
This instrument demonstrates how to use the component
Pol_Bfield with a constant field to make a Mezei Spin flipper.

This instrument matches the example: Test_Pol_MSF (under tests)
made with the old component: Pol_constBfield
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| ROT_ANGLE | deg | Angle that the spin of a 5AA neutron rotates in the 0.2 m magnet | 180 |
| ONOFF |  | Should we use a window description (with a _stop comp) to turn the field on and off. | 0 |

## Links

- [Source code](Test_Magnetic_Constant.instr) for `Test_Magnetic_Constant.instr`.

---
