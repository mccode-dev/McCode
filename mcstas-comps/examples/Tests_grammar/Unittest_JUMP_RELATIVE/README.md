# The `Unittest_JUMP_RELATIVE` Instrument

*McStas: Relative JUMP targets: JUMP PREVIOUS(n) and JUMP NEXT(n)*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 7th, 2026

## Description

```text
Unit test for relative JUMP targets, see
docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md

One unit of intensity is emitted from a 1x1 m square.
- Back jumps to PREVIOUS(2), i.e. to Target, 'iter' times in total
(JUMP ... ITERATE), so the Mon monitor in between is passed 'iter' times
and reads Mon_I = iter.
- Skip jumps to NEXT(2), i.e. over Skipped and onto Landed, so
Skipped_I = 0 and Landed_I = 1.

Before the fix, relative targets were used as absolute component indices:
JUMP PREVIOUS(n) crashed the code generator and JUMP NEXT(n) went to
component n. JUMP PREVIOUS is PREVIOUS(1) and JUMP NEXT is NEXT(1).
```

## Examples

- **Test: iter=3 Detector: Mon_I=3**
- **Test: iter=3 Detector: Landed_I=1**
- **Test: iter=3 Detector: Skipped_I=0**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| iter | 1 | Number of passes through Mon (JUMP ITERATE count) | 3 |
| Pp0 | 1 | Dummy input parameter used internally | 0 |

## Links

- [Source code](Unittest_JUMP_RELATIVE.instr) for `Unittest_JUMP_RELATIVE.instr`.
- docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md

---
