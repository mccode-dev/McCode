# The `He3_spin_filter` Instrument

*McStas: Test instrument for He3_cell*

## Identification

- **Site:** Tests_polarization
- **Author:** Pietro Tozzi and Erik B Knudsen
- **Origin:** DTU Physics et.
- **Date:** Nov 20

## Description

```text
Simple instrument model serving as an example and unit tyest of the He3-cell model.
For the values POLHE=.8 and .5 results comply with "Batz, M et al. “(3) He Spin Filter for Neutrons.”
Journal of research of the National Institute of Standards and Technology vol. 110,3 293-8. 1 Jun. 2005, doi:10.6028/jres.110.042"
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| pol |  | If "x","y", or "z" set polarization along that axis. If "0" use a zero polarization. | "y" |
| lamb_1 | AA | Lower wavelength limit to simulate | 1 |
| lamb_2 | AA | Upper wavelegnth limit to simulate | 10 |
| RANDOM |  | Randomize polarization vector in the (ideal) polariser | 0 |
| polval |  | Set polarization to (polval,polval,polval). Gets overridden by pol | 1 |
| POLHE |  | Polarization of the 3He-gas in the cell. | 0.7 |
| box |  | Flag to indicate if cell geometry is cylindrical (0) or box-shaped (1) | 0 |

## Links

- [Source code](He3_spin_filter.instr) for `He3_spin_filter.instr`.
- <reference/HTML link>

---
