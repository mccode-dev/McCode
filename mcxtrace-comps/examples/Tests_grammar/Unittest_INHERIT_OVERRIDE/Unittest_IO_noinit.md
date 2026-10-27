# The `Unittest_IO_noinit` Component

*McXtrace: Grammar unit test helper for Unittest_INHERIT_OVERRIDE*

## Identification

- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 7th, 2026

## Description

```text
Inherits everything from Unittest_IO_parent but cancels its INITIALIZE with
an empty INITIALIZE EXTEND section: io_val stays 0, so TRACE halves the weight.
See docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md
```


## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|

## Links

- [Source code](Unittest_IO_noinit.comp) for `Unittest_IO_noinit.comp`.

---
