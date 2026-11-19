# The `Test_Phonon_BvK_PG` Instrument

*McStas: Simple thermal triple axis spectrometer for test of Phonon_BvK_PG component
Modified from older version by Dorothy Wang, Kim Lefmann*

## Identification

- **Site:** Generic
- **Author:** Kim Lefmann
- **Origin:** NBI, KU
- **Date:** 20.07.2025

## Description

```text
Simple TAS to test the Phonon_BvK_PG component
```

## Examples

- **Test: h=0   l=2.2 E=4.9   Temp=2   phononmode=2  Detector: e_mon_beforeana_zoom_I=2.28e-10**
- **Test: h=0   l=2.2 E=1.795 Temp=2   phononmode=1  Detector: e_monitor_beforeana_I=2.0e-13**
- **Test: h=0   l=2.2 E=4.9   Temp=2   phononmode=12 Detector: e_mon_beforeana_zoom_I=2.43e-10**
- **Test: h=0   l=2.2 E=4.9   Temp=300 phononmode=2  Detector: e_mon_beforeana_zoom_I=1.301e-9**
- **Test: h=0   l=2.8 E=4.9   Temp=2   phononmode=2  Detector: e_mon_beforeana_zoom_I=1.037e-19**
- **Test: h=2.4 l=0   E=44.95 Temp=2   phononmode=0  Detector: e_mon_beforeana_zoom_I=9.33e-19**
- **Test: h=0.4 l=6   E=44.95 Temp=2   phononmode=0  Detector: e_mon_beforeana_zoom_I=8.81e-11**
- **Test: h=0   l=2.1 E=15.8  Temp=2   phononmode=5  Detector: e_mon_beforeana_zoom_I=4.02e-18**
- **Test: h=0   l=2.9 E=15.8  Temp=2   phononmode=5  Detector: e_mon_beforeana_zoom_I=1.86e-10**
- **Test: h=2   l=0   E=5.78  Temp=2   phononmode=12 Detector: e_mon_beforeana_zoom_I=7.8e-10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E | meV | Energy transfer | 4.5 |
| Ef | meV | Final energy | 25 |
| Dlambda | AA | width of wavelength band | 0.05 |
| h | rlu | q-value in hexagonal plane | 0 |
| l | rlu | q-value out of hexagonal plane | 3.5 |
| dA3 | deg | A3 offset | 90 |
| Temp | K | Sample temperature | 2 |
| width | m | Beam and sample size scale | 0.001 |
| coll | arcmin | Collimation before the sample | 20 |
| phononmode | 1 | Phonon mode 0..11, or 12 for random choice | 0 |
| E_steps_high | 1 | Root-search intervals above the elastic line | 100 |
| E_steps_low | 1 | Root-search intervals below the elastic line | 100 |
| Verbose | 1 | Verbosity level of the sample component | 0 |
| DISP | 1 | Dispersion output mode of the sample component | 0 |

## Links

- [Source code](Test_Phonon_BvK_PG.instr) for `Test_Phonon_BvK_PG.instr`.

---
