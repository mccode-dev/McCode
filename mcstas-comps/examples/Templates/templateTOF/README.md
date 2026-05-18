# The `templateTOF` Instrument

*McStas: A test instrument for the S(q,w) sample, with furnace/container*

## Identification

- **Site:** Templates
- **Author:** E. Farhi
- **Origin:** ILL
- **Date:** Jan 2004

## Description

```text
This instrument is a test instrument for the S(q,w) sample.
It produces a tof-angle and q-energy detectors and also exports the
S(q,w) and S(q) data.
The sample environment is a single cylinder.
The sample container has the same shape as the sample itself, but is not
active when using a spherical sample shape (height=0).
detector is 2.5 m diameter, with 40 cm heigh, 1 inch diameter detector tubes.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | meV | incident neutron beam energy | 4.94 |
| dE | meV | incident neutron beam energy spread | 0.24 |
| dt | s | time spread from chopper distribution | 6.4e-6 |
| coh | str | sample coherent Sqw data file or NULL | "Rb_liq_coh.sqw" |
| inc | str | sample incoherent Sqw data file or NULL | "Rb_liq_inc.sqw" |
| thickness | m | thickness of sample. 0=filled | 1e-4 |
| yheight | m | height of sample. 0=sphere | 0.0168 |
| radius | m | radius of sample (outer) | 0.005 |
| container | str | container material or NULL | "Nb.laz" |
| container_thickness | m | container thickness | 50e-6 |
| environment | str | sample environment material or NULL | "Al.laz" |
| environment_radius | m | sample environment outer radius | 0.025 |
| environment_thickness | m | sample environment thickness | 2e-3 |
| dt0 |  |  | 0 |

## Links

- [Source code](templateTOF.instr) for `templateTOF.instr`.
- The Isotropic_Sqw sample
- The Samples_Isotropic_Sqw example instrument

---
