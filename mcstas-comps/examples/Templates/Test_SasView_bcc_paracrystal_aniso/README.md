# The `test_SasView_bcc_paracrystal` Instrument

*McStas: Test instrument for the SasView bcc_paracrystal model component as sample description.*

## Identification

- **Site:** Templates
- **Author:** Jose Robledo
- **Origin:** FZJ / DTU / ESS DMSC
- **Date:** 

## Description

```text
Very simple test instrument for the SasView_bcc_paracrystal component


Example: model_scale=1 Detector: detector_I=83.8252
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons. | 6 |
| dlambda | AA | Wavelength spread of neutrons. | 0.05 |
| dnn |  | ([-inf, inf]) Nearest neighbour distance. | 220 |
| d_factor |  | ([-inf, inf]) Paracrystal distortion factor. | 0.06 |
| radius |  | ([0, inf]) Particle radius. | 40 |
| sld |  | ([-inf, inf]) Particle scattering length density. | 4 |
| sld_solvent |  | ([-inf, inf]) Solvent scattering length density. | 1 |
| theta |  |  | 60 |
| phi |  |  | 60 |
| Psi |  |  | 60 |
| model_scale |  | Global scale factor for scattering kernel. For systems without inter-particle interference, the form factors can be related to the scattering intensity by the particle volume fraction. | 1.0 |
| model_abs |  | Absorption cross section density at 2200 m/s. | 0.0 |
| xwidth | m | ([-inf, inf]) Horiz. dimension of sample, as a width. | 0.01 |
| yheight | m | ([-inf, inf]) vert . dimension of sample, as a height for cylinder/box | 0.01 |
| zdepth | m | ([-inf, inf]) depth of sample | 0.005 |
| R | m | Outer radius of sample in (x,z) plane for cylinder/sphere. | 0 |
| target_index |  | Relative index of component to focus at, e.g. next is +1. | 1 |
| target_x | m | relative focus target position. | 0 |
| target_y | m | relative focus target position. | 0 |
| target_z | m | relative focus target position. | 1 |
| focus_xw | m | horiz. dimension of a rectangular area. | 0.5 |
| focus_yh | m | , vert. dimension of a rectangular area. | 0.5 |
| focus_aw | deg | , horiz. angular dimension of a rectangular area. | 0 |
| focus_ah | deg | , vert. angular dimension of a rectangular area. | 0 |
| focus_r |  |  | 0 |

## Links

- [Source code](Test_SasView_bcc_paracrystal_aniso.instr) for `Test_SasView_bcc_paracrystal_aniso.instr`.

---
