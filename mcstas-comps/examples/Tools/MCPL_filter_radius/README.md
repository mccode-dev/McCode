# The `MCPL_filter_radius` Instrument

*McStas: Filtering-by-radius instrument file for processing MCPL input files*

## Identification

- **Site:** Tools
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** 2019-03

## Description

```text
Example: Split an MCPL file at a radius of 2.5 m from the origin
mcrun MCPL_filter_radius MCPLfile=my.mcpl.gz radius=2.5
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| MCPLfile | str | Defines the MCPL input file to process | "my.mcpl.gz" |
| radius | m | Defines a threshold distance to split the MCPLfile at | 2.5 |

## Links

- [Source code](MCPL_filter_radius.instr) for `MCPL_filter_radius.instr`.
- <reference/HTML link>
- MCPL website at <a href="https://mctools.github.io/mcpl/">https://mctools.github.io/mcpl/</a>

---
