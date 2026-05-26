# The `ILL_SALSA` Instrument

*McStas: ILL_SALSA instrument*

## Identification

- **Site:** ILL
- **Author:** Daniel Lomholt Christensen
- **Origin:** ILL, Grenoble, France
- **Date:** 14:28:08 on January 23, 2024

## Description

```text
ESCRIPTION
SALSA is a instrument developed mainly for measuring residual stress
in a gauge volume defined by its collimators.
The instrument has three possible collimator positions, with three available collimators at each position.
This allows the gauge volumed to be defined in height, width and depth.
The size of the gauge volume is chosen in the collimator parameters. The following values correspond to sizes
in a dictionary format.
The vertical collimator {0: 2mm, 1: 4mm, 2: 10mm}
The horizontal collimator {0: 0.6mm, 1: 2mm, 2: 4mm}
The depth collimator {0: 0.6mm, 1: 2mm, 2: 4mm}

The instument showcases the Monochromator_bent component,
and the focusing use of the Collimator_radial component.

Some of the monitors used in this instrument are not actually present in the real instrument.
The only detector that is actually there, is the final monitor.
Furthermore the sample environment is of course variable depending on the sample
in question.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda_mean | Å | Desired wavelength of neutrons coming off of the monochromator. | 1.66795 |
| lambda_spread | Å | Range over which the neutrons are produced. lambda_mean +- lambda_spread. | 0.0224372 |
| m | 1 | m-value of whole guide coating. 0 absorbing, 1 for Ni, 1.2 for Ni58, 2-4 for SM | 2 |
| mip | 1 | m-value of in-pile guide coating | 2 |
| monochromator_horizontal_radius | m | Radius of the horizontal cylinder the monochromator is bent on. | 2.6 |
| takeoff_angle | deg | Takeoff angle of the instrument | 75.8 |
| mono_rotation | deg | Rotation of monochromator in the horizontal plane. Omega angle in SALSA | 37.9 |
| lamellas | 1 | Number of lamellas in the monochromator array | 16 |
| monochromator_slabs | 1 | Number of slabs of lamellas placed on top of each other in the monochromator array | 39 |
| vertical_mono_radius | m | Radius of the cylinder the monochromator is bent on vertically | 2.2 |
| vertical_focus | mm | Choice of collimator focus in mm. Choose either 2, 4, or 10. | 2 |
| horizontal_focus | mm | Choice of collimator focus in mm. Choose either 0.6, 2, or 4. | 0.6 |
| outgoing_focus | mm | Choice of collimator focus in mm. Choose either 0.6, 2, or 4. | 0.6 |
| measuring_angle | deg | Angle between the outgoing collimator and sample. Rotates in the negative direction of revolution. I.E with the clock. | 50 |
| Debug |  |  | 0 |

## Links

- [Source code](ILL_SALSA.instr) for `ILL_SALSA.instr`.

---
