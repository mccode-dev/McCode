# The `ESS_BEER_MCPL` Instrument

*McStas: Version: 1.1, 27/11/2018
ToF Diffractometer BEER@ESS with MCPL input at the sample slit.*

## Identification

- **Site:** ESS
- **Author:** Jan Saroun, saroun@ujf.cas.cz
- **Origin:** NPI Rez
- **Date:** June 2017

## Description

```text
Secondary part of the ToF diffractometer BEER@ESS. It takes MCPL file at the input as the source
of primary beam in front of the sample.
Results:
1) xy, divergence, lambda and ToF_lambda plots of the primary beam (monitors the MCPL content)
2) projections of the sampling volume defined by the primary slit and secondary radial collimator
3) Intensity vs. dhkl plot, using NPI_tof_dhkl_detector.comp
4) 2D plot (ToF,2theta),  using NPI_tof_theta_monitor

Modulation mode:
If modul=1, a modulation mode of BEER is assumed: the primary beam is modulated by a chopper placed close to the source.
(set the lc distance appropriately). The NPI_tof_dhkl_detector.comp then performs data reduction in event mode, using
the given modulation parameters: lam0, mod_frq, mod_twidth and a table with primary dhkl estimates (dhkl.dat should be
in the current directory). As a result, the diffractogram with recombined peaks is produced, together with a 2D map
of estimated valid, empty and overlap regions on the ToF-2theta diagram.

If module=0, modulation parameters are ignored and the NPI_tof_dhkl_detector.comp performs a standard event based
data reduction producing a diffractogram on the basis of given nominal values of lam0 and distances.

(See NPI_tof_dhkl_detector.comp for more details)

NOTE: some instrument parameters are hard coded and have to be edited in the INITIALIZE and RUN sections.

To use, please copy the relevant mcpl files from your $MCSTAS/data folder to the curring working folder

Example 1:
Simulation in medium resolution mode with pulse shaping choppers, wavelength range centred at 2 AA - use:
ESS_BEER_MCPL input=BEER_MR.mcpl repetition=50 pwdfile=duplex.laz lc=6.65 lam0=2 dlam=1.8 omega=45 chi=90 colw=1 modul=0 mod_frq=2240 mod_twidth=0.0029 mod_shift=0 only_event=-1 pinc=0.1 ptra=0 strain=0 ustrain=0
Detector: psdtof_I=276.842

Example 2:
Simulation in the modulation mode, using the modulation chopper MCB with 8 slits (4 deg wide) rotating at 280 Hz. Wavelength range centred at 2 AA - use:
ESS_BEER_MCPL input=BEER_MCB.mcpl repetition=50 pwdfile=duplex.laz lc=9.35 lam0=2 dlam=1.8 omega=45 chi=90 colw=1 modul=1 mod_frq=2240 mod_twidth=0.0029 mod_shift=0 only_event=-1 pinc=0.1 ptra=0 strain=0 ustrain=0
Detector: psdtof_I=98.3885
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| input | str | Input MCPL file | "BEER_MR.mcpl" |
| repetition | 1 | Number of loops through the MCPL file | 50 |
| pwdfile | str | Input sample file for PowderN.comp | "duplex.laz" |
| lc | m | distance of the pulse definition chopper | 6.65 |
| lam0 | AA | nominal wavelength (centre of the frame, determines the chopper phase) | 2.0 |
| dlam | AA | wavelength band width (only for filtering MCPL input and plot ranges) | 1.8 |
| omega | deg | sample orientation (y-axis) | 45 |
| chi | deg | sample orientation (z-axis) | 90 |
| colw | mm | collimator width (0.5, 1, 2, 3 or 4) | 1 |
| modul | 0\|1 | modulation mode switch | 0 |
| mod_frq | Hz | modulation frequency (chopper frequency x number of slits) | 2240 |
| mod_twidth | s | modulation frame width (should be ~> ESS pulse width) | 0.0029 |
| mod_shift |  | assumed line shift introduced to NPI_tof_dhkl_detector (modulation mode only) | 0 |
| only_event | 1 | if > -1, filters out events with line_info.itype<>only_event after PowderN sample | -1 |
| pinc |  | pinc value passed to PowderN.comp | 0.1 |
| ptra |  | p_transmit value passed to PowderN.comp | 0.0 |
| strain | ppm | Macro-strain (peak shift) | 0 |
| ustrain | ppm | Micro-strain (peak broadening) | 0 |

## Links

- [Source code](ESS_BEER_MCPL.instr) for `ESS_BEER_MCPL.instr`.

---
