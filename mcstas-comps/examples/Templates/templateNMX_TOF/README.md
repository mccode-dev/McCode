# The `templateNMX_TOF` Instrument

*McStas: A simple Laue NMX TOF diffractometer for macromolecules, adapted from templateNMX
and templateLaue instruments.

Demonstrates use of PSD_monitor_TOF.

Example: templateNMX_TOF.instr REPS=53 reflections=Rubredoxin.lau theta=-40.85 phi=15.188 xw=0.012 yh=0.012 tmin=13000 tmax=15000 Detector: det_I=78816.1
Example: templateNMX_TOF.instr REPS=5 reflections=YBaCuO.lau theta=-91.1 phi=0 xw=0.012 yh=0.012 tmax=7000 tmin=5000 Detector: det_I=6277.74*

## Identification

- **Site:** Templates
- **Author:** K. Nielsen
- **Origin:** DTU
- **Date:** June 2nd, 2010

## Description

```text
A single crystal sample is illuminated with a white cold beam.
Based on a Laue tutorial written by K. Nielsen, Feb 7, 2000.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| REPS |  | Number of SPLIT repetitions | 53 |
| reflections | string | MX crystal reflection list | "Rubredoxin.lau" |
| theta | deg | Rotation of sample around y (1st rotation) | -40.85 |
| phi | deg | Rotation of sample around x (2nd rotation) | 15.188 |
| xw | m | Width of final monitors | 0.012 |
| yh | m | Height of final monitors | 0.012 |
| tmin | mu-s | Minimum ToF to record | 0 |
| tmax | mu-s | Maximum ToF to record | 2e5 |

## Links

- [Source code](templateNMX_TOF.instr) for `templateNMX_TOF.instr`.

---
