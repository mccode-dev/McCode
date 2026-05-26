# The `templateSANS2_Mantid` Instrument

*McStas: Test instrument for the SANS_spheres2 component. No guide / velocity selector.*

## Identification

- **Site:** Mantid
- **Author:** Peter Willendrup, Wim G. Bouwman
- **Origin:** <a href="http://www.fysik.dtu.dk">DTU</a>
- **Date:** Dec 20190

## Description

```text
Very simple test instrument for the SANS_spheres2 component, derived / simplified from
H. Frielinghaus SANS_benchmark2
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons | 6 |
| dlambda | AA | Wavelength spread of neutrons | 0.05 |
| r | AA | Radius of scattering hard spheres | 150 |
| PHI | 1 | Particle volume fraction | 1e-3 |
| Delta_Rho | cm^-2 | Scattering length density | 6e10 |
| frac_dir | 1 | Fraction of statistics for direct beam | 0.03 |
| frac_inc | 1 | Fraction of statistics for incoherent scattering in scattered beam | 0.01 |

## Links

- [Source code](templateSANS2_Mantid.instr) for `templateSANS2_Mantid.instr`.

---
