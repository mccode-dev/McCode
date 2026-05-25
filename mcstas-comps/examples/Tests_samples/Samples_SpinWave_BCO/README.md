# The `Samples_SpinWave_BCO` Instrument

*McStas: Simple test instrument for the SpinWave_BCO component*

## Identification

- **Site:** Tests_samples
- **Author:** Silas Schack
- **Origin:** NBI
- **Date:** Spring 2026

## Description

```text
Simple test instrument for the SpinWave_BCO component.
Refer to the component documentation for further instructions.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E | meV | Mean energy at source | 1.06 |
| Ef | meV | Final energy | 14.7 |
| Dlambda | AA | Wavelength spread at source | 0.1 |
| h | 1 | 1st sample Miller index | 1 |
| l | 1 | 3rd sample Miller index | 0 |
| B | T | Field strength of external magnetic field applied along z-axis | 0 |
| dA3 | deg | Offset from A3 nominal value | -90 |
| Temp | K | Sample temperature | 2 |
| width | deg | Width of sample | 0.005 |
| E_steps_low | 1 | Number of low-energy steps in zrid | 50 |
| E_steps_high | 1 | Number of high-energy steps in zrid | 50 |
| mode | 1 | Index of mode(s) to simulate | 2 |
| verbose | 1 | Verbosity flag | 0 |

## Links

- [Source code](Samples_SpinWave_BCO.instr) for `Samples_SpinWave_BCO.instr`.

---
