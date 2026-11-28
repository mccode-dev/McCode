# The `JJ_SAXS` Instrument

*McXtrace: Crude model of a laboratory SAXS-instrument mimicking the type sold by SAXSlab/JJ-Xray Systems.*

## Identification

- **Site:** SAXSLAB
- **Author:** Erik Knudsen (erkn@risoe.dtu.dk) and E. Farhi
- **Origin:** Risø DTU, (Finnair flight AY67 to Hong Kong)
- **Date:** September 24th, 2009

## Description

```text
This is a very sketchy model of a SAXS-system, related to the old design of JJ X-ray systems.
It has been superseeded by the SAXS_saxslab model.
```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L | m | KB focus, e.g. Distance to the sample | 2 |
| theta | rad | Mirror glancing angle | 0.00275 |
| R | m | Mirror curvature radius | 0 |
| mirror_length | m | Length of each KB mirror | 0.5 |
| source2kb | m | Distance from source to KB | 0.6 |
| E0 | keV | Nominal energy at the source | 6.7 |
| dE | keV | Energy half-bandwidth at the source | 0.5 |
| detector_pos | m | distance from sample to detector | 2 |

## Links

- [Source code](JJ_SAXS.instr) for `JJ_SAXS.instr`.

---
