# The `Test_lib_read_table` Instrument

*McStas: Unit test of common/lib/share/read_table-lib (Table_Value, Table_Value2d,
Table_Rebin, Table_Read) on tables built in memory. Covers the bracket search
for constant-step and monotonic tables, both increasing and decreasing, the
1-row vector case of Table_Value2d, and that Table_Read of a missing file
leaves an empty table.

The Arm absorbs every particle if any check failed, so the PSD intensity is
Ncount on success and 0 on failure.*

## Identification

- **Site:** libtest
- **Author:** 
- **Origin:** 
- **Date:** 

## Description

```text
Tests: mccode-dev/McCode#1968, #2565, #3016, #3017
```

## Examples

- **Test: Ncount=1000 Detector: PSD_I=1000**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Ncount | 1 | Number of particles | 1000 |

## Links

- [Source code](Test_lib_read_table.instr) for `Test_lib_read_table.instr`.

---
