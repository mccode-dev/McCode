# The `Test_source_spectra` Instrument

*McXtrace: Test instrument for the Source_spectra component*

## Identification

- **Site:** Tests_sources
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Aug. 10th, 2014

## Description

```text
A unit test instrument for the Source component that can take input from a SPECTRA-simulation
of an undulator/wiggler etc. and input it into McXtrace.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central wavelength for sampling | 12.5 |
| dE | keV | Halfwidth of sampleing window (in energy) | 1 |
| stem4d |  | Filename stem for 4D-datafiles from SPECTRA. | "sp8LU_xy" |
| flag4d |  | If zero use projection datafiles, else use 4D. | 0 |
| stemx |  | Filename stem for the x-projection data files coming from SPECTRA. | "sp8LU_x" |
| stemy |  | Filename stem for the y-projection data files coming from SPECTRA | "sp8LU_y" |

## Links

- [Source code](Test_source_spectra.instr) for `Test_source_spectra.instr`.

---
