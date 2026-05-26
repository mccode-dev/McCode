# The `Test_V_cavity_SNAG_single_side` Instrument

*McStas: Test instrument for Transmission_V_polarisator, single layer reflectivity.*

## Identification

- **Site:** Tests_polarization
- **Author:** Michael Schneider (SNAG)
- **Origin:** SNAG
- **Date:** 2024

## Description

```text
Test instrument for Transmission_V_polarisator, single layer reflectivity.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lam | AA | Central wavelength produced at source | 7.0 |
| dLam | AA | Wavelength spread produced at source | 6.0 |
| l_guide0 |  |  | 2 |
| m_hori | 1 | m-value of feeding guide, horizontal mirrors | 1.0 |
| m_vert | 1 | m-value of feeding guide, vertical mirrors | 3.0 |
| m_pol_h | 1 | m-value of guide body, horizontal (m of FeSi is defined by data files) | 1.0 |
| m_pol_v | 1 | m-value of guide body, vertical (m of FeSi is defined by data files) | 3.0 |
| W | m | Width of guide and cavity | 0.024 |
| H | m | Height of guide and cavity | 0.058 |
| Length_C | m | Length of guide body | 1.300 |
| tap_ang | deg | Taper angle with respect to optical axis of V-cavity | 0.55 |
| distK | m | Distance between V-cavities (applicable, if 2 cavities are installed) | 1.23864 |
| d_Fe | m | Thickness of Fe in one supermirror coating | 6.95e-6 |
| d | m | Distance from source to guide entrance | 2.0 |

## Links

- [Source code](Test_V_cavity_SNAG_single_side.instr) for `Test_V_cavity_SNAG_single_side.instr`.
- P. B&ouml;ni, W. M&uuml;nzer and A. Ostermann: <a href="https://doi.org/10.1016/j.physb.2009.06.031">Physica B: Condensed Matter Volume 404, Issue 17, 1 September 2009, Pages 2620-2623</a>

---
