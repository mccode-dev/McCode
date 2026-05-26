# The `MCPL2Mantid_flat` Instrument

*McStas: Instrument taking MCPL input giving Mantid-compatible NeXus output.*

## Identification

- **Site:** Tools
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** 2019-03

## Description

```text
Instrument taking MCPL input giving Mantid-compatible NeXus output.

1) The instrument coordintate system coincides with the MCPL-file coordinates.
2) The sourceMantid position is freely positionable in that space
3) The sampleMantid position is freely positionable in that space
4) The detector is a single, flat pixellated panel freely positionable in that space

An example mcpl input file corresponding to the default geometry can be generated using
templateSANS_MCPL
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| MCPLfile | str | Defines the MCPL input file to process | "my.mcpl.gz" |
| sourceX | m | sourceMantid x-position wrt. MCPL coords | 0 |
| sourceY | m | sourceMantid y-position wrt. MCPL coords | 0 |
| sourceZ | m | sourceMantid z-position wrt. MCPL coords | -10 |
| sampleX | m | sampleMantid x-position wrt. MCPL coords | 0 |
| sampleY | m | sampleMantid y-position wrt. MCPL coords | 0 |
| sampleZ | m | sampleMantid z-position wrt. MCPL coords | 0 |
| detectorX | m | nD_Mantid_0 x-position wrt. MCPL coords | 0 |
| detectorY | m | nD_Mantid_0 y-position wrt. MCPL coords | 0 |
| detectorZ | m | nD_Mantid_0 z-position wrt. MCPL coords | 6 |
| detrotX | m | nD_Mantid_0 x-rotation wrt. MCPL coords | 0 |
| detrotY | m | nD_Mantid_0 y-rotation wrt. MCPL coords | 0 |
| detrotZ | m | nD_Mantid_0 z-rotation wrt. MCPL coords | 0 |
| xwidth | m | nD_Mantid_0 xwidth | 0.6 |
| yheight | m | nD_Mantid_0 yheight | 0.6 |
| xbins | 1 | nD_Mantid_0 x-bins | 128 |
| ybins | 1 | nD_Mantid_0 y-bins | 128 |

## Links

- [Source code](MCPL2Mantid_flat.instr) for `MCPL2Mantid_flat.instr`.
- <reference/HTML link>
- MCPL website at <a href="https://mctools.github.io/mcpl/">https://mctools.github.io/mcpl/</a>

---
