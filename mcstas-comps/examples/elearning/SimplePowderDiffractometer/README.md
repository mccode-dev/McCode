# The `SimplePowderDIffractometer` Instrument

*McStas: SimplePowderDIffractometer from e-neutrons.org: A simple monochromatic powder diffractometer with variable bandwidth, aluminium sample container with powder sample (default nickel powder).*

## Identification

- **Site:** elearning
- **Author:** Linda Udby <udby@nbi.dk>
- **Origin:** Your institution
- **Date:** 26/02/2015

## Description

```text
A simple monochromatic powder diffractometer with variable bandwidth, collimation, aluminium sample container and powder sample.
The sample container (Al can) is optional.
A banana detector records scattering agles [20,100]

Example: mcrun SimplePowderDIffractometer.instr coll=40, container=1
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda0 | AA | The mean value of  incoming wavelegths (Gaussian distribution) | 1 |
| dlambda | AA | Gaussian sigma of incoming wavelength distribution | 0.005 |
| coll | arcmin | horizontal collimation | 120 |
| container | 1 | When >0 a 2mm thick Al pressed powder can is inserted around the sample | 0 |
| sample | 1 | 0=Ni, 1=Fe, 2=SiO2, 3=C_diamond, otherwise empty | 0 |

## Links

- [Source code](SimplePowderDiffractometer.instr) for `SimplePowderDiffractometer.instr`.
- http://vnt.nmi3.org/mcstas-web

---
