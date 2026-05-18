# The `Test_Mirrors` Instrument

*McXtrace: Unit test instrument to check that Mirrors are working*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jul 16

## Description

```text
A mere unit test instrument. Also includes a perfectly flat Mirror as reference.
The choice of the Mirror to test is set with "index"
index=0: Mirror
index=1: Mirror_elliptic
index=2: Mirror_parabolic
index=3: Mirror_curved
index=4: Mirror_toroid
index=5: Mirror_toroid_pothole
index=6: Multilayer_elliptic
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| gamma | deg | Nominal glancing angle of mirror | 0.5 |
| index | 1 | Index of the Mirror component to test | 1 |
| L | m | Distance source-mirror and mirror-detector | 2 |
| radius | m | Radius of curvature | 1000 |
| E0 | keV | Mean photon energy | 12.5 |

## Links

- [Source code](Test_Mirrors.instr) for `Test_Mirrors.instr`.

---
