# The `ILL_H15_D11` Instrument

*McStas: D11 at the ILL: Lowest momentum transfer, lowest background small-angle neutron scattering instrument*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** 2012

## Description

```text
D11 - LOWEST MOMENTUM TRANSFER and  LOWEST BACKGROUND SANS

D11 is the archetype of a long, pinhole geometry instrument for small angle
neutron scattering (SANS), designed for the study of large scale structures in
soft matter systems, chemistry, biology, solid state physics and materials science.

This model allows to specify the collimation length, i.e. the free path between
the end of the guide and the sample location. When Lc=1.5 m, the intensity is
maximal, but Q-resolution (divergence) is degraded. The Lc=40.5 m configuration
is the low-Q one.
The collimation can also be given as predefined configuration 'iLc' starting from 0

iLc ={ 1.5, 2.5, 4, 5.5, 8, 10.5, 13.5, 16.5, 20.5, 28, 34, 40.5}

so that e.g. iLc=5 corresponds with a 10.5 m collimation

The quality of the collimation section can be specified among a set of glass
pre-set specifications. The available configurations are:

| Borkron_1972   Borkron_2003    Borofloat_2001  Borofloat_2003
Chamfers [mm] | 0.2            0.2             0.8             0.2
Wavyness [rad]| 2.5e-5         1e-4            8e-4            2e-4
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lambda | Angs | neutron wavelength reaching D11 sample | 4.51 |
| Config | string | guide configuration. Must be one of Borkron_1972 Borkron_2003 Borofloat_2001 Borofloat_2003 | "Borkron_1972" |
| Lc | m | guide collimation, i.e. free path between end of guide and sample location. Must be one of 40.5, 34, 28, 20.5, 16.5 13.5 10.5, 8, 5.5, 4, 2.5, 1.5 | 0 |
| iLc | index | guide collimation predefined length | 5 |
| Chamfers | m | chamfers width (input/output movable guide section+longitudinal top/bottom sides) | 0.0002 |
| Waviness | rad | movable guide Waviness | 2.5e-5 |

## Links

- [Source code](ILL_H15_D11.instr) for `ILL_H15_D11.instr`.

---
