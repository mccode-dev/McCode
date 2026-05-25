# The `Test_Absorption` Instrument

*McXtrace: Example instrument to compare material absorption components.*

## Identification

- **Site:** Tests_samples
- **Author:** E. Farhi (emmanuel.farhi@synchrotron-soleil.fr)
- **Origin:** Synchrotron SOLEIL
- **Date:** March '21

## Description

```text
This instrument simply has a lab source, a few monitors and a set of components
to model material absorption. Material is chosen to be Mn (K-edge 6.5 keV).

This test compares the folowwing components:
- index=1: Filter
- index=2: Absorption_sample
- index=3: Abs_objects
- index=4: Fluorescence
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| index | 1 | Index of the absorption component to use in 1-4. See above. | 1 |
| material | str | Material file to use, e.g. Mn.txt | "Mn.txt" |
| E0 | keV | Mean photon energy | 6.5 |
| dE | keV | Photon energy spread, half width | 1 |

## Links

- [Source code](Test_Absorption.instr) for `Test_Absorption.instr`.

---
