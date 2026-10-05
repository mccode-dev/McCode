# The `Unittest_SEARCH` Instrument

*McStas: SEARCH unittest*

## Identification

- **Site:** Tests_grammar
- **Author:** Thomas Kittelmann
- **Origin:** ESS
- **Date:** 2026

## Description

```text
SEARCH unittest: the monitor component (Unittest_SEARCH_Monitor) is only
found via the SEARCH statement for the localcomps/ subdirectory.
```

## Examples

- **Test: frac=1 Detector: mon_I=1.99**
- **Test: frac=0.5 Detector: mon_I=0.995**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| frac | 1 | Fraction of the beam area covered by the monitor | 1 |

## Links

- [Source code](Unittest_SEARCH.instr) for `Unittest_SEARCH.instr`.

---
