# The `ILL_H53_IN14` Instrument

*McStas: IN14 cold triple-axis machine (TAS) on guide H53 with sample*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 2006

## Description

```text
IN14 is a high flux cold neutron three-axis spectrometer situated on a straight
58Ni coated guide looking at the horizontal cold source. IN14 is equipped with
a vertically curved (002) pyrolytic graphite monochromator. A polarising bender
can be inserted after the monochromator, for polarised neutron work.

The sample, analyser and detector units are mounted on standard Tanzboden
modules and the distance between these units may be changed if necessary. For
sample positioning, two motorized arcs (+/- 20deg), and two horizontal
translation tables (+/- 10 mm) are available. Soller collimators can be easily
inserted before and after the sample, and before the detector.

Three horizontally focusing analysers are available: (002) pyrolytic graphite,
bent Si(111), and Heusler (111) for polarised neutrons.

In this TAS configuration, PG002 is used as monochromator analyser,
with a single type detector.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| KI | Angs-1 | central wavevector for incoming neutrons | 1.55 |
| QM | Angs-1 | wavevector transfer modulus at the sample | 1.0 |
| EN | meV | energy transfer at the sample | 0.0 |
| verbose |  | toggle verbose mode | 1 |
| WM | m | Width of monochromator | 0.15 |
| HM | m | Height of monochromator | 0.12 |
| NHM | 1 | Number of vertical slabs composing the monochromator | 1 |
| NVM | 1 | Number of horizontal slabs composing the monochromator | 9 |
| RMV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| DM |  |  | 3.355 |
| WA | m | Width of analyzer | 0.20 |
| HA | m | Height of analyzer | 0.10 |
| NHA | 1 | Number of horizontal slabs composing the analyzer | 11 |
| NVA | 1 | Number of vertical slabs composing the analyzer | 1 |
| RAV | m | Analyzer vertical curvature, 0 for flat, -1 for automatic setting | 0 |
| RAH | m | Analyzer horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| DA | AA | Analyzer lattice spacing | 3.355 |
| SM | 1:left, -1:right | Scattering sense of beam from Monochromator | 1 |
| SS | 1:left, -1:right | Scattering sense of beam from Sample | -1 |
| SA | 1:left, -1:right | Scattering sense of beam from Analyzer | 1 |
| L1 | m | Source-Monochromator distance. Contains 1st Collimator | 7.0 |
| L2 | m | Monochromator-Sample distance. Contains 2nd Collimator | 2.12 |
| L3 | m | Sample-Analyzer distance. Contains 3rd Collimator | 1.37 |
| L4 | m | Analyzer-detector distance. Contains 4th Collimator | 0.7 |

## Links

- [Source code](ILL_H53_IN14.instr) for `ILL_H53_IN14.instr`.
- The <a href="http://www.ill.fr/in14">IN14 TAS</a> at the ILL

---
