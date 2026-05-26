# The `SSRL_bl_11_2_white_src` Instrument

*McXtrace: Simulating the beamline 11-2 of SSRL to reproduce their glitches database.*

## Identification

- **Site:** SSRL
- **Author:** Stephane Bac, Antoine Padovani
- **Origin:** Synchrotron Soleil
- **Date:** Dec 5th 2022

## Description

```text
BL 11-2 beamline description here: https://www-ssrl.slac.stanford.edu/mes/11-2/manual.php
https://www-ssrl.slac.stanford.edu/mes/Glitch_Curves/ , the instrument is the BL in the unfocussed and uncollimated mode
SSRL glitch database found here: https://www-ssrl.slac.stanford.edu/smbin/dataextractnew.pl
Documentation and results of the .instr here: https://gitlab.synchrotron-soleil.fr/grades/beamlines/-/tree/main/glitches
A jupyter notebook will soon be available regrouping everything.
You may scan like so for example:
mxrun -n 1e7 SSRL_bl_11_2_white_src.instr -N601 Etohit=6900,7500 detuning_percentage=40
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Etohit | eV. | Energy used to calculate the bragg angle the monochromator is put at. This is the energy we select. | 6900 |
| h | 1 | Miller indices. | 2 |
| k | 1 | Miller indices. | 2 |
| l | 1 | Miller indices. | 0 |
| fwhm | deg. | Full width half maximum of the second crystals's rocking curve. | 0.1 |
| detuning_percentage | %. | Percentage of the rocking curve fwhm to detune the second crystal by. | 0 |
| detuning_rockingcurve | deg. | Degrees to detune the second crystal by. Used to find the 2nd xtal's rocking curve. Leave at 0 otherwise. | 0 |

## Links

- [Source code](SSRL_bl_11_2_white_src.instr) for `SSRL_bl_11_2_white_src.instr`.

---
