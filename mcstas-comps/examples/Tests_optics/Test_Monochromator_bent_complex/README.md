# The `Test_Monochromator_bent_complex` Instrument

*McStas: A test instrument that highlights Monochromator Bent complex's capabilities*

## Identification

- **Site:** Tests_optics
- **Author:** Jakob Lass (jakob.lass@psi.ch) and modified by Daniel Lomholt Christensen (daniel.lomholt.2000@gmail.com)
- **Origin:** PSI/LSN
- **Date:** 20250702

## Description

```text
This is a first draft for the WARP instrument proposal, as created by
Daniel Gabriel Mazzone (daniel.mazzone@psi.ch) and Jakob Lass
(jakob.lass@psi.ch). This instrument introduces how to use the Monochromator_bent_complex
component in an instrument draft.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| sample_x | m | Source horz illumination | 0.005 |
| sample_y | m | Source vert illumination | 0.015 |
| L0 | m | Z-displacement of detector wrt. rotation point | 1.38000 |
| L1 | m | Y-displacement of detector wrt. rotation point | 2.47143 |
| Ld | m | Detector width | 2 |
| det_rot | m | Detector rotation | -6.00000 |
| mos | arcmin | Monochromator mosaicity | 60 |
| extra | deg | Perturbation rotation angle | 0 |
| use_single_plane | flag | Flag indicating whether the crystal should be instantiated with a single plane of reflection, instead of 92 planes | 0 |

## Links

- [Source code](Test_Monochromator_bent_complex.instr) for `Test_Monochromator_bent_complex.instr`.

---
