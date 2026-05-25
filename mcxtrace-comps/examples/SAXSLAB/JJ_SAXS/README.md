# The `JJ_SAXS` Instrument

*McXtrace: Crude model of a laboratory SAXS-instrument mimicking the type sold by SAXSlab/JJ-Xray Systems.*

## Identification

- **Site:** SAXSLAB
- **Author:** Erik Knudsen (erkn@risoe.dtu.dk)
- **Origin:** Ris&oslash; DTU, (Finnair flight AY67 to Hong Kong)
- **Date:** September 24th, 2009

## Description

```text
This is a very sketchy model of a SAXS-system, related to the old design of JJ X-ray systems.
It has been superseeded by the SAXS_saxslab model.
Limitations include:
- Focusing only in 1 direction.
- iNo reflectivity coating is implemented.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| pin2_pos | m | distance between 1st and 2nd pinhole in beam tube | 0.2 |
| pin3_pos | m | distance between 2nd and 3rd pinhole in beam tube | 0.4 |
| optic_L | m | length of the focusing optic | 0.1 |
| sample_pos | m | distance from 3rd pinhole to sample | 0.2 |
| detector_pos | m | distance from 3rd pinhole to detector | 2 |

## Links

- [Source code](JJ_SAXS.instr) for `JJ_SAXS.instr`.

---
