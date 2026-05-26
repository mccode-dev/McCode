# The `ISIS_LET` Instrument

*McStas: LET instrument on ISIS TS2

Instrument: ISIS_LET*

## Identification

- **Site:** ISIS
- **Author:** Ross Stewart
- **Origin:** ISIS
- **Date:** September 2025

## Description

```text
This is a simulation of LET on ISIS TS2.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Ei | meV | Centre energy for moderator | 3.7 |
| dE |  | Multiplier for energy spread:  Emin = Ei/dE, Emax = Ei*dE | 1.1 |
| Ch3_speed | Hz | Chopper 3 frequency | 100 |
| Ch5_speed | Hz | Chopper 5 frequency | 200 |
| Ch2_phase | mus | Chopper 2 phase setting | 95000 |
| pha_offset | mus | Offset in time for moderator focus | 80e-6 |
| res | string | "HF" - High Flux, "HR" - High Resolution, "I" - Intermediate | "HF" |
| snout | string | "in" or "out" | "out" |
| monitors_on | 1 | Flag to enable/disable TOF monitors in primary optics | 0 |
| movable_monitors | 1 | Flag to enable/disable PSDs and Div monitors in primary optics | 0 |

## Links

- [Source code](ISIS_LET.instr) for `ISIS_LET.instr`.

---
