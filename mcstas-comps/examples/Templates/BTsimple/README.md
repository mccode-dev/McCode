# The `BTsimple` Instrument

*McStas: Example instrument showing how to calculate brilliance transfer using L_monitor's and WHEN statements*

## Identification

- **Site:** Templates
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** 2018/03/21

## Description

```text
Example instrument showing how to calculate brilliance transfer using L_monitor's and WHEN statements
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Central wavelength produced at the source | 10 |
| dlambda | AA | Halfwidth of wavelength band produced at the source | 9.9 |
| maxhd | deg | Maxiamal horzizontal divergence accepted | 3 |
| maxvd | deg | Maxiamal vertical divergence accepted | 3 |
| gw | m | Opening-width of ellipitic guide | 0.1 |
| gh | m | Opening-height of ellipitic guide | 0.1 |
| gL | m | Length of ellipitic guide | 50 |
| gm | 1 | M-value of the guide coating | 6 |
| delta1 | m | Optional focal displacement of guide wrt source | 0 |
| delta2 | m | Optional focal displacement of guide wrt sample position | 0 |
| Nbins | 1 | Number of bins on Wavelength / Brilliance transfer monitors | 101 |

## Links

- [Source code](BTsimple.instr) for `BTsimple.instr`.

---
