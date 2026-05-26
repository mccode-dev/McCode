# The `MAXIV_DanMAX_pxrd1d` Instrument

*McXtrace: DanMAX Powder diffraction/Imaging beamline being designed at MAX IV.*

## Identification

- **Site:** MAXIV
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Aug 2015

## Description

```text
Design study model of the DanMAX PowderX/Imaging beamline.
This early version uses a Gaussian approximation source, and a simple bandpass filter
as the multilayer.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | The central energy to sample from source model. | 35 |
| DE | keV | Spectral width (std. dev. if gaussian source) to sample from source model. | 0.05 |
| undK |  | Undulator K parameter, overrides E0. | 0 |
| undDetune | eV | First harmonic detuning in eV. When zero - max flux on axis. If set to approx. 4 eV one can gain ~20% of flux through the aperture. | 4 |
| DCM_e0 | keV | The energy to tune the Si monochromator to. May be different from E0. If 0 the DCM is controlled by DCM_angle. | 35 |
| DMM_e0 | keV | The energy to tune the ML monochromator to. May be different from E0. If 0 the DMM is controlled by DMM_angle. | 35 |
| oh_premonoh | m | Pre-mono (white beam) slit height. | 1e-3 |
| oh_premonow | m | Pre-mono (white beam) slit width. | 1e-3 |
| oh_endh | m | OH exit slit height. | 1e-3 |
| oh_endw | m | OH exit slit width. | 1e-3 |
| PXRDsampleap_h | m | PXRD clean-up aperture height. | 300e-6 |
| PXRDsampleap_w | m | PXRD clean-up aperture width. | 300e-6 |
| DMM_angle | deg | Glancing angle of the ML. | 0 |
| DCM_angle | deg | Glacing angle of the Si-111 monochromator crystals. | 0 |
| DCM |  | If nonzero the high-resolution SI DCM is active. | 1 |
| DMM |  | If nonzero the multilayer mono is active. | 0 |
| OH_2DCRL_N |  | Number of 2D focus CRLs in the optics hutch transfocator. 0 means transfocator is inactive. | 11 |
| EH_2DCRL_N |  |  | 0 |
| D_EH_2DCRL |  |  | 0 |
| PXRD_SIMPLE |  | If zero - the full powder diffraction strip detector is active, else a single circular approximation is active. | 1 |
| sample_radius | m | Powder sample cylinder radius | 100e-6 |
| pxrd_strip_tth0 | deg | Angle offset of the PXRD 1d-strip detector from the optical axis. | 0 |
| debugMon |  | If nonzero, all intermediate monitors appear for debugging purposes. | 1 |
| beamStop |  | If nonzero, a beamstop is in between sample and PXRD 1d-strip detector. | 0 |
| SPLITS |  | Split-number at the sample-position | 1410 |

## Links

- [Source code](MAXIV_DanMAX_pxrd1d.instr) for `MAXIV_DanMAX_pxrd1d.instr`.

---
