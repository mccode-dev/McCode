# The `Test_monitors` Instrument

*McXtrace: Unit test instrument for various monitors.*

## Identification

- **Site:** Tests
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jan 21

## Description

```text
This is a unit test instrument to test some of the McXtrace monitors.
```

## Examples

- **Test: Test_monitors.instr monitor_no=1 Detector: epsd0_I=1.81362**
- **Test: Test_monitors.instr monitor_no=2 Detector: epsd1_I=1.81367**
- **Test: Test_monitors.instr monitor_no=3 Detector: div_I=1.83645**
- **Test: Test_monitors.instr monitor_no=4 Detector: mnd_psd_I=1.83632**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| monitor_no |  | Pick a monitor to test - causes the others not to write any data files. | 1 |

## Links

- [Source code](Test_monitors.instr) for `Test_monitors.instr`.

---
