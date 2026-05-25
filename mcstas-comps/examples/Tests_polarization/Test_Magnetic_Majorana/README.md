# The `Test_Magnetic_Majorana` Instrument

*McStas: This instrument demonstrates how to use the Pol_Bfield
component with a Majorana field.*

## Identification

- **Site:** Tests_polarization
- **Author:** Peter Christiansen and Peter Willendrup
- **Origin:** RISOE
- **Date:** August 2006

## Description

```text
This instrument demonstrates how to use the component
Pol_Bfield with a so called Majorana field.

See:
Seeger et al. NIM A: Volume 457, Issues 1-2 , 11 January 2001, Pages 338-346

Note that there are some sytematic fluctuation with the current solution.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Blarge | T | Large linear decreasing field along x axis (+Blarge to -Blarge). | 1.0 |
| Bsmall | T | Constant small component of field along y axis. | 0.003 |
| magnetLength | m | Magnet length. | 1.0 |

## Links

- [Source code](Test_Magnetic_Majorana.instr) for `Test_Magnetic_Majorana.instr`.

---
