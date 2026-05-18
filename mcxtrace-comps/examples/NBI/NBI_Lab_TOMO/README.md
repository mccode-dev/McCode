# The `NBI_Lab_TOMO` Instrument

*McXtrace: Laboratory tomography setup at NBI*

## Identification

- **Site:** NBI
- **Author:** Erik B Knudsen and M Thomsen
- **Origin:** Your institution
- **Date:** March 3rd 2014

## Description

```text
Consists simply of a Mo-source, a sample, an Al-filter and a detector, all in line with each other
The sample is a chess-king off-shape.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| fname |  | filename which contains the emission spectrum for the source | "spectrumU50_th5.dat" |
| d_sample_det | m | distance between the sample and the detector | .790 |
| d_source_sample | m | distance between source adn sample | 1.1450 |
| Omega | deg | rotation angle of sample (around y-axis) | 0 |
| detw | m | Detector width | .0351 |
| deth | m | Height of detector | .0334 |
| rx | deg | Rotation of the sample around the X-axis | -80 |
| ry | deg | Rotation of the sample around the Y-axis | 0 |
| rz | deg | Rotation of the sample around the Z-axis | 0 |

## Links

- [Source code](NBI_Lab_TOMO.instr) for `NBI_Lab_TOMO.instr`.

---
