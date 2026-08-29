# The `ESS_butterfly_Guide_curved_test` Instrument

*McStas: Test instrument for the updated BF1 butterfly moderator design including a simple guide.*

## Identification

- **Site:** ESS
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** ESS
- **Date:** 2016-09-26

## Description

```text
Test instrument for the updated BF1 butterfly moderator design including a simple guide with 10 m straight flight-path followed by a 50 m curved guide.

The below example gives a 50-50 (statistics-wise) cold/thermal beam at beamline S2.
Example: sector=S beamline=2 cold=0.5 Detector: Monitor2_xy1_I=1.59e+11

Use the ESS_butterfly_MCPL_test instrument for direct comparison with MCPL-based source descriptions.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| sector | str | Defines the 'sector' of your instrument position. Valid values are "N","S","E" and "W" | "S" |
| beamline | 1 | Defines the 'beamline number' of your instrument position. Valid values are 1..10 or 1..11 depending on sector | 2 |
| Lmin | AA | Minimum wavelength simulated | 0.2 |
| Lmax | AA | Maximum wavelength simulated | 20 |
| c_performance | 1 | Cold brilliance scalar performance multiplicator c_performance > 0 | 1 |
| t_performance | 1 | Thermal brilliance scalar performance multiplicator t_performance > 0 | 1 |
| index | 1 | Target index for source focusing. Defaults to illuminate the "cold collimated" brilliance monitor, thereby suppressing "dist" | 0 |
| dist | m | Distance from origin to focusing rectangle; at (0,0,dist) - alternatively use target_index | 2 |
| cold | 1 | Defines the statistical fraction of events emitted from the cold part of the moderator | 0.5 |
| Yheight | m | Defines the moderator height. Valid values are 0.03 m and 0.06 m | 0.03 |
| delta | m | Parameter that allows to scan "collimator" position | 0 |
| n_pulses | 1 | Number of pulses to simulate | 1 |
| allmons | 1 | Flag to include also debugging-monitor output | 0 |

## Links

- [Source code](ESS_butterfly_Guide_curved_test.instr) for `ESS_butterfly_Guide_curved_test.instr`.
- <reference/HTML link>
- Benchmarking website available at <a href="http://ess_butterfly.mcstas.org">http://ess_butterfly.mcstas.org</a>

---
