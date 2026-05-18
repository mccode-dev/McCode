# The `MAXIV_FemtoMAX` Instrument

*McXtrace: Simulation of the FemtoMAX short pulse facility at MAX IV laboratory*

## Identification

- **Site:** MAXIV
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jun 2017

## Description

```text
This a a skeleton version of the FemtoMAX short-pulse facility at MAXIV
N.b. This model is out of date with the present day instrumentation of FemtoMAX.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DXUS | m | Horizontal opening of the user slit. | 1e-2 |
| DYUS | m | Vertical opening of the user slit. | 1e-2 |
| MONO |  | Flag enabling the Crystal monochromator. | 1 |
| MLMONO |  | Flag enabling the mulitlayer monochromator. | 1 |
| U2SRC |  | Flag enabling wide source model. | 1 |
| U3SRC |  | Flag enabling the narrow source model. Overrides U2SRC. | 0 |
| RX |  | X-rotation of the sample goniometer. | 0 |
| RY |  | Y-rotation of the sample goniometer. | 0 |

## Links

- [Source code](MAXIV_FemtoMAX.instr) for `MAXIV_FemtoMAX.instr`.
- https://www.maxiv.lu.se/accelerators-beamlines/beamlines/femtomax/

---
