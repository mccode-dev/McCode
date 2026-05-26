# The `Template_DCM` Instrument

*McXtrace: Template for a vertically defleting DCM*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk>
- **Origin:** DTU Physics
- **Date:** Jan '21

## Description

```text
Simple skeleton instrument showing a vertically deflecting
double crystal monochromator. To instead make it horizontally deflecting
simply add Arms before and after the crystal assembly that rotate by
90 and -90 deg. respectively around the z-axis.
The crystal is illuminated by a model point source.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central energy to be emitted by the source. 0=set from theta. | 0 |
| dE | keV | Half width to be emitted by the source | 0.2 |
| theta | deg | Rotation angle of the crystals. 0=set from E0. | 9.17 |

## Links

- [Source code](Template_DCM.instr) for `Template_DCM.instr`.

---
