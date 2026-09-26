# The `ESS_KVASIR` Instrument

*McStas: Model of the proposed ESS instrument KVASIR.*

## Identification

- **Site:** ESS
- **Author:** Amalie Falkenberg (McStasScript), adapted by Peter Willendrup
- **Origin:** NBI
- **Date:** May 2026

## Description

```text
Model of the proposed ESS instrument KVASIR, implemented using McStasScript /
a Jupyter notebook. The flat ESS_KVASIR.instr runs from off an MCPL file from
a BIFROST guide guide simulation and adds the secondary spectrometer from
the proposed KVASIR instrument.
```

## Examples

- **Test: t=50 Detector: det_He3_0_ToF_I=0.0302392**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| t | 1 | Number of simulated KVASIR 'wedges' | 50 |
| repeats | 1 | Repeat count for MCPL input file | 100 |

## Links

- [Source code](ESS_KVASIR.instr) for `ESS_KVASIR.instr`.

---
