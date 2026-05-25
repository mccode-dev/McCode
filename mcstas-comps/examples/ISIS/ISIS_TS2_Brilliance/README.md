# The `ISIS_TS2_Brilliance` Instrument

*McStas: This instrument produces brilliance curves from the ISIS TS2 facility.*

## Identification

- **Site:** ISIS
- **Author:** Peter Willendrup
- **Origin:** DTU Fysik
- **Date:** 20130425

## Description

```text
This instrument produces brilliance curves from the ISIS TS2 facility.

The Brilliance_monitor is used to determine both the mean and peak brilliances, plus pulse-shapes for different wavelengths.

Example: ISIS_brilliance Detector: Brillmon_I=3.80e+15 (First detector output)
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| modfile | string | Name of moderator face - TS2=groove,hydrogen,narrow,broad - - TS1=Water,ch4,h2,merlin or instrument name eg maps, crisp etc. | "hydrogen" |

## Links

- [Source code](ISIS_TS2_Brilliance.instr) for `ISIS_TS2_Brilliance.instr`.

---
