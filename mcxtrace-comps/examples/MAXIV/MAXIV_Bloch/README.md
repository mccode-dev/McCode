# The `MAXIV_Bloch` Instrument

*McXtrace: Bloch high resolution photoelectron spectroscopy beamline under development at the MAX IV synchrotron.*

## Identification

- **Site:** MAXIV
- **Author:** Kristian Soerensen and Philip Smith (s154443@win.dtu.dk)
- **Origin:** DTU Physics
- **Date:** June 2018

## Description

```text
This is a simple simulation of the BLOCH beamline at MAXIV.

Two kinds of parameters are given, P and P2.
P Parameters are the ones used in the general simulation, i.e when the FOI is intensity ect.
P2 parameters are used when the strain of the beamline is the FOI, i.e what happens when the distances change.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Wanted_energy | keV | The grating reflection energy. Uses E0 when not set. | 0.378 |
| m |  | Diffraction order, used for Monochromator angle. (fungerer ikke). | 3 |
| cff |  | Constant cff value for the grating. | 0 |
| r_rho | l/mm | Ruling density of the grating. | 800 |
| SourceChoice | 0/1 | Choice of souce. For easy simulation do Flat souce [0],in nonzero, the undulator is used. | 0 |
| E0 | keV | The central energy to sample from source model. | 0.6 |
| dE | keV | Spectral width to sample from source model. | 0.4 |
| undK |  | Undulator K parameter, overrides E0, only used if SourceChoice is non zero. | 5.6 |
| Nper |  | Number of magnetic periods in the undulator. | 187 |
| grating_mode | 0/1 | If 1 the NIM mode will be used. If 0 cPGM. Otherwise, optimum will be calculated. | -1 |
| zm_mirror2 | m | distance(z) to mirror2 from previous component. | 2 |
| R0_M2 |  | Constant relectivity of Mirror2 [0;1]. | 1 |
| zm_mirror1 | m | distance(z) to first mirror from previous component. | 14 |
| theta_mirror1 | m | glancing angle of first mirror. | 3 |
| R0_M1 |  | Constant relectivity of Mirror2 [0;1]. | 1 |
| zm_mirror3 | m | distance(z) to mirror3 from previous component. | 1 |
| theta_mirror3 | deg | glancing angle of mirror3. | 3 |
| R0_M3 |  | Constant relectivity of Mirror3 [0;1]. | 1 |
| zm_mirror4 | m | distance(z) to mirror4 from previous component. | 19 |
| theta_mirror4 | deg | glancing angle of mirror4. | 3 |
| R0_M4 |  | Constant relectivity of Mirror4 [0;1] | 1 |
| zm_ExitSlit | m | distance from Mirror4 to exit slit. | 9 |
| xwidth_ExSlit | m | xwidth of exit slit. | 1e-2 |
| yheight_ExSlit | m | yheight of exit slit. | 1e-2 |
| Exitslit_yshift |  | y-shift of the exit slit. | 0.005 |
| verbose |  | Flag to print more information | 0 |
| perfectMirrors |  | When 0, a toroidal mirros is used, otherwise a plane mirror is used. | 0 |
| Error |  | When 1, alignment errors are applied randomly on the optics (from  R. Sankari) | 0 |
| angle_grating | deg | Additional tilt on the grating angle. | 6 |
| mirror2_angle |  | M2 angle used when grating_mode is 0, otherwise it is computed. | 6 |

## Links

- [Source code](MAXIV_Bloch.instr) for `MAXIV_Bloch.instr`.
- Bloch_MAX IV description: https://www.maxiv.lu.se/accelerators-beamlines/beamlines/bloch/
- McXtrace komponent definition: http://www.mcxtrace.org

---
