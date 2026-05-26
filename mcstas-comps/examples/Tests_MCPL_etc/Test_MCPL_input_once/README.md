# The `Test_MCPL_input_once` Instrument

*McStas: A test instrument for MCPL_input_once*

## Identification

- **Site:** Tests_MCPL_etc
- **Author:** Peter Willendrup <erkn@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** Mar 2024

## Description

```text
This is a unit test for the MCPL_input_once component, which differs from the original MCPL_input by:
1) Being MPI-parallelized (no implicit "repeat" of the particles in the file, file content is shared among processes
2) No implementation of "repetition", if further stats is needed downstream, SPLIT should be applied
```

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
