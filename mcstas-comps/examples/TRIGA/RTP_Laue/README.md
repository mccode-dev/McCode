# The `RTP_Laue` Instrument

*McStas: The NR instrument installed at Reactor TRIGA PUSPATI (Malaysia)*

## Identification

- **Site:** TRIGA
- **Author:** E. Farhi and Megat Harun Al-Rashid
- **Origin:** ILL/RTP
- **Date:** June 2014

## Description

```text
This is a radiography installed on a radial beam port 3 at the Reactor TRIGA
PUSPATI (RTP). It uses a thermal beam port.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| theta | deg | horizontal rotation of the object | 0 |
| phi | deg | vertical rotation of the object | 0 |
| det_rotation | deg | detector rotation around the sample | 0 |
| reflections | str | sample configuration | "leucine.lau" |

## Links

- [Source code](RTP_Laue.instr) for `RTP_Laue.instr`.
- <a href="http://www.nuclearmalaysia.gov.my/Plant&Facilities/reactor.php">Nuclear Malaysia</a>
- M. Sufi et al., J. AppL Cryst. (1997). 30, 884-888 [doi:10.1107/S0021889897001738]

---
