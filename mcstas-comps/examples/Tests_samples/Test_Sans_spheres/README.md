# The `Test_Sans_spheres` Instrument

*McStas: A small instrument to compare the Sans_spheres and SANS_spheres2 components*

## Identification

- **Site:** Test_samples
- **Author:** Daniel Lomholt Christensen
- **Origin:** Niels Bohr Institute @ UCPH
- **Date:** 18/08/2026

## Description

```text
This instrument is a small SANS instrument that simulates the Sans_spheres
component, and the SANS_spheres2 component, in order to ensure that they
give the same value in this specific case.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E_i | meV | Energy of the simulated neutrons. | 5 |
| use_SANS_spheres2 | 1 | Flag to select whether to use the Sans_spheres component or the SANS_spheres2 component. | 0 |
| improved_res | 1 | Flag to indicate whether the instrument should run in the insanely good resolution mode. | 0 |
| flux_mult | n/s | Factor that is multiplied onto the source component in order to adjust the flux. | 0.5512 |

## Links

- [Source code](Test_Sans_spheres.instr) for `Test_Sans_spheres.instr`.
- [Additional information](Test_Sans_spheres.md)

---
