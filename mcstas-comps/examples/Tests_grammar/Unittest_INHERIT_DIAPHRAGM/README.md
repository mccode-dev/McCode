# The `Unittest_INHERIT_DIAPHRAGM` Instrument

*McStas: Whole-component INHERIT: Diaphragm (DEFINE COMPONENT Diaphragm INHERIT Slit)*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 7th, 2026

## Description

```text
Unit test for whole-component inheritance, see
docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md

Diaphragm.comp is just "DEFINE COMPONENT Diaphragm INHERIT Slit" + END,
so all of its code sections must come from Slit.

One unit of intensity is emitted uniformly from a 1x1 m square:
- a Slit of 0.5x0.5 m passes 1/4:    AfterSlit_I = 0.25
- a Diaphragm of 0.25x0.25 m passes 1/16 of the source: AfterDiaphragm_I = 0.0625

Before the fix, a component inheriting all of its code got none of it, so
the Diaphragm let everything through (AfterDiaphragm_I = 0.25).
```

## Examples

- **Test: Detector: AfterSlit_I=0.25**
- **Test: Detector: AfterDiaphragm_I=0.0625**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Pp0 | 1 | Dummy input parameter used internally | 0 |

## Links

- [Source code](Unittest_INHERIT_DIAPHRAGM.instr) for `Unittest_INHERIT_DIAPHRAGM.instr`.
- docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md

---
