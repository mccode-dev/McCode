# The `Mirror_validation` Instrument

*McStas: Comparison of Mirror component and Union version*

## Identification

- **Site:** ESS
- **Author:** Mads Bertelsen (mads.bertelsen@ess.eu)
- **Origin:** ESS
- **Date:** October 2025

## Description

```text
Comparison of Mirror component and a similar system made with Union components
though with a key difference that a substrate is simulated which results in two
effects: At large angles where no reflection occur, there will be some intensity
from the scattering in the substrate. At very small angles, the reflectivity can
exceed the R0 of the mirror coating, as the ray can be reflected by the Ni
substrate. NCrystal is used to simulate the Ni substrate.
The instrument includes a detector that ignores scattered rays, which should
produce identical results for large angles, though there can still be a tiny
discrepancy at small angles especially if R0 of the mirror coating is lowered.

Example: mcrun Mirror_validation.instr angle=0.7
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| angle | deg | Rotation of sample | 0.4 |
| ref_component | 1 | 0 to use Union components, 1 for reference Mirror component | 0 |
| R0_value | 1 | Low-angle reflectivity | 0.99 |
| m_value | 1 | m-value of material | 3 |
| Qc_value | AA-1 | Critical scattering vector | 0.0219 |
| alpha_value | AA | Slope of reflectivity | 6.07 |
| W_value | AA-1 | Width of supermirror cut-off | 0.003 |

## Links

- [Source code](Mirror_validation.instr) for `Mirror_validation.instr`.
- A reference/HTML link for more information

---
