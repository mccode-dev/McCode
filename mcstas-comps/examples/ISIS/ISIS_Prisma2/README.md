# The `ISIS_Prisma2` Instrument

*McStas: Simple simulation of PRISMA2 with RITA-style analyser backend.*

## Identification

- **Site:** ISIS
- **Author:** Kristian Nielsen and Mark Hagen
- **Origin:** ISIS/Risoe
- **Date:** August 1998.

## Description

```text
Demonstrates how the standard components from the component library
may be easily modified for special purposes; in this case to have
the individual analyser blades paint a "color" on the neutrons to
differentiate them in the detector.

Output is in the file "prisma2.tof". The format is ASCII; each
line consists of the time-of-flight in microseconds followed by seven
intensities of neutrons from each individual analyser blade.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| TT | deg | Take-off angle at the sample position, aka A4 | -30 |
| PHA | deg | Analyzer group rotation angle, aka A5 | 22 |
| PHA1 | deg | Analyzer 1 tilt angle | -3 |
| PHA2 | deg | Analyzer 2 tilt angle | -2 |
| PHA3 | deg | Analyzer 3 tilt angle | -1 |
| PHA4 | deg | Analyzer 4 tilt angle | 0 |
| PHA5 | deg | Analyzer 5 tilt angle | 1 |
| PHA6 | deg | Analyzer 6 tilt angle | 2 |
| PHA7 | deg | Analyzer 7 tilt angle | 3 |
| TTA | deg | Take-off angle at the analyzer position, aka A6 | 4 |

## Links

- [Source code](ISIS_Prisma2.instr) for `ISIS_Prisma2.instr`.
- The McStas User manual
- <a href="http://www.isis.rl.ac.uk/excitations/prisma/">PRISMA</a>

---
