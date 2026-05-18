# The `Test_Magnon_bcc_2D` Instrument

*McStas: Test instrument for the Sqq_w_monitor and Magnon_bcc components, derived from template_Laue*

## Identification

- **Site:** Tests_samples
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** June, 2018

## Description

```text

```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Central wavelength of wavelength distribution | 10 |
| dlambda | AA | Width of wavelength distribution | 9.9 |
| Rotation | deg | Sample orientation | 0 |
| inelastic | in 0:1 | Fraction of statistics to scatter inelastically | 1 |
| aa | AA | BCC lattice constant | 6.283185307179586 |
| sample_J | meV | Magnitude of sample nearest-neighbour interaction | 2 |
| TT | K | Sample temperature | 300 |
| FerroMagnet | boolean | Flag to choose if sample is FM or AFM | 0 |
| Verbose |  |  | 0 |
| imultiplier | 1 | Parameter to rescale intensity-output from Magnon comp | 1 |

## Links

- [Source code](Test_Magnon_bcc_2D.instr) for `Test_Magnon_bcc_2D.instr`.

---
