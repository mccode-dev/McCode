# The `TAS1_C1_Tilt` Instrument

*McStas: Example formerly known as linup-2.instr

The conventional cold-source triple-axis spectrometer TAS1 at Risoe National
Laboratory used as a diffractometer for a collimator tilt alignment.*

## Identification

- **Site:** Risoe
- **Author:** A. Abrahamsen, N. B. Christensen, and E. Lauridsen
- **Origin:** Risoe
- **Date:** 1998

## Description

```text
This instrument is the conventional cold-source triple-axis spectrometer TAS1
at Risoe National Laboratory. It does not exist anymore, but was used as the
first detailed work performed with the McStas package.
Here it is used as a diffractometer for a collimator tilt alignment. The
detector is at the sample position and there is no analyzer.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| PHM | deg | Monochromator arm angle, aka A1 | -37.077 |
| TTM | deg | Monochromator take-off angle, aka A2 | -74 |
| C1 | min | Collimator 1 aperture (mono-sample arm) | 30 |
| OMC1 | deg | Tilt angle of the Collimator 1 | 5.5 |

## Links

- [Source code](TAS1_C1_Tilt.instr) for `TAS1_C1_Tilt.instr`.
- The McStas User manual
- A. Abrahamsen, N. B. Christensen, and E. Lauridsen. McStas simulations of the TAS1 spectrometer. Student's report, Niels Bohr Institute, University of Copenhagen, 1998.

---
