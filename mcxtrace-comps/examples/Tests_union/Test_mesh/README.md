# The `Test_mesh` Instrument

*McXtrace: Union_mesh sample compared to the same sample as a Union_box*

## Identification

- **Site:** Tests_union
- **Author:** Peter Willendrup
- **Origin:** DTU Physics
- **Date:** October 2026

## Description

```text
A 50 x 50 x 10 mm slab of an incoherent scatterer, read from box.off with
Union_mesh (use_mesh=1) or given as a Union_box (use_mesh=0). The slab sits
in a vacuum Union_sphere and is cut by a vacuum Union_cylinder, so the mesh
within/overlap functions against the basic shapes are used. The two settings
should give the same intensities.
```

## Examples

- **Test: use_mesh=1 Detector: psd_trans_I=7.79e-05**
- **Test: use_mesh=0 Detector: psd_trans_I=7.79e-05**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| use_mesh | 1 | 1: slab from box.off with Union_mesh, 0: slab as Union_box | 1 |

## Links

- [Source code](Test_mesh.instr) for `Test_mesh.instr`.

---
