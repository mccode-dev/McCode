# The `MCPL_filter_wavelength` Instrument

*McStas: Filtering-by-wavelength instrument file for processing MCPL input files*

## Identification

- **Site:** Tools
- **Author:** Peter Willendrup <pkwi@fysik.dtu.dk>
- **Origin:** DTU
- **Date:** 2019-03

## Description

```text
Example: Split an MCPL file at a wavelength of 0.5 AA
mcrun MCPL_filter_wavelength MCPLfile=my.mcpl.gz wavelength=0.5
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| MCPLfile | str | Defines the MCPL input file to process | "my.mcpl.gz" |
| wavelength | AA | Defines a threshold wavelength to split the MCPLfile at | 0.5 |
| max | AA | Maximum wavelength in Monitor_nD plots | 20 |
| min | AA | Minimum wavelength in Monitor_nD plots | 0 |
| bins | 1 | Number of bins in Monitor_nD plots | 100 |

## Links

- [Source code](MCPL_filter_wavelength.instr) for `MCPL_filter_wavelength.instr`.
- <reference/HTML link>
- MCPL website at <a href="https://mctools.github.io/mcpl/">https://mctools.github.io/mcpl/</a>

---
