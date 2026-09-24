# The `Test_TOFRes_sample` Instrument

*McStas: Testing resolution of a TOF spectrometer, use "reso.dat" output with mcresplot.py*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup (adapted from ancient Kim Lefmann instrument)
- **Origin:** DTU
- **Date:** 25-Oct-2023

## Description

```text
Testing resolution of a TOF spectrometer, use "reso.dat" output with mcresplot.py

TOF resolution test instrument.
```

## Examples

- **Test: TWOTHETA=60 Detector: TOFL3_I=1.49356e+08**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Chop_W1 | m | Width of 1st chopper slit | 0.1 |
| Chop_ph1 | s | Temporal phase of 1st chopper | 0.0009 |
| Chop_W2 | m | Width of 2nd chopper slit | 0.2 |
| Chop_ph2 | s | Temporal phase of 2nd chopper | 0.003 |
| Chop_W3 | m | Width of 3rd chopper slit | 0.1 |
| Chop_ph3 | s | Temporal phase of 3rd chopper | 0.006 |
| TIME_BIN | us | Target detection time | 10000 |
| BIN_WIDTH | us | Width of detection times | 10 |
| TWOTHETA | deg | Scattering angle | 60 |

## Links

- [Source code](Test_TOFRes_sample.instr) for `Test_TOFRes_sample.instr`.

## Resolution events

The `TOFRes_monitor` instance writes `TOFres.dat` through the generic
event-list output path. Its fixed column order is:

```text
ki_x ki_y ki_z kf_x kf_y kf_z x y z p_i p_f
```

The monitor's `bufsize` parameter controls the fixed event-buffer capacity;
`bufsize=0` stores up to the instrument ray count. Overflow is reported as a
warning and the accepted rows are still saved.

---
