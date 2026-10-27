# The `Unittest_USERVARS_TYPES` Instrument

*McXtrace: USERVARS of non-double type, several declarations per line, USERVARS+EXTEND*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 7th, 2026

## Description

```text
Unit test for USERVARS/DECLARE handling in the code generator, see
docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md

One unit of intensity is emitted from a 1x1 m square.
- USERVARS declares "int flag; double wsum;" on ONE line. Before the fix
only the first declaration on a line was found, so wsum was missing.
- flag (an int) is set to 1 for x>0 and read by Monitor_nD (user1="flag")
through particle_getvar(). Before the fix its memory was read as a
double (garbage); now it is converted, and the [0.5 1.5] range holds
half of the beam: Flag_I = 0.5. (2 bins: a 1-bin Monitor_nD is 0-D and
ignores the limits.)
Note: USERVARS used with Monitor_nD should still be declared double;
int is used here only to test the conversion.
- Unittest_UV_check (local component) tests %include "x.hpp", component
USERVARS followed by EXTEND, and two DECLARE variables on one line. It
absorbs the particle if its EXTEND-declared USERVAR is missing, so
AfterCheck_I = 1.
```

## Examples

- **Test: Detector: AfterCheck_I=1**
- **Test: Detector: Flag_I=0.5**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Pp0 | 1 | Dummy input parameter used internally | 0 |

## Links

- [Source code](Unittest_USERVARS_TYPES.instr) for `Unittest_USERVARS_TYPES.instr`.
- docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md

---
