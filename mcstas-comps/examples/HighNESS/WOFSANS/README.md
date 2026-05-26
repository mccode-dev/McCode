# The `WOFSANS` Instrument

*McStas: Instrument developed for the HighNESS EU project, describing a Wolter Optics
Focusing SANS (WOFSANS), applied for the study of moderator parameters.*

## Identification

- **Site:** HighNESS
- **Author:** Stavros Samothrakis, PSI
- **Origin:** PSI
- **Date:** 2021-2023

## Description

```text
Instrument developed for the HighNESS EU project, describing a Wolter Optics
Focusing SANS (WOFSANS_v2), applied for the study of moderator parameters.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| focusing_rectangle | m | Dimensions, both width and height of the source focusing area | 0.15 |
| width | m | Width-dimension of the source | 0.15 |
| Emin | meV | Minimum energy of neutron beam simulated from source | 0.2 |
| Emax | meV | Maximum energy of neutron beam simulated from source | 100000.0 |
| l_min | AA | Minimum wavelength allowed through emulated chopper system | 7.2 |
| pulseskip | 1 | Flag to skip one or more pulse frames in bandwidth calculation | 0 |
| npulses | 1 | Number of pulses to simulate | 1 |
| chopper1_flag | 1 | Switch emulated chopper on/off | 0 |
| det | m | Detector position for bandwidth calculation | 41.5 |
| R0 | 1 | R0 low-angle reflectivity of optical coating | 0.99 |
| m | 1 | m-value of optical coating | 3 |
| W | AA-1 | Supermirror cut-off, optical coating | 0.003 |
| alpha | AA | alpha parmeter of optical coating | 6.07 |
| nshells_ph | 1 | Number of conical shells simulated in PH-Wolter optic | 28 |
| fi_ph | m | Focal length for PH-Wolter optic | 5.5 |
| PH_beamstop | 1 | Flag to (de)activate Beamstop post PH-Wolter optic | 1 |
| nshells_eh | 1 | Number of conical shells simulated in EH-Wolter optic | 25 |
| fs_eh | m | Upstream focal length for EH-Wolter optic | 8 |
| fi_eh | m | Downstream focal length for EH-Wolter optic | 6 |
| EH_beamstop | 1 | Flag to (de)activate Beamstop post EH-Wolter optic | 1 |

## Links

- [Source code](WOFSANS.instr) for `WOFSANS.instr`.
- V. Santoro et. al. Nuc. Sci. Eng. 2023 https://doi.org/10.1080/00295639.2023.2204184
- https://highnessproject.eu
- This work was supported by the HighNESS project and is funded by European Commission (H2020-951782).

---
