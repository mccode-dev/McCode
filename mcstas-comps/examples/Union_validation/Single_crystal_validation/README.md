# The `Single_crystal_validation` Instrument

*McStas: Validation of Union single crystal against standard single_crystal.*

## Identification

- **Site:** Union_validation
- **Author:** Mads Bertelsen
- **Origin:** Johns Hopkins University, Baltimore
- **Date:** May 2016

## Description

```text
Validation of Union single crystal against standard single_crystal.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp_select | 1 | 1: Union components, 2: Single crystal | 1 |
| material_data_file | string | Sample material defintion | "YBaCuO.lau" |
| sigma_inc | barns | Sample incoherent cross-section | 2.105 |
| my_absorption_union | m^-1 | Sample mass-absorption coefficient | 8.55 |
| sigma_abs_sc | banrs | Sample absorption cross-section | 0 |
| delta_d_d |  | Sample latttice spacing variation | 1e-4 |
| mosaic | arcmin | Sample mosaicity | 5 |
| unit_cell_volume | AA | Sample unit cell volume | 173.28 |
| lam0 | AA | Source mean wavelength | 7 |
| dlam | AA | Source wavelength spread | 5 |
| xwidth | m | Sample x-dimension | 0.01 |
| yheight | m | Sample y-dimension | 0.01 |
| zdepth | m | Sample z-dimension | 0.01 |
| x_rotation_geometry | deg | Rotates union geometry component around x | 0 |
| y_rotation_geometry | deg | Rotates union geometry component around y | 0 |
| x_rotation_geometry_ref | deg | Rotates reference component around x | 0 |
| y_rotation_geometry_ref | deg | Rotates reference component around y | 0 |
| x_rotation_process | deg | Rotates crystal process (crystal orientation), x | 0 |
| y_rotation_process | deg | Rotates crystal process (crystal orientation), y | 0 |
| geometry_interact | deg | Rotates crystal process (crystal orientation), z | 0 |
| PG |  | Sample PG-mode | 0 |
| powder |  | Sample powder-mode | 0 |

## Links

- [Source code](Single_crystal_validation.instr) for `Single_crystal_validation.instr`.

---
