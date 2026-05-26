# The `Test_Single_crystal_inelastic` Instrument

*McStas: Test instrument for the Single_crystal_inelastic component.*

## Identification

- **Site:** Tests_samples
- **Author:** <a href="mailto:duc.le@stfc.ac.uk">Duc Le</a>
- **Origin:** ISIS
- **Date:** Feb 2015

## Description

```text
Simple test instrument for Single_crystal_inelastic in Horace mode

Example: mcrun Test_Single_crystal_inelastic.instr

once your instrument is written and functional:
- replace INSTRUMENT_SITE entry above with your Institution name as a single word
- rename the instrument name after DEFINE INSTRUMENT below
- update the parameter help in the following Parameters section
- update the instrument description, and better add a usage example with a
sensible parameter set.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E | meV | Mean energy produced at the source | 10 |
| DE | meV | Energy spread produced at the source | 0.1 |
| HDIV | deg | Horizontal divergence from the source | 0.5 |
| VDIV | deg | Vertical divergence from the source | 0.5 |
| OM | deg | Sample orientation around y | 0 |
| TH | deg | Sample orientation around x | 0 |
| FI | deg | Sample orientation around z | 0 |
| SQW | str | 4D Sqw input file | "example.sqw4" |

## Links

- [Source code](Test_Single_crystal_inelastic.instr) for `Test_Single_crystal_inelastic.instr`.
- Duc Le (2015) - duc.le@stfc.ac.uk

---
