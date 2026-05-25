# The `SANS_KWS2_AnySample` Instrument

*McStas: KWS2 SANS instrument at FZ-Juelich. 2 detectors. 4 available sample models.*

## Identification

- **Site:** FZ_Juelich
- **Author:** Henrich Frielinghaus
- **Origin:** FZ-Juelich/FRJ-2/IFF/KWS-2
- **Date:** Sept 2004

## Description

```text
KWS2 SANS instrument at FZ-Juelich. Custom sample (None, Guinier, Debye or Any).
Sample is at 40 m from source. 2 detectors.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons | 7.0 |
| dlambda | AA | Wavelength spread of neutrons | 0.7 |
| FLUX | n/s/cm2/st | incoming neutron flux | 1e8 |
| NGblen | m | collimation width/height | 0.05 |
| sample | int | type of sample, as 0=None, 1='AnySample', 2='Debye' or 3='Guinier' | 0 |
| Clen | m | distance to collimation in 0-20. Sample is at 40 m from source | 10.0 |
| Dlen | m | distance from sample to detector | 10.0 |

## Links

- [Source code](FZJ_SANS_KWS2_AnySample.instr) for `FZJ_SANS_KWS2_AnySample.instr`.

---
