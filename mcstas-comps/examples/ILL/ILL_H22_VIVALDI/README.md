# The `ILL_H22_VIVALDI` Instrument

*McStas: The VIVALDI Laue diffractometer on the H22 curved thermal guide at the ILL*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** June, 2008

## Description

```text
VIVALDI (Very-Intense, Vertical-Axis Laue DIffractometer) provides a tool for
development of new diffraction experiments, and is complementary to other ILL
single-crystal diffractometers. Fields of interest for experiments on VIVALDI
include magnetism, charge density waves, high-pressure studies and structural
phase transitions. VIVALDI allows rapid preliminary investigation of new
materials, even when only small single crystals are available. The detector is
also suitable for some types of diffuse scattering experiments on a
monochromatic beam.

VIVALDI is located at the end of the thermal guide H22. The full thermal
spectrum can be accepted without detrimental overlap of reflections for
primitive unit cells up to 25 Angs on edge.

This model include a cryostat and container description, with a crystal sample.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | mean incident wavelength. | 3 |
| dlambda | AA | wavelength full width. | 2.2 |
| crystal | str | File name for crystal description (LAU) | "YBaCuO.lau" |
| container | str | File name for sample-container material description | "V.laz" |
| verbose | 1 | Print DIF configuration. 0 to be quiet | 1 |

## Links

- [Source code](ILL_H22_VIVALDI.instr) for `ILL_H22_VIVALDI.instr`.

---
