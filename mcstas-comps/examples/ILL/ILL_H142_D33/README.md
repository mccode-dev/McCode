# The `ILL_H142_D33` Instrument

*McStas: A simplified H142 cold guide model at the ILL, with a simple D33 model at the end*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** 2012

## Description

```text
This model decribes a simiplified H142 cold guide at the ILL, with D33.

The D33 Massive dynamic q-range small-angle diffractometer is a Small-Angle Neutron
Scattering instrument for the characterization of samples with typical sizes
varying from the nanometer scale to few tenth of micrometer. In addition to a
standard monochromatic mode of operation, D33 offers a time of flight mode (TOF)
to cover an enhanced dynamic q-range qmax/qmin in one instrument setting. High
magnetic fields, up to 17 T at the sample position, beam polarization and 3He
spin analysis, facilitate and expand studies of magnetism and allow a more
quantitative analysis of spin incoherent samples. The high flux allows for
kinetic experiments with time resolution of the order of few milliseconds.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda |  | central wavelength band for guide illumination [Angs] | 14 |
| dlambda |  | half width of guide wavelength band [Angs] | 1.4 |
| m1 |  | m-coating for 1st guide section | 1 |
| m2 |  | m-coating for 2nd guide section | 1 |
| m3 |  | m-coating for 3th guide section | 1 |
| m4 |  | m-coating for 4th guide section | 1 |
| diaphragm |  | diaphragm diameter between section 3 and 4; use 0 to remove [cm] | 0 |

## Links

- [Source code](ILL_H142_D33.instr) for `ILL_H142_D33.instr`.

---
