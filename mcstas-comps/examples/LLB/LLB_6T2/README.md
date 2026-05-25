# The `LLB_6T2` Instrument

*McStas: The 6T2 thermal single crystal diffractometer at the LLB.*

## Identification

- **Site:** LLB
- **Author:** <a href="mailto:xavier.fabreges@cea.fr">Xavier Fabrèges</a>
- **Origin:** <a href="http://www-llb.cea.fr">LLB (France)</a>
- **Date:** November 5th 2015.

## Description

```text
6T2 is a high flux thermal single crystal diffractometer dedicated to the study of
complex magnetic structures in condensed matter. Two monochromators are available.
It works at 0.9 and 2.4AA without polarization and at 1.4AA with incident polarization
(optional FeCo supermirror) and polarization analysis (P//z).

Monochromators:
PG       002 DM=3.355 AA (Highly Oriented Pyrolythic Graphite)
Cu       220 DM=1.278 AA
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | wavelength | 1.0 |
| phi | Deg | sample rotation along the vertical axis | 0 |
| gamma | Deg | in plane detector position | 0 |
| nu | Deg | out of plane detector position | 0 |
| monok | 1 | monochromator to use (0: PG, 1: Cu, 5: test mode) | 0 |
| samp_f | str | LAU file describing sample | "C60.lau" |

## Links

- [Source code](LLB_6T2.instr) for `LLB_6T2.instr`.

---
