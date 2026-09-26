# The `Test_Cyl_monitors` Instrument

*McStas: Test Monitor_nD against basic monitor*

## Identification

- **Site:** Tests_monitors
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** 30. September 2020

## Description

```text
A test instrument to compare Monitor_nD output against basic 1D and 2D monitors.
```

## Examples

- **Test: Test_Cyl_monitors.instr lambda=5 Detector: PSD_I=4825.57**
- **Test: Test_Cyl_monitors.instr lambda=5 Detector: CylMon_I=4825.57**
- **Test: Test_Cyl_monitors.instr lambda=5 Detector: CylMonPSD_I=4825.57**
- **Test: Test_Cyl_monitors.instr lambda=5 Detector: CylMonTOF_I=4825.57**
- **Test: Test_Cyl_monitors.instr lambda=5 Detector: NDcyl_I=4825.57**
- **Test: Test_Cyl_monitors.instr lambda=5 Detector: NDcylPSD_I=4825.57**
- **Test: Test_Cyl_monitors.instr lambda=5 Detector: NDcylTOF_I=4825.57**
- **Test: Test_Cyl_monitors.instr lambda=5 tz=0.05 Detector: PSD_I=2921.65**
- **Test: Test_Cyl_monitors.instr lambda=5 tz=0.05 Detector: CylMon_I=2921.65**
- **Test: Test_Cyl_monitors.instr lambda=5 tz=0.05 Detector: CylMonPSD_I=2921.65**
- **Test: Test_Cyl_monitors.instr lambda=5 tz=0.05 Detector: CylMonTOF_I=2921.65**
- **Test: Test_Cyl_monitors.instr lambda=5 tz=0.05 Detector: NDcyl_I=2921.65**
- **Test: Test_Cyl_monitors.instr lambda=5 tz=0.05 Detector: NDcylPSD_I=2921.65**
- **Test: Test_Cyl_monitors.instr lambda=5 tz=0.05 Detector: NDcylTOF_I=2921.65**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator, computed from DM and THETA_M if left as 0. | 5 |
| L1 | m | Source-sample distance | 10 |
| bins |  | Number of bins on monitors | 100 |
| omega | deg | Focusing angle wrt. sample | 90 |
| thmin | deg | Minimum angle mesured | -180 |
| thmax | deg | Maximum angle mesured | 180 |
| focus_aw | deg | Angular width of focusing from sample | 10 |
| focus_ah | deg | Angular height of focusing from sample | 2 |
| focus_xw | m | Width of focusing from sample | 0 |
| focus_yh | m | Height of focusing from sample | 0 |
| tx |  |  | 0 |
| tz |  |  | 0 |

## Links

- [Source code](Test_Cyl_monitors.instr) for `Test_Cyl_monitors.instr`.

---
