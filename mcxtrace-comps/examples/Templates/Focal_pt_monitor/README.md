# The `Focal_pt_monitor` Instrument

*McXtrace: Template for creating a focal point monitor.*

## Identification

- **Site:** Templates
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Aug. 21

## Description

```text
A template instrument shows how to create a focal point monitor with an
instance of Monitor_nD and an EXTEND block on a preceeding Arm.
The instrument uses a point source and a thin lens to create a focus.

The working principle behind, is to bin the weight carried by each ray by the
z-coordinate (of a cartesian system defined by an Arm preceeding the Monitor_nD
instance) where the ray's trajectory passes closest to the Z-axis.

Note, the origin of the monitored z-coordinate is defined by the Arm on which
the EXTEND-block is attached (In this case fpt0). If the subsequent monitor
is placed elsewhere an implicit offset will be the result.
The
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Rcurve | m | Radius of curvature of the lens at the apex. | 100e-6 |

## Links

- [Source code](Focal_pt_monitor.instr) for `Focal_pt_monitor.instr`.

---
