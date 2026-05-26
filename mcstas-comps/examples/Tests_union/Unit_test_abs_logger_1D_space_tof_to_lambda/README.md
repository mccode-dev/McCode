# The `Unit_test_abs_logger_1D_space_tof_to_lambda` Instrument

*McStas: Test of abs_logger_1D_space_tof_to_lambda that calculates wavelength from tof*

## Identification

- **Site:** Tests_union
- **Author:** Mads Bertelsen
- **Origin:** ESS
- **Date:** June 2020

## Description

```text
This component works as other absorption loggers, but converts the tof and
position data to wavelength, which is then compared to the actual wavelength
calculated from the neutron state. The neutron position used to calculate
the wavelength is pixelated while the time of flight is continous. The
distance from source to sample is an input parameter, and the distance from
sample to a detector pixel is calculated using the reference component position
which should be specified with a relative component index. This test checks
both the standard view with measured and true wavelength on each axis, and the
relative where they have been normalized and a value of 1 is expected.

Includes conditional component to test record_to_temp functionality
of investigated abs_logger.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|

## Links

- [Source code](Unit_test_abs_logger_1D_space_tof_to_lambda.instr) for `Unit_test_abs_logger_1D_space_tof_to_lambda.instr`.

---
