# The `Test_GROUP_restore` Instrument

*McStas: Test instrument for GROUP of monitors using restore_neutron*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU/ESS
- **Date:** June 2024

## Description

```text
Test instrument for GROUP of monitors using restore_neutron.

1st test: Source emits intensity of 6e6 - "cube" positioned monitors each catch 1/6 of that.
2nd test: Reduced dimension of monitors mean a fraction is lost in the GROUP
3rd test: Adding a "catchall" arm recovers the intensity
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| dim | m | Width/height of monitors in "cubic" GROUP arrangement | 4 |
| catchall | 1 | Flag to indicate if "catchall"-Arm is included in GROUP | 0 |
| restore |  |  | 1 |
| src_dim | m | radius of 4PI-emitting source | 0.01 |

## Links

- [Source code](Test_GROUP_restore.instr) for `Test_GROUP_restore.instr`.
- A reference/HTML link for more information

---
