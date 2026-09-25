# The `Test_Monitor_nD_list` Instrument

*McStas: Exercise Monitor_nD's final list save across the list options matrix*

## Identification

- **Site:** Tests_monitors
- **Author:** McCode event-output work (Step 6)
- **Origin:** DTU
- **Date:** September 2026

## Description

```text
Exercise Monitor_nD's final list save (migrated to the generic
mcevent_out_list_nd event API) across the list options matrix:
  - a two-column custom list (x y list)
  - a list containing a pixel ID column (x y pixel)
  - a fixed-capacity list with overflow (x y list=5)
  - list all (x y list all)
  - auto limits combined with list output (auto x y list)
```

Each `Monitor_nD` instance is given a distinct `filename`, so the list
output is written as `<name>_list.p.<col>...` (no extension) alongside the
normal `<name>.x_y` histogram. All list files carry the header
`xlabel: List of neutron events` and the per-instance column names.

Expected list outputs (with `NCount=1000`):

| Instance | options | type | data rows |
|----------|---------|------|-----------|
| `ND_xy`   | `x y list`                    | `list(3, 1000)` | 1000 |
| `ND_pix`  | `x bins=10 y bins=10 pixel list` | `list(4, 1000)` | 1000 (extra `id` pixel column) |
| `ND_ovf`  | `x y list=5`                  | `list(3, 5)` | 5 (buffer overflow) |
| `ND_all`  | `x y list all`                | `list(3, 1000)` | 1000 |
| `ND_auto` | `auto x bins=10 y bins=10 list` | `list(3, 1000)` | 1000 |

The histogram (`<name>.x_y`) always accumulates all 1000 events regardless of
the list buffer capacity (verified for `ND_ovf`), i.e. the list capacity does
not affect the histogram.

## Examples

- **Test: NCount=1000 Detector: FlexRef_I=1000**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| **NCount** | 1 | Statistic to run with | 1000 |

## Links

- [Source code](Test_Monitor_nD_list.instr) for `Test_Monitor_nD_list.instr`.

---
