# The `Czerny_Turner` Instrument

*McXtrace: Czerny-Turner monochromator.*

## Identification

- **Site:** BARC
- **Author:** Stephane Bac, Antoine Padovani
- **Origin:** Synchrotron Soleil
- **Date:** Jul 21st 2022

## Description

```text
This Czerny-Turner monochromator was built from "Design and fabrication of a Czerny-Turner monochromator-cum-spectrograph" by
Murty, M.V.R.K.; Shukla, R.P.; Bhattacharya, S.S.; Krishnamurthy, G. (Bhabha Atomic Research Centre, Bombay (India). Spectroscopy Div.)
Bhabha Atomic Research Centre, Bombay (India)
1987
It can be found here: https://inis.iaea.org/collection/NCLCollectionStore/_Public/19/019/19019134.pdf
Example: Do a scan (scan parameter =1) from x_screw=20 to 103 mm. The calculated monitor "Wavelength(Ang) as a function of x_screw(mm)" will be a linear function. The monochromator functions properly.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| x_screw | mm | Displacement perpendicular to initial position of the lever (sine drive mechanism) | 0 |
| scan | 0 or 1 | 1 is to activate wavelength monitor while scanning the x_screw parameter | 0 |

## Links

- [Source code](Czerny_Turner.instr) for `Czerny_Turner.instr`.

---
