# The `MCPL_filter_energy` Instrument

*McStas: Filtering-by-energy instrument file for processing MCPL input files*

## Identification

- **Site:** Tools
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** 2019-03

## Description

```text
Example: Split an MCPL file at an energy of 0.5 meV
mcrun MCPL_filter_energy MCPLfile=my.mcpl.gz energy=0.5
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| MCPLfile | str | Defines the MCPL input file to process | "my.mcpl.gz" |
| energy | meV | Defines a threshold energy to split the MCPLfile at | 40 |
| max | meV | Maximum energy in Monitor_nD plots | 100 |
| min | meV | Minimum energy in Monitor_nD plots | 0 |
| bins | 1 | Number of bins in Monitor_nD plots | 100 |

## Links

- [Source code](MCPL_filter_energy.instr) for `MCPL_filter_energy.instr`.
- <reference/HTML link>
- MCPL website at <a href="https://mctools.github.io/mcpl/">https://mctools.github.io/mcpl/</a>

---
