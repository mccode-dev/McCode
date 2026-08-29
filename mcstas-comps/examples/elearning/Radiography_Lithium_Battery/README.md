# The `Radiography_Lithium_Battery` Instrument

*McStas: Instrument to study radiographic imaging of a LiFePO4 battery under charge/discharge. Developed for PaNOSC (EU H2020 GA No. 823852).*

## Identification

- **Site:** elearning
- **Author:** Mads Bertelsen and Viktor L. Holm
- **Origin:** University of Copenhagen
- **Date:** 2020-2021

## Description

```text
Instrument to study radiographic imaging of a LiFePO4 battery under charge/discharge. Developed for PaNOSC (EU H2020 GA No. 823852).

Nota Bene: For simpification, this version of the instrument does <strong>NOT</strong> currently react to setting "battery_charge".

Radiography_Lithium_Battery was derived from Radiography_absorbing_edge. by Linda Udby and Peter Willendrup.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| D | m | Diameter of pinhole before sample | 0.01 |
| L | m | Distance between pinhole and sample | 5 |
| l | m | Distance between sample and detector | 0.10 |
| sample_used | 1 | Flag to indicate if battery stack sample (0) or single-slab calibration sample (1) is in the beam | 0 |
| battery_charge | 1 | Level of charge of battery (Currently not functional!) | 1.0 |

## Links

- [Source code](Radiography_Lithium_Battery.instr) for `Radiography_Lithium_Battery.instr`.
- Originally developed for the <a href="https://e-learning.pan-training.eu/">PaNOSC e-learning portal</a>

---
