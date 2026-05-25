# The `Granroth_SNS_decoupled_poisoned` Instrument

*McStas: Simple instrumentfile for estimating SNS brilliance, moderator is a the Granroth SNS_source applying linear interpolation in MCNPX tables.*

## Identification

- **Site:** SNS
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU Physics
- **Date:** May 29,2013

## Description

```text
Simple instrumentfile for estimating SNS brilliance, moderator is a the Granroth SNS_source applying linear interpolation in MCNPX tables.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lambda_min | AA | Minimum wavelength produced at source | 0.5 |
| Lambda_max | AA | Maximum wavelength produced at source | 20 |
| filename | str | Source input file from $MCSTAS/data | "source_sct091_tu_02_1.dat" |

## Links

- [Source code](Granroth_SNS_decoupled_poisoned.instr) for `Granroth_SNS_decoupled_poisoned.instr`.

---
