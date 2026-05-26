# The `HZB_FLEX` Instrument

*McStas: Primary and secondary spectrometer for the FLEX upgrade*

## Identification

- **Site:** HZB
- **Author:** M. Skoulatos and K. Habicht, port to McStas 2.0 by Mathias Kure KU
- **Origin:** Helmholtz-Zentrum Berlin
- **Date:** 26 October 2010

## Description

```text

```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| kI | inv AA | incident wavevector | 1.55 |
| kF | inv AA | outgoing wavevector | 1.55 |
| wVS | m | Width of virtual source slit | 0.03 |
| tilt | deg | Tilt angle for velocity selector | 0 |
| SA | 1 | Analyzer scattering sense | -1 |
| A3 | deg | Sample omega angle | 0 |
| A4 | deg | Sample 2-theta angle | 70 |
| L3 | m |  | 1.00 |
| L4 | m |  | 1.00 |
| Mono_flatswitch | 1 | Flag for flat or curved monochromator | 0 |

## Links

- [Source code](HZB_FLEX.instr) for `HZB_FLEX.instr`.

---
