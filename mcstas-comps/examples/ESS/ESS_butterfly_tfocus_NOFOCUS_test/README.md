# The `ESS_butterfly_tfocus_NOFOCUS_test` Instrument

*McStas: Test instrument for the updated BF1 butterfly moderator design*

## Identification

- **Site:** ESS
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** ESS
- **Date:** 2016-08-24

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
| index | 1 | Target index for source focusing. Defaults to illuminate the "cold collimated" brilliance monitor, thereby suppressing "dist" | 0 |
| dist | m | Distance from origin to focusing rectangle; at (0,0,dist) - alternatively use target_index | 100 |
| cold | 1 | Defines the statistical fraction of events emitted from the cold part of the moderator | 0.5 |
| Yheight | m | Defines the moderator height. Valid values are 0.03 m and 0.06 m | 0.03 |
| delta | m | Parameter that allows to scan "collimator" position | 0 |
| tfocus_dist |  |  | 10 |
| tfocus_time |  |  | 0.01 |
| tfocus_width |  |  | 0.001 |

## Links

- [Source code](ESS_butterfly_tfocus_NOFOCUS_test.instr) for `ESS_butterfly_tfocus_NOFOCUS_test.instr`.
- <reference/HTML link>
- Benchmarking website available at <a href="http://ess_butterfly.mcstas.org">http://ess_butterfly.mcstas.org</a>

---
