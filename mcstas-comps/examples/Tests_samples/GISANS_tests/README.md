# The `GISANS_tests` Instrument

*McStas: Instrument to test features of newly developed sample model for GISANS from H. Frielinghaus.*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup (DTU/ESS) and Henrich Frielinghaus (MLZ)
- **Origin:** MLZ
- **Date:** 2023-2024

## Description

```text
Instrument to test features of newly developed sample model for GISANS from H. Frielinghaus,
developed to model the GISANS features described in M. S. Hellsing et. al [1]

Test case 1, grazing incidence from front
```

## Examples

- **Test: mcrun GISANS_tests.instr mode=1 Detector: DETfin_I=66.7514**
- Test case 2, grazing incidence from back
- **Test: mcrun GISANS_tests.instr mode=2 Detector: DETfin_I=418.947**
- Test case 3, transmission, with beamstop
- **Test: mcrun GISANS_tests.instr mode=3 BEAMSTOP=1 Detector: DETfin_I=4.93923e-05**
- Test case 4, transmission, without beamstop
- **Test: mcrun GISANS_tests.instr mode=3 BEAMSTOP=0 Detector: DETfin_I=0.000247253**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| mode | 1 | Test mode selection | 1 |
| BEAMSTOP | 1 | If non-zero, insert the beamstop | 0 |

## Links

- [Source code](GISANS_tests.instr) for `GISANS_tests.instr`.
- [1] M. S. Hellsing et. al "Crystalline order of polymer nanoparticles over large areas at solid/liquid interfaces" Appl. Phys. Lett. 100, 221601 (2012) https://doi.org/10.1063/1.4723634

---
