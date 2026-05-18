# The `Test_PowderN` Instrument

*McXtrace: Unit-test instrument for the PowderN sample component.*

## Identification

- **Site:** Tests_samples
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** 2009

## Description

```text
Simply a model source illuminating a powder sample.
The default sample itself is an LaB6-powder.
Alternatively, the Single_crystal (powder mode) and FluoPowder components can also be tested.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Source energy (width 1 keV) | 15 |
| L1 | m | Source-sample distance | 10 |
| directbeam | 1 | Suppress direct beam (0) or not (1) | 0 |
| reflections | str | List of powder reflections, LAU/LAZ/CIF format. | "LaB6.cif" |
| SPLITS | 1 | Number of SPLIT's before sample | 1 |
| frac_c | 1 | Fraction of stats assigned to coherent scattering | 0.8 |
| frac_i | 1 | Fraction of stats assigned to incoherent scattering | 0.1 |
| frac_t | 1 | Fraction of stats assigned to unscattered, "direct beam" | 0.1 |
| d_phi | deg | Angle corresponding to the vertical angular range to focus to, e.g. detector height. 0 for no focusing. | 0 |
| index | 1 | Index of the sample component to use. 1=PowderN, 2=Single_crystal, 3=FluoPowder | 1 |

## Links

- [Source code](Test_PowderN.instr) for `Test_PowderN.instr`.

---
