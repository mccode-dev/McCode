# The `TAS1_Vana` Instrument

*McStas: Example formerly known as linup-6.instr

The conventional cold-source triple-axis spectrometer TAS1 at Risoe National
Laboratory used with a vanadium sample.*

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
The sample is a vanadium and the analyzer is a single plate.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| PHM | deg | Monochromator rotation angle, aka A1 | -37.077 |
| TTM | deg | Monochromator take-off angle, aka A2 | -74 |
| TT | deg | Take-off angle at the sample position, aka A4 | 33.52 |
| TTA | deg | Take-off angle at the analyzer position, aka A6 | 0 |
| C1 | min | Collimator 1 aperture (mono-sample arm) | 30 |
| OMC1 | deg | Tilt angle of the Collimator 1 | 5.5 |
| C2 | min | Collimator 2 aperture (sample-ana arm) | 28 |
| C3 | min | Collimator 3 aperture (ana-detector arm) | 67 |
| OMA | deg | Analyzer rotation angle, aka A5 | -17.45 |

## Links

- [Source code](TAS1_Vana.instr) for `TAS1_Vana.instr`.
- The McStas User manual
- A. Abrahamsen, N. B. Christensen, and E. Lauridsen. McStas simulations of the TAS1 spectrometer. Student's report, Niels Bohr Institute, University of Copenhagen, 1998.

---
