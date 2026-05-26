# The `Test_source_lab` Instrument

*McXtrace: Unit test instrument for the Source_lab component*

## Identification

- **Site:** Tests_sources
- **Author:** Erik B Knudsen
- **Origin:** DTU Physics
- **Date:** Jul. 8th 2014

## Description

```text
This instrument serves as a unit test for the Source_lab component.
The emon1-monitor catches the Kalpha-peaks, and emon2 the Kbeta peaks.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Emin | keV | minimal energy of the photons at the source | 1 |
| Emax | keV | maximal energy of the photons at the source | 40 |
| LO | 0/1 | activate Lorentzian line profile at the source | 1 |

## Links

- [Source code](Test_source_lab.instr) for `Test_source_lab.instr`.

---
