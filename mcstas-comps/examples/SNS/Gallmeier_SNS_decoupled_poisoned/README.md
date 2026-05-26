# The `Gallmeier_SNS_decoupled_poisoned` Instrument

*McStas: Simple instrumentfile for estimating SNS brilliance, moderator is a the Gallmeier SNS_source_analytic applying Ikeda-Carpenter vs. Pade function fits to MCNPX tables.*

## Identification

- **Site:** SNS
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU Physics
- **Date:** May 29,2013

## Description

```text
Simple instrumentfile for estimating SNS brilliance, moderator is a the Gallmeier SNS_source_analytic applying Ikeda-Carpenter vs. Pade function fits to MCNPX tables.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lambda_min | AA | Minimum wavelength produced at source | 0.5 |
| Lambda_max | AA | Maximum wavelength produced at source | 20 |
| filename | str | Source input file from $MCSTAS/data | "a1Gw2-2-f5_fit_fit.dat" |

## Links

- [Source code](Gallmeier_SNS_decoupled_poisoned.instr) for `Gallmeier_SNS_decoupled_poisoned.instr`.

---
