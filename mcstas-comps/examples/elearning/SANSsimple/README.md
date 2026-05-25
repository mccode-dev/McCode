# The `SANS2_liposomes` Instrument

*McStas: SANSsimple from e-neutrons.org*

## Identification

- **Site:** elearning
- **Author:** Linda
- **Origin:** Copenhagen
- **Date:** 05.09.10

## Description

```text
Instrument longer description (type, elements, usage...)

Example: mcrun SANSsimple.instr <parameters=values>
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| pinhole_rad | m | radius of the collimating pinholes | 0.004 |
| LC | m | length of the collimator - distance between pinholes | 3 |
| LD | m | distance between the last pinhole slit and detector | 3 |
| Lambda | AA | Average wavelength traced from source | 6 |
| DLambda | AA | Wavelength band +/- traced from source | 0.6 |
| R | AA | radius of the hard, monodisperse spheres in the sample | 400 |
| dR | AA | Normal variance of Radius | 0 |
| dbilayer | AA | Thickness of spherical shell, only relevant when SAMPLE==2 | 35 |
| PHI | 1 | Volumefraction of the hard, monodisperse spheres in the sample | 1e-2 |
| Delta_Rho | fm/AA^3 | Volume specific scattering length density contrast of the  hard, monodisperse spheres in the sample as compared to the solution | 0.6 |
| Qmax | AA^-1 | Maximum scattering vector allowed by geometry to hit the detector area | 0.3 |
| BEAMSTOP | 0/1 | If set, the beamstop is inserted in front of the detector in order to block the transmitted beam | 1 |
| SAMPLE | 0/1/2 | When SAMPLE==0, no sample is used, SAMPLE==1 sample is composed of hard spheres, if SAMPLE==2 sample is composed ofspherical shells. | 1 |
| Sigma_a | barn | Absorption crossection of the sample | 0 |

## Links

- [Source code](SANSsimple.instr) for `SANSsimple.instr`.
- A reference/HTML link for more information

---
