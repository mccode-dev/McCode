# The `ILL_H5_new` Instrument

*McStas: The full H5 new cold guide at the ILL, with ThALES, D16, Super-Adam, IN15
(sample only), D22WASP and CryoEDM
This is a proposed geometry for major H5 guide hall upgrade (since 2013).
The exact adopted H5 geometry may be actually different.*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** May, 2011

## Description

```text
This model decribes the full new H5 cold guide at the ILL, with ThALES, WASP, D16,
Super-Adam, IN15, D22.

H511:The IN15 Spin-echo spectrometer
is simulated with an incoming polarized beam, but not with a full spin-echo
description. Sample is Vanadium
H512:The D22 Large dynamic range small-angle diffractometer
is fully simulated
H521:The D16 Small momentum transfer diffractometer D16 is realistic
H521:The SuperADAM reflectometer is used in low angle diffraction mode.
H522: The WASP Spin-echo spectrometer
is simulated with an incoming polarized beam, but not with a full spin-echo
description. Sample is Vanadium
H523:The CryoEDM with its polarized beam
H53: The ThALES Cold neutron three-axis spectrometer IN14

For each instrument, a sample can be specified (liquid/powder/amorphous),
with monitoring of the scattering in angular (diffraction) and energy modes
(for spectroscopy).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda |  | central wavelength band for guide illumination [Angs] | 5 |
| dlambda |  | half width of guide wavelength band [Angs] | 4.5 |
| ThALES_lambda |  | ThALES monochromator setting wavelength. Usual 2.4 and 4.2 [Angs] | 4.2 |
| WASP_lambda |  | IN16 monochromator setting wavelength. Usual 3.3 and 6.3 [Angs] | 6.3 |
| D16_lambda |  | D16  monochromator setting wavelength. Usual 4.7 and 5.6 [Angs] | 5.6 |
| SADAM_lambda |  | SuperADAM monochromator setting wavelength. Usual 4.4 [Angs] | 4.4 |
| IN15_lambda |  | IN15 velocity selector setting wavelength [Angs] | 6.5 |
| D22_lambda |  | D22  velocity selector setting wavelength [Angs] | 4.5 |
| D22_collimation |  | D22 collimation length and sample-detector distance [m] | 2 |
| ThALES_sample |  | ThALES liquid/powder/amorphous sample [string] | "Rb_liq_coh.sqw" |
| WASP_sample |  |  | "Rb_liq_coh.sqw" |
| D16_sample |  | D16  liquid/powder/amorphous sample [string] | "H2O_liq.qSq" |
| SADAM_sample |  | SuperADAM liquid/powder/amorphous sample [string] | "SiO2_quartza.laz" |
| D22_sample |  | D22  liquid/powder/amorphous sample [string] | "H2O_liq.qSq" |
| ThALES_RMV |  |  | -1 |
| D16_RMV |  |  | -1 |
| SADAM_RMV |  |  | -1 |
| ThALES_RMH |  |  | -1 |

## Links

- [Source code](ILL_H5_new.instr) for `ILL_H5_new.instr`.
- The NoteDPT11 at the ILL
- The DPT/SMAE 11/070 WASP design report
- The DPT/SMAE 10/271 H5 design report
- Daily notes from K. Andersen about the H5 project
- Mirotron drawing MR-0656-000 for the IN15 V-mirror geometry

---
