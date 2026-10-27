# The `Unittest_UV_check` Component

*McStas: Grammar unit test helper for Unittest_USERVARS_TYPES*

## Identification

- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 7th, 2026

## Description

```text
Exercises code-generator fixes from
docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md:
- %include "unittest_uv.hpp" (multi-character extension) in SHARE,
- USERVARS followed by EXTEND: uv_b is declared in the EXTEND block and
was silently dropped before the fix,
- two DECLARE variables on one line: only the first was found before.

In TRACE the component writes this instance's uv_b USERVAR (by name,
"uv_b_<index>", built in INITIALIZE as sprintf is a no-op on GPU) and
reads it back. If that fails, the particle is
absorbed, so a monitor after it reads 0 instead of the full intensity.
```


## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|

## Links

- [Source code](Unittest_UV_check.comp) for `Unittest_UV_check.comp`.

---
