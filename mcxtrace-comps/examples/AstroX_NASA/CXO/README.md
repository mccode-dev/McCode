# The `CXO` Instrument

*McXtrace: Chandra X-ray Observatory CXO*

## Identification

- **Site:** AstroX_NASA
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Nov '20

## Description

```text
Model of the Chandra X-ray Observatory based on the description avaiable
in the Chandra Observers' Guide. Fully collimated X-ray light impinges on the
optic consisting of 4 mirrors in the true Wolter I configuration.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central eX-ray energy to be simulated. | 1 |
| dE | keV | Half-width of energy spectrum | 0.001 |
| verbose | 0 | Emit extra information. | 1 |
| mcoat | str | Filename of file which conatins reflectivity for coating | "Ir_CXO.dat" |
| OA_angle | deg | Off-axis angle. Angle around y-axis at which radiation will hit the optic. | 0 |

## Links

- [Source code](CXO.instr) for `CXO.instr`.
- https://cxc.harvard.edu/proposer/POG/html/index.html

---
