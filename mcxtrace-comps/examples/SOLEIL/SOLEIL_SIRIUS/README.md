# The `SOLEIL_SIRIUS` Instrument

*McXtrace: SOLEIL SIRIUS beamline: Soft Interfaces and Resonant Investigation on Undulator Source*

## Identification

- **Site:** SOLEIL
- **Author:** FARHI Emmanuel
- **Origin:** SOLEIL
- **Date:** September 2026

## Description

```text
SIRIUS is designed for Soft Matter research (1.4 - 13 keV).
It features vertical focusing primary mirrors, a fixed-exit Si(111)
monochromator, and a deflection mirror to enable grazing incidence
measurements on liquid/solid interfaces.

Position | Element
---------|-------------------------------------------------------------
0        | Source: Undulator (U26)
14       | M1: Primary Mirror (Pd coated, vertical focusing)
16       | MDC1: 1st Crystal (Flat, Si 111)
16.01    | MDC2: 2nd Crystal (Sagittally bent, Si 111)
18       | Deflection Mirror (for grazing incidence)
30       | Sample Stage (XAS configuration)
35       | Sample Stage (SEXAFS configuration)
```

## Examples

- **Test: E0=8 Detector: def_loc_I=2.14522e+10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central energy (1.4 - 13 keV) | 8 |
| dE | keV | Energy spread | 0.1 |
| M1_angle | mrad | Incidence angle of M1 | 3 |
| M2_angle | mrad | Incidence angle of M2 | 3 |
| def_angle | mrad | Deflection mirror tilt (for grazing incidence) | 2.0 |
| sample_pos | m | Distance to sample (30 for XAS, 35 for SEXAFS) | 30 |
| sample | str | Sample structure file | "LaB6.cif" |

## Links

- [Source code](SOLEIL_SIRIUS.instr) for `SOLEIL_SIRIUS.instr`.
- Documentation: APS_BL20.pdf

---
