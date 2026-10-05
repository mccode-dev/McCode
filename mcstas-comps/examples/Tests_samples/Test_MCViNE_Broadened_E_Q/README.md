# The `Test_MCViNE_Broadened_E_Q` Instrument

*McStas: Test of the MCViNE Broadened_E_Q kernel (Gaussian-broadened dispersion E(Q)), as a Union process and as a standalone sample*

## Identification

- **Site:** Tests_samples
- **Author:** Fahima Islam
- **Origin:** MCViNE (https://github.com/mcvine/mcvine) kernels ported to McStas
- **Date:** 2026-09-29

## Description

```text
Monochromatic beam on a small sample described by the MCViNE Broadened_E_Q kernel.
comp_select=1 uses the Union process MCViNE_Broadened_E_Q_process (geometry, absorption
and multiple scattering by Union_master); comp_select=2 uses the standalone
component MCViNE_Broadened_E_Q with the same geometry, absorption and up to 20 orders of
scattering. The two should give the same scattered intensity (see the two
example lines below).
```

## Examples

- **Test: comp_select=1 Detector: detector_I=5.69779e-07**
- **Test: comp_select=2 Detector: detector_I=5.67965e-07**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp_select | 1 | 1: Union process MCViNE_Broadened_E_Q_process in Union_master, 2: standalone MCViNE_Broadened_E_Q | 1 |
| Ei | meV | Incident energy | 60 |

## Links

- [Source code](Test_MCViNE_Broadened_E_Q.instr) for `Test_MCViNE_Broadened_E_Q.instr`.

---
