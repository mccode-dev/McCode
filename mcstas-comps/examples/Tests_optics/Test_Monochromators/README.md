# The `Test_Monochromators` Instrument

*McStas: Compares intensities of Monochromator components.*

## Identification

- **Site:** Tests_optics
- **Author:** Peter Christiansen
- **Origin:** RISOE.
- **Date:** July 2006.

## Description

```text
Very simple setup to compare intensities diffracted by Monochromators.
It shows that implementations are equivalent.

PG 002 oriented examples:
```

## Examples

- **Test: Mono=1 Detector: psd1_I=8.9e-05**
- **Test: Mono=2 Detector: psd1_I=8.9e-05**
- **Test: Mono=3 Detector: psd1_I=8.9e-05**
- **Test: Mono=4 Detector: psd1_I=8.9e-05**
- **Test: Mono=5 Detector: psd1_I=8.9e-05**
- **Test: Mono=6 Detector: psd1_I=1.0e-04**
- **Test: Mono=6 PG=1 Detector: psd1_I=1.3e-04**
- **Test: Mono=6 powder=1 Moz=60 Detector: psd1_I=4.5e-07**
- **Test: Mono=6 ay=2.13389 az=-1.232 bz=2.464 cx=6.711 Detector: psd1_I=1.0e-04**
- **Test: Mono=6 ay=2.94447 by=1.47224 bz=2.54999 cx=0.936252 recip=1 Detector: psd1_I=1.0e-04**
- **Test: Mono=7 ay=2.94447 by=1.47224 bz=2.54999 cx=0.936252 recip=1 Detector: psd1_I=9.6e-05**
- SiO2 lattice orientation examples:
- Example: Mono=1 DM=4.25504 lmin=0.1 lmax=7.0 lambda=6.0
- Example: Mono=6 DM=4.25504 lmin=0.1 lmax=7.0 lambda=6.0 reflections=SiO2_quartza.lau  ax=4.3271 az=-2.49825 bz=4.9965 cy=-5.4543
- Example: Mono=6 DM=4.25504 lmin=0.1 lmax=7.0 lambda=6.0 reflections=SiO2_quartza.lau recip=1 ax=1.45205 bx=0.726027 bz=1.25752 cy=-1.15197
- Example: Mono=7 DM=4.25504 lmin=0.1 lmax=7.0 lambda=6.0 NXrefs=SiO2-alpha_sg154_AlphaQuartz.ncmat recip=1 ax=1.45205 bx=0.726027 bz=1.25752 cy=-1.15197

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Mono | 1 | Choice of Monochromator component to use, with 1=Monochromator_flat 2=Monochromator_pol 3=Monochromator_pol (forcing 1% events to be reflected) 4=Monochromator_curved (in flat mode) 5=Monochromator_2foc (contrib, flat mode) 6=Single_crystal 7=NCrystal | 1 |
| lmin | Angs | Minimum wavelength produced from source | 1.6 |
| lmax | Angs | Maximum wavelength produced from source | 2.4 |
| Moz | arcmin | Mosaicity | 40 |
| reflections | str | input-file for Single_crystal (Mono=6) | "C_graphite.lau" |
| NXrefs | str | input-file for NCrystal (Mono=7) | "C_sg194_pyrolytic_graphite.ncmat" |
| DM | Angs | Mono lattice spacing (should be chosen compatible with file-unput when Mono=6/7 | 3.3539 |
| PG | 1 | PG-mode for Single_crystal (range 0-1) | 0 |
| powder | 1 | Powder-mode for Single_crystal | 0 |
| order | 1 | Maximum order of scattering in Single_crystal | 0 |
| ax | Angs or 1/Angs | x-coordinate of 1st direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| ay | Angs or 1/Angs | y-coordinate of 1st direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| az | Angs or 1/Angs | z-coordinate of 1st direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| bx | Angs or 1/Angs | x-coordinate of 2nd direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| by | Angs or 1/Angs | y-coordinate of 2nd direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| bz | Angs or 1/Angs | z-coordinate of 2nd direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| cx | Angs or 1/Angs | x-coordinate of 3rd direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| cy | Angs or 1/Angs | y-coordinate of 3rd direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| cz | Angs or 1/Angs | x-coordinate of 3rd direct/recip lattice unit cell vector (depends on recip setting) | 0 |
| recip | 1 | Flag to indicate if ax/y/z, bx/y/z, cx/y/z lattice corrdinates are in direct (=0) or reciprocal space units | 0 |
| dthick | m | Thickness of mono slab in Mono=6/7 cases | 0.0015 |
| p_transmit | 1 | Probability of transmission in Single_crystal Mono=6 case | 0.001 |
| lambda | Angs | Target mochromator wavelength | 2.0 |

## Links

- [Source code](Test_Monochromators.instr) for `Test_Monochromators.instr`.

---
