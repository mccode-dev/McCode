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
