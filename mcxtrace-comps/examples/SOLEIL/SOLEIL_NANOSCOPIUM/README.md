# The `SOLEIL_NANOSCOPIUM` Instrument

*McXtrace: The NANOSCOPIUM tomography beam-line at Synchrotron SOLEIL.*

## Identification

- **Site:** SOLEIL
- **Author:** C.Gaudry
- **Origin:** Synchrotron SOLEIL
- **Date:** 10/2026

## Description

```text
The beamline NANOSCOPIUM
works at photon energies between 5 and 50 keV. It is dedicated to full-field
radiography and tomography in absorption and phase contrast, with pixel sizes
from 20 nm to 20 µm.

List of beamline elements. Distances are given from the center of the undulator.

Position (m) |   Element
-------------|---------------------------------------------
0            | Undulator (source)
18.109       | Primary slits
22.655       | Mirror M1 (curved) : horizontal deviation, vertical focus
24.471       | Mirror M2 (curved) : horizontal deviation, horizontal focus
75.758       | Secondary slits
77.224       | Monochromator (DCM)
82.064       | Slits Secondary source
148.893      | Slits JJ XRay 1 (entry Nanoprobe 1)  (doesn't work)
150.721      | Nano-probe 1  (doesn't work)
154.353      | Slits JJ XRay 2 (entry Nanoprobe 2)
154.856      | Mirror KB1 (elliptic) : vertical focus
154.961      | Mirror KB2 (elliptic) : horizontal focus
155.140      | Nano-probe 2
```

## Examples

- **Test: NANO_E0=11 Detector: NANO_e_dcm_out_I=4.09492e+10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| NANO_E0 | keV | Energy selected at the Undulator. | 11 |
| NANO_dE | keV | Energy spread at the Undulator. | 1 |
| NANO_Emono | keV | Energy to be selected by the monochromator, or E0 when unset. | 0 |
| NANO_coating_m2 | 1 | 2 stripes on M2 : Rh (coated : 1) or Si (uncoated : 0) | 1 |
| NANO_angle_m1_m2 | rad | Incidence angle on M1 and M2 mirrors | 2.5e-3 |
| NANO_angle_kb1 | rad | Incidence angle on the first KB mirror (vertical focus) | 3e-3 |
| NANO_angle_kb2 | rad | Incidence angle on the second KB mirror (horizontal focus) | 2.5e-3 |

## Links

- [Source code](SOLEIL_NANOSCOPIUM.instr) for `SOLEIL_NANOSCOPIUM.instr`.

---
