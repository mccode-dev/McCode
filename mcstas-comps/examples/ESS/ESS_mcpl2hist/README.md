# The `ESS_mcpl2hist` Instrument

*McStas: Utility instrument that generates a set of histograms from an MCPL input file*

## Identification

- **Site:** ESS
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** ESS
- **Date:** 2016-12-02

## Description

```text
Utility instrument that generates a set of histograms from an MCPL input file.

Generates energy, wavelength, position, veloicty and time histograms via Monitor_nD.

Assumes access to binary MCPL datasets in . named [sector][beamline].mcpl.gz, i.e. W8.mcpl.gz.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| sector | str | Defines the 'sector' of your instrument position. Valid values are "N","S","E" and "W" | "N" |
| beamline | 1 | Defines the 'beamline number' of your instrument position. Valid values are 1..10 or 1..11 depending on sector | 1 |
| filter | 1 | Flag to define if the filtered or nonfiltered version of the MCPL file should be used. | 0 |
| thres | 1 | Weight threshold above which neutrons are absorbed | 0.003 |

## Links

- [Source code](ESS_mcpl2hist.instr) for `ESS_mcpl2hist.instr`.
- <reference/HTML link>
- Benchmarking website available at <a href="http://ess_butterfly.mcstas.org">http://ess_butterfly.mcstas.org</a>

---
