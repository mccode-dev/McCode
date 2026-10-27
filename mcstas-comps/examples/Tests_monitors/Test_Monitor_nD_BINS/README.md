# The `Test_Monitor_nD_BINS` Instrument

*McStas: Regression test for the NeXus "BINS" pixel map of a list-mode Monitor_nD
with many non-spatial bins (issue #2484).*

## Identification

- **Site:** Tests_monitors
- **Author:** Mads Bertelsen and P. Willendrup
- **Origin:** ESS and DTU
- **Date:** October 2026

## Description

```text
With --format=NeXus, a list-mode Monitor_nD writes a "BINS" pixel map that
relates the event pixel ids to the detector geometry. The map spans the
spatial axes only (x, y, z, angle, divergence, theta, phi, or OFF faces).

The "events" monitor bins wavelength (100 bins), time (500000 bins) and
y (150 bins). Its BINS map holds the 150 y pixels. Before the fix it was
sized over all axes, 100 x 500000 x 150 = 7.5e9 pixels (about 180 GB), and
the simulation crashed during INITIALIZE without a message.

The "count" monitor is a plain y histogram used for the test value.

Run with --format=NeXus to exercise the BINS output; the default McCode
format writes no BINS map.
```

## Examples

- **Test: Test_Monitor_nD_BINS.instr Detector: count_I=0.0939253**
- **Test: Test_Monitor_nD_BINS.instr --format=NeXus Detector: count_I=0.0939253**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|

## Links

- [Source code](Test_Monitor_nD_BINS.instr) for `Test_Monitor_nD_BINS.instr`.

---
