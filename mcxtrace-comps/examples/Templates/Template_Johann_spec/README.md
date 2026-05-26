# The `Template_Johann_spec` Instrument

*McXtrace: Template instrument for a curved crystal Johann-spectrometer*

## Identification

- **Site:** Templates
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jan 2019

## Description

```text
This is a template instrument for a Johann spectrometer intended
for easy inclusion in other "real" instrument simulations.

The source is a divergent line like source which illumnates the
full crystal. The analyzer crystal itself is modelled as a single
bent crystal curved to a primary radius of 2*Rowland-radius.

To include a Johann spectrometer in an instrument the sample should be put where
source is in this template. and otherwis copy-paste.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L | m | Length of the analyzer crystal. | 0.2 |
| r_Row | m | Radius of the Rowland circle. Also governs the crystal curvature (2*r_Row). | 0.5 |
| theta_inc | deg | Angle of incidence at the crystal centre | 78.5 |
| dtheta_s | deg | Rotation angle of the Source. To allow scanning with a pencil beam across the crystal face. | 0 |
| phi_s | deg | Opening angle of source. If 0 it is set to cover the full crystal (and some). | 0 |

## Links

- [Source code](Template_Johann_spec.instr) for `Template_Johann_spec.instr`.

---
