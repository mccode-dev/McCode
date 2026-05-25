# The `Unit_test_logger_2D_kf_time` Instrument

*McStas: Tests Logger_2D_kf_time that records final wavevector in several time steps*

## Identification

- **Site:** Tests_union
- **Author:** Mads Bertelsen
- **Origin:** ESS
- **Date:** June 2020

## Description

```text
Tests the 2D kf time logger that histograms the final wavevector after each
scattering event in a 2D plane, and these are further distributed into a number
of time bins so the time evolution of final wavevectors can be monitored.

Includes conditional component to test record_to_temp functionality
of investigated logger.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|

## Links

- [Source code](Unit_test_logger_2D_kf_time.instr) for `Unit_test_logger_2D_kf_time.instr`.

---
