# The `ESS_MCPL_generate_filtered` Instrument

*McStas: Instrument meant to 'filter' legacy target division MCPL files.*

## Identification

- **Site:** ESS
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** ESS
- **Date:** 2016-09-26

## Description

```text
Instrument meant to 'filter' legacy target division MCPL files (i.e. S2.mcpl.gz -> S2_filtered.mcpl.gz) for use with
e.g. the ESS_butterfly_MCPL_test instrument and filter=1.

To function, the instrument needs a local copy of the MCPL description you want to work with, e.g. from
https://public.esss.dk/users/willend/MCPL/

The instrument assumes that binary MCPL datasets are available in . named [sector][beamline].mcpl.gz, i.e. W8.mcpl.gz.
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
| allmons | 1 | Flag to include also debugging-monitor output | 0 |

## Links

- [Source code](ESS_MCPL_generate_filtered.instr) for `ESS_MCPL_generate_filtered.instr`.
- <reference/HTML link>
- Benchmarking website available at <a href="http://ess_butterfly.mcstas.org">http://ess_butterfly.mcstas.org</a>

---
