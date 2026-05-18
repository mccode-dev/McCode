# The `Test_Detector_pn` Instrument

*McXtrace: Unit test for Detector_pn*

## Identification

- **Site:** Tests
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk>
- **Origin:** DTU Physics
- **Date:** Apr 20

## Description

```text
A simple unit test instrument for the Detector_pn component. It consists merely of
two sets of monitor each with:
1 regular PSD_monitor and 2 instances of Detector_pn with a scinitillator screen of
Si and Pb respectively. Obviously, no sane person would use Pb, but it's there for
illustration purposes.
A restore_flag parameter may be passed to Detector_pn which negates the absorption
effect for subsequent components.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| restore_flag |  | Should the monitor/detector instances restore the x-ray to its pre-detector state | 0 |
| E0 | keV | The central simulation energy to use. A 1% BW will be added. | 12.4 |
| DE | keV | Half energy width of the simulation energy. | 0.124 |

## Links

- [Source code](Test_Detector_pn.instr) for `Test_Detector_pn.instr`.

---
