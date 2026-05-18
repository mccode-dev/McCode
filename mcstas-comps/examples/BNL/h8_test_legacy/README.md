# The `h8_test_legacy` Instrument

*McStas: The former thermal H8 triple-axis spectrometer from Brookhaven reactor, 1.12c-comparable*

## Identification

- **Site:** BNL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 1st Feb 2001.

## Description

```text
This instrument is a simple model of the thermal H8 triple-axis spectrometer
from former Brookhaven reactor. It is directly illuminated by the moderator,
and has flat monochromator and analyzer. Sample is a vanadium cylinder.
Such an instrument was used for the Monte Carlo neutron simulation package
cross comparison related in paper "A Model Instrument for Monte Carlo Code
Comparisons", Neutron News 13 (No. 4), 24-29 (2002).

This version of the instrument does not use the SPLIT keyword and is comparable
to the version provided with McStas 1.12c. Has been added for benchmarking
purposes only.

Example: mcrun h8_test_legacy.instr Lambda=2.36
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lambda | Angs | source energy | 2.36 |

## Links

- [Source code](h8_test_legacy.instr) for `h8_test_legacy.instr`.
- Neutron News 13 (No. 4), 24-29 (2002).

---
