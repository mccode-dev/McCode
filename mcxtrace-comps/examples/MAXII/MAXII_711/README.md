# The `MAXII_711` Instrument

*McXtrace: Powder diffraction beamline*

## Identification

- **Site:** MAXII
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Sep 2013

## Description

```text
A simple beamline setup consisting of a single focusing mirror,
a monochromator, and a four-circle goniometer at the sample stage.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| R | m | Radius of curvature of the focusing mirror. N.b. a negative curvature simply means the mirror curves towards negative x | -100 |
| phi_mirror | m | Set angle of incidence of the focusing mirror | 3 |
| phi_mono | deg | Set angle of incidence of Monochromator crystal. | 9.25 |
| chi | deg | 2nd rotation of four circle, around X. | 0 |
| ome | deg | 1st rotation of four circle, around Y. | 0 |
| phi | deg | 3rd rotation of four circle, around Y'. | 0 |
| tth | deg | 4th rotation of four circle, i.e. detector arm. Around Y. | 0 |
| dphi | deg | Restrict the powder scattering angle along vertical axis Z. | 160 |

## Links

- [Source code](MAXII_711.instr) for `MAXII_711.instr`.

---
