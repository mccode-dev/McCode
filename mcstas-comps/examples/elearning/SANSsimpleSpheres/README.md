# The `SANSsimpleSpheres` Instrument

*McStas: SANS_simple from e-neutrons.org, including polydisperse hard spheres.*

## Identification

- **Site:** elearning
- **Author:** Linda Udby (udby@nbi.dk)
- **Origin:** Niels Bohr Institute
- **Date:** 20.09.2016

## Description

```text
Instrument longer description (type, elements, usage...)
This simple SANS instrument is used for the SANS simulation quiz on th e-learning platform www.e-neutrons.org

Example: mcrun SANSsimple.instr  Lambda=10 DLambda=0
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| pinhole_rad | m | radius of the collimating pinholes. Also used to optimise the sample size. | 0.004 |
| LC | m | length of the collimator - distance between pinholes | 3 |
| LD | m | distance between the last pinhole slit and detector | 3 |
| Lambda | Angs | Average wavelength traced from source | 6 |
| DLambda | Angs | Wavelength band +/- traced from source | 0.001 |
| R | m | average radius of the monodisperse spheres in the sample | 400 |
| dR | AA | Normal variance of  the radius of spheres in the sample. If zero the sample is monodisperse. | 0 |
| PHI | 1 | Volumefraction of the hard spheres in the sample | 1e-2 |
| Delta_Rho | fm/Angs^3 | Volume specific scattering length density contrast of the  hard, monodisperse spheres in the sample as compared to the solution | 0.6 |
| BEAMSTOP | 0/1 | If set, the beamstop is inserted in front of the detector in order to block the transmitted beam | 1 |
| SAMPLE | 0/1 | If set, a sample of spheres or spherical shells is inserted | 1 |

## Links

- [Source code](SANSsimpleSpheres.instr) for `SANSsimpleSpheres.instr`.
- https://sim.e-neutrons.org/instrument/intro-ns/SANSsimple

---
