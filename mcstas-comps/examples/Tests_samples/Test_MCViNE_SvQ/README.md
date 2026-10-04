# The `Test_MCViNE_SvQ` Instrument

*McStas: Test of the MCViNE SvQ kernel (elastic S(Qx,Qy,Qz)), as a Union process and as a standalone sample*

## Identification

- **Site:** Tests_samples
- **Author:** Fahima Islam
- **Origin:** MCViNE (https://github.com/mcvine/mcvine) kernels ported to McStas
- **Date:** 2026-09-29

## Description

```text
Monochromatic beam on a small sample described by the MCViNE SvQ kernel.
comp_select=1 uses the Union process MCViNE_SvQ_process (geometry, absorption
and multiple scattering by Union_master); comp_select=2 uses the standalone
component MCViNE_SvQ with the same geometry, absorption and up to 20 orders of
scattering. The two should give the same scattered intensity (see the two
example lines below).
```

## Examples

- **Test: comp_select=1 Detector: detector_I=3.44297e-07**
- **Test: comp_select=2 Detector: detector_I=3.43321e-07**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp_select | 1 | 1: Union process MCViNE_SvQ_process in Union_master, 2: standalone MCViNE_SvQ | 1 |
| Ei | meV | Incident energy | 60 |
| sample_rotation | deg | Rotation of the sample frame about the vertical axis | 0 |

## Links

- [Source code](Test_MCViNE_SvQ.instr) for `Test_MCViNE_SvQ.instr`.

---
