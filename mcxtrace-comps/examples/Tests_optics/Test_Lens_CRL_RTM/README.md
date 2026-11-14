# The `Test_Lens_CRL_RTM` Instrument

*McXtrace: Test of the analytic (fast=1) and per-lenslet (fast=0) absorption in Lens_CRL_RTM*

## Identification

- **Site:** Tests_optics
- **Author:** E. farhi
- **Origin:** McXtrace
- **Date:** Nov 2026

## Description

```text
A source with a spread in both position and angle illuminates a Be CRL stack.
The ray trajectories do not depend on 'fast', so with the same seed and
number of rays both modes must give the same intensity. The low energy and the
wide beam make the absorption depend strongly on y and on the angle, so an
error in any term of the analytic sum shows up as a difference.
```

## Examples

- **Test: fast=1 N=20 E0=8 Detector: det_I=0.34115**
- **Test: fast=0 N=20 E0=8 Detector: det_I=0.34115**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| fast | 1 | Lens_CRL_RTM absorption mode: 1 analytic, 0 sum over lenslets. | 1 |
| N | 1 | Number of lenslets. | 20 |
| E0 | keV | Photon energy. | 8 |

## Links

- [Source code](Test_Lens_CRL_RTM.instr) for `Test_Lens_CRL_RTM.instr`.

---
