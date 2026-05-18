# The `templateSANS_MCPL` Instrument

*McStas: Test instrument for the Sans_spheres component. No guide / velocity selector
etc. Will be developed further at later time. Behaves like the normal templateSANS
but dumps all events in an MCPL file.*

## Identification

- **Site:** Templates
- **Author:** <a href="mailto:kim.lefmann@risoe.dk">Kim Lefmann</a>
- **Origin:** <a href="http://www.risoe.dk">Risoe</a>
- **Date:** 19th Dec 2003.

## Description

```text
Very simple test instrument for the Sans_spheres component

Example: lambda=6 Detector: detector_I=6.56942e-17
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons | 6 |
| dlambda | AA | Wavelength spread of neutrons | 0.05 |
| r | AA | Radius of scattering hard spheres | 100 |
| PHI | 1 | Particle volume fraction | 1e-3 |
| Delta_Rho | fm/AA^3 | Excess scattering length density | 0.6 |
| sigma_abs | barns | Absorption cross section at 2200 m/s | 0.5 |

## Links

- [Source code](templateSANS_MCPL.instr) for `templateSANS_MCPL.instr`.

---
