# The `Reflectometer` Instrument

*McStas: Horizontal reflectometer, multi-angle of incidence*

## Identification

- **Site:** Templates
- **Author:** Anette Vickery, contact: anette.vickery@fys.ku.dk
- **Origin:** KU
- **Date:** November 2011

## Description

```text
A horizontal reflectometer. The instrument is built of a 2.5 m long mirror and an inclined elliptical guide. The grazing angle is defined by a set of
slits. The sample size is 4cm x 4cm (horizontal sample).
The role of the mirror is to provide an angle of incidence of up to 4 deg and at the same time avoid a direct line of sight to the source.
The reflectivity is the ratio between the direct beam reflected beam intensities. Therefore two simulations are needed to get the reflectivity curve:

Example: mcrun Reflectometer.instr -n1e10 directbeam=1,thetasample=0.4,Qmin=0,Qmax=0.15 -d directbeam
Example: mcrun Reflectometer.instr -n1e10 directbeam=0,thetasample=0.4,Qmin=0,Qmax=0.15 -d reflectedbeam
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| directbeam |  | If 1, the sample is a mirror with reflectivity 1. If 0, the mirror reflects like a D2O surface with zero roughness | 1 |
| Lam_min | AA | Minimum wavelength emitted by the cold moderator | 2.0 |
| Lam_max | AA | Maximum wavelength emitted by the cold moderator | 10 |
| deltatheta | % | The angular resolution | 2 |
| theta_sample | deg | The grazing angle | 1 |
| defineAngle |  | If 0, the angle defining slits are wide open. If 1 the angular resolution is deltatheta | 1 |
| Qmin | AA^-1 | The minimum value of the interesting Qrange | 0 |
| Qmax | AA^-1 | The maximum value of the interesting Qrange | 0.5 |
| straight |  | If 1, the guide walls are straight | 1 |
| width | m | The width of the guide in the case of straight guide walls | 0.05 |

## Links

- [Source code](TOF_Reflectometer.instr) for `TOF_Reflectometer.instr`.

---
