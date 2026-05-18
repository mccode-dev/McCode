# The `MCPL2hist` Instrument

*McStas: Flexible histogramming instrument file for processing MCPL input files using Monitor_nD*

## Identification

- **Site:** Tools
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** 2017-04

## Description

```text
Flexible histogramming instrument file for processing MCPL input files using Monitor_nD.

Example1: Multiple 1D histograms of position, velocity, divergence, time and wavelength, use
NDoptions=1 defines options="previous, multiple, auto, x, y, z, vx, vy, vz, hdiv, vdiv, tof, lambda"

Example2: 2D x-y plot of beam cross section:
NDoptions=2 defines options="previous, auto, x, y"

Example3: 2D TOF-lambda plot of beam:
NDoptions=3 defines options="previous, auto, tof, lambda"
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| MCPLfile | str | Defines the MCPL input file to process | "my.mcpl.gz" |
| NDoptions | int | Defines what Monitor_nD measures, see example list | 1 |

## Links

- [Source code](MCPL2hist.instr) for `MCPL2hist.instr`.
- <reference/HTML link>
- MCPL website at <a href="https://mctools.github.io/mcpl/">https://mctools.github.io/mcpl/</a>

---
