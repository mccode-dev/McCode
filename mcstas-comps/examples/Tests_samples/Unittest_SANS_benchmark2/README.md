# The `Unittest_SANS_benchmark2` Instrument

*McStas: Test instrument for the SANS_benchmark2 component. No guide / velocity selector.*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup
- **Origin:** <a href="http://www.ess.eu">ESS</a>
- **Date:** April 2023

## Description

```text
Very simple test instrument for the SANS_benchmark2 component from H. Frielinghaus.
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
| modnum | int | Sample "modle number" from SANS_benchmark2 | 1 |

## Links

- [Source code](Unittest_SANS_benchmark2.instr) for `Unittest_SANS_benchmark2.instr`.

---
