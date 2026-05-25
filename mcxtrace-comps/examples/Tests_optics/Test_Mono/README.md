# The `Test_Mono` Instrument

*McXtrace: Compares intensities of Monochromator components.*

## Identification

- **Site:** Tests_optics
- **Author:** A. Vickery, A. Prodi and E. Knudsen
- **Origin:** NBI,Ris&oslash; DTU Physics
- **Date:** July 2011.

## Description

```text
Very simple setup to compare intensities diffracted by Monochromators.
It shows that implementations are equivalent.

The input parameter 'Mono' chooses which model to use:
0: Obsolete Perfect_crystal
1: Bragg_crystal - validated component
2: Bragg_crystal_bent - bent version of the above
3: Bragg_crystal_BC - variant of the above based on Mendenhall et. al., J. Appl. Cryst 2019
4: Bragg_crystal_bent_BC - variant of the above based on Mendenhall et. al., J. Appl. Cryst 2019
5: Bragg_crystal_simple - a simple mono with a static (flat) Darwin width.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Mono |  | Which monochromator model to use. See description above. | 1 |
| lambda | AA | Source wavelength. | 1.0 |
| L1 | m | Distance from source to monochromator. | 10.0 |
| OMM | deg | Rotation angle of monochromator, i.e. angle of incidence. | 14.2208 |
| TTM | deg | Scattering angle at which to put the detector. | 28.4416 |
| BEND | m | Curvature radius of the monochromator crystal lattice. Only relevant for Mono==2. | 1 |
| monsiz | m | Size (h and w) of PSD measuring diffracted beam. | 5e-5 |

## Links

- [Source code](Test_Mono.instr) for `Test_Mono.instr`.

---
