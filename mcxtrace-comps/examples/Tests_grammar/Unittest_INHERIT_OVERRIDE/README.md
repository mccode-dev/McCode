# The `Unittest_INHERIT_OVERRIDE` Instrument

*McXtrace: DEFINE COMPONENT X INHERIT Y: written sections (even empty) replace Y's*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 7th, 2026

## Description

```text
Unit test for whole-component inheritance, see
docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md
(same rule as mccode-antlr, mccode-dev/mccode-antlr#321).

Local components: Unittest_IO_parent absorbs particles with x>0. Its
children, DEFINE COMPONENT ... INHERIT Unittest_IO_parent:
- Unittest_IO_cancel writes an empty TRACE %{ %}: replaces the parent's
TRACE, absorbs nothing. Before, an empty block counted as "not written"
and the parent's TRACE was used.
- Unittest_IO_keep writes no sections: inherits the parent's TRACE.
- Unittest_IO_extend writes TRACE INHERIT Unittest_IO_parent EXTEND %{..%}:
the parent's TRACE followed by more code (absorbs x>0 and y>0). It also
uses INHERIT within USERVARS.
- Unittest_IO_noinit writes an empty INITIALIZE EXTEND %{ %}: keeps the
parent's DECLARE and TRACE but not its INITIALIZE (io_val stays 0, so
its TRACE halves the weight). "Inherit everything but one section".

One unit of intensity is emitted uniformly from a 1x1 m square:
AfterCancel_I = 1, AfterKeep_I = 0.5, AfterExtend_I = 0.25,
AfterNoInit_I = 0.125.
```

## Examples

- **Test: Detector: AfterCancel_I=1**
- **Test: Detector: AfterKeep_I=0.5**
- **Test: Detector: AfterExtend_I=0.25**
- **Test: Detector: AfterNoInit_I=0.125**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Pp0 | 1 | Dummy input parameter used internally | 0 |

## Links

- [Source code](Unittest_INHERIT_OVERRIDE.instr) for `Unittest_INHERIT_OVERRIDE.instr`.
- docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md

---
