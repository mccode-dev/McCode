# The `Test_MCPL_input_once` Instrument

*McStas: A test instrument for MCPL_input_once*

## Identification

- **Site:** Tests_MCPL_etc
- **Author:** Peter Willendrup <erkn@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** Mar 2024

## Description

```text
This is a unit test for the MCPL_input_once component. Since Oct 2026 MCPL_input_once
is an INHERIT of MCPL_input with repeat_count=1 and replicate_across_processes=0 forced:
the file content is shared among MPI processes and every particle is used once.
```

## Examples

- **Test: -n1e3 v_smear=0.1 MCPLFILE=voutput.mcpl.gz Detector: m1_I=2.42284e+11**
- **Test: -n1e3 v_smear=0.1 MCPLFILE=voutput_legacy  Detector: m1_I=2.42284e+11**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| v_smear | 1 | Mmake Gaussian MC choice on particle velocity with spread v_smear * particle velocity | 0.1 |
| pos_smear | m | Make uniform MC choice on sphere of radius pos_spear around particle position | 0.001 |
| dir_smear | deg | Make Gaussian MC choice in cone of opening dir_smear around particle direction | 0.01 |
| MCPLFILE | str | Input MCPL file. | "voutput.mcpl" |

## Links

- [Source code](Test_MCPL_input_once.instr) for `Test_MCPL_input_once.instr`.

---
