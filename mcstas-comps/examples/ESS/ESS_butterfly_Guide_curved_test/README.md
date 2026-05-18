# The `ESS_butterfly_Guide_curved_test` Instrument

*McStas: Test instrument for the updated BF1 butterfly moderator design*

## Identification

- **Site:** ESS
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** ESS
- **Date:** 2016-09-26

## Description

```text
Test instrument for the updated BF1 butterfly moderator design.

The below example gives a 50-50 (statistics-wise) cold/thermal beam at beamline N10.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| sector | str | Defines the 'sector' of your instrument position. Valid values are "N","S","E" and "W" | "N" |
| beamline | 1 | Defines the 'beamline number' of your instrument position. Valid values are 1..10 or 1..11 depending on sector | 1 |
| Lmin | AA | Minimum wavelength simulated | 0.2 |
| Lmax | AA | Maximum wavelength simulated | 20 |
| c_performance | 1 | Cold brilliance scalar performance multiplicator c_performance > 0 | 1 |
| t_performance | 1 | Thermal brilliance scalar performance multiplicator t_performance > 0 | 1 |
| index | 1 | Target index for source focusing. Defaults to illuminate the "cold collimated" brilliance monitor, thereby suppressing "dist" | 25 |
| dist | m | Distance from origin to focusing rectangle; at (0,0,dist) - alternatively use target_index | 0 |
| cold | 1 | Defines the statistical fraction of events emitted from the cold part of the moderator | 1 |
| Yheight | m | Defines the moderator height. Valid values are 0.03 m and 0.06 m | 0.03 |
| delta | m | Parameter that allows to scan "collimator" position | 0 |
| thres |  | Weight-threshold, neutrons with higher weight are absorbed | 0.003 |
| repeat |  | Number of MCPL file repetitions | 1 |
| v_smear | 1 | When repeating events, make a Gaussian MC choice within v_smear*V around particle velocity V | 0.1 |
| pos_smear | m | When repeating events, make a flat MC choice of position within pos_smear around particle starting position | 0.01 |
| dir_smear | deg | When repeating events, make a Gaussian MC choice of direction within dir_smear around particle direction | 0.01 |
| n_pulses | 1 | Number of pulses to simulate | 1 |

## Links

- [Source code](ESS_butterfly_Guide_curved_test.instr) for `ESS_butterfly_Guide_curved_test.instr`.
- <reference/HTML link>
- Benchmarking website available at <a href="http://ess_butterfly.mcstas.org">http://ess_butterfly.mcstas.org</a>

---
