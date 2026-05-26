# The `Test_MCPL_input` Instrument

*McStas: A test instrument for MCPL_input*

## Identification

- **Site:** Tests_MCPL_etc
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** Mar 2016

## Description

```text
This is a unit test for the MCPL_input component.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| repeat | 1 | Repeat the contents of the inputfile this many times. NB: When using MPI you implicitly repeat by #mpi processes | 1 |
| v_smear | 1 | When repeating, make Gaussian MC choice on particle velocity with spread v_smear * particle velocity | 0.1 |
| pos_smear | m | When repeating, make uniform MC choice on sphere of radius pos_spear around particle position | 0.001 |
| dir_smear | deg | When repeating, make Gaussian MC choice in cone of opening dir_smear around particle direction | 0.01 |
| MCPLFILE | str | Input MCPL file. | "voutput.mcpl" |

## Links

- [Source code](Test_MCPL_input.instr) for `Test_MCPL_input.instr`.

---
