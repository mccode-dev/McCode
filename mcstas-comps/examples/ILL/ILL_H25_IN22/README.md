# The `ILL_H25_IN22` Instrument

*McStas: IN22 thermal triple-axis machine (TAS) on guide H25 with sample*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 2006

## Description

```text
This instrument is a model of IN22@ILL with PG002 monochromator/analyzer,
installed at the end of the H25 supermirror thermal guide.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| m | 1 | m-value of guide coating (H14/H142) | 2 |
| KI | Angs-1 | central wavevector for incoming neutrons | 3.84 |
| QM | Angs-1 | wavevector transfer modulus at the sample | 1.0 |
| EN | meV | energy transfer at the sample | 0.0 |
| verbose |  | toggle verbose mode | 1 |
| WM | m | Width of monochromator | 0.15 |
| HM | m | Height of monochromator | 0.12 |
| NHM | 1 | Number of vertical slabs composing the monochromator | 1 |
| NVM | 1 | Number of horizontal slabs composing the monochromator | 9 |
| RMV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| WA | m | Width of analyzer | 0.20 |
| HA | m | Height of analyzer | 0.10 |
| NHA | 1 | Number of horizontal slabs composing the analyzer | 11 |
| NVA | 1 | Number of vertical slabs composing the analyzer | 3 |
| RAV | m | Analyzer vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| RAH | m | Analyzer horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| SM | 1:left, -1:right | Scattering sense of beam from Monochromator | -1 |
| SS | 1:left, -1:right | Scattering sense of beam from Sample | 1 |
| SA | 1:left, -1:right | Scattering sense of beam from Analyzer | -1 |
| L1 | m | Source-Monochromator distance. Contains 1st Collimator | 10.0 |
| L2 | m | Monochromator-Sample distance. Contains 2nd Collimator | 1.7 |
| L3 | m | Sample-Analyzer distance. Contains 3rd Collimator | 1.0 |
| L4 | m | Analyzer-detector distance. Contains 4th Collimator | 0.8 |

## Links

- [Source code](ILL_H25_IN22.instr) for `ILL_H25_IN22.instr`.
- The <a href="http://www.ill.fr/in22">IN22 TAS</a> at the ILL

---
