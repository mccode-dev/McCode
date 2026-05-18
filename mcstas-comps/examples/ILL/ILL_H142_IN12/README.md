# The `ILL_H142_IN12` Instrument

*McStas: The H142 S-curved cold guide at the ILL feeding IN12 TAS spectrometer*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** April 7, 2004

## Description

```text
The H142 beam is the only S-curved guide at the ILL. It is used here to feed
the IN12 TAS spectrometer (classical configuration).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| m | 1 | m-value of guide coating (H14/H142) | 1 |
| KI | Angs-1 | central wavevector for incoming neutrons | 2.662 |
| QM | Angs-1 | wavevector transfer modulus at the sample | 1.0 |
| EN | meV | energy transfer at the sample | 0.0 |
| verbose |  | verbose-mode toggle | 1 |
| WM | m | Width of monochromator | 0.08 |
| HM | m | Height of monochromator | 0.12 |
| NHM | 1 | Number of vertical slabs composing the monochromator | 1 |
| NVM | 1 | Number of horizontal slabs composing the monochromator | 6 |
| RMV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| WA | m | Width of analyzer | 0.121 |
| HA | m | Height of analyzer | 0.118 |
| NHA | 1 | Number of horizontal slabs composing the analyzer | 11 |
| NVA | 1 | Number of vertical slabs composing the analyzer | 1 |
| RAH | m | Analyzer horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| L2 | m | Monochromator-Sample distance. Contains 2nd Collimator | 1.726 |
| L3 | m | Sample-Analyzer distance. Contains 3rd Collimator | 1.300 |
| L4 | m | Analyzer-detector distance. Contains 4th Collimator | 0.710 |

## Links

- [Source code](ILL_H142_IN12.instr) for `ILL_H142_IN12.instr`.
- The <a href="http://www.ill.fr/in12">IN12 TAS</a> at the ILL

---
