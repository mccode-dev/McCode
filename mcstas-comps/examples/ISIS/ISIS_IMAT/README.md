# The `ISIS_IMAT` Instrument

*McStas: IMAT: McStas instrument file of the imaging and diffraction parts of the IMAT instrument.*

## Identification

- **Site:** ISIS
- **Author:** Genoveva Burca (Genoveva.Burca@stfc.ac.uk)
- **Origin:** ISIS
- **Date:** October 2017

## Description

```text
McStas instrument for simulating the IMAT - imaging and diffraction
New McStas moderator file (G. Skoro, ISIS Facility) was added
\\ISIS\Shares\NeutronicsTeam\TS-2\HydroMod_Upgrade\McStas_TS2_HydroMod_Base_newVirtMod_350mm\Let_Base.mcstas
Components positions were extracted from IMAT final design (March 2016)
Choppers were not added to this model
Diffraction detectors are

IMPORTANT NOTES:
1. For reference use G. Burca et al., Modelling of an imaging beamline at the ISIS pulsed neutron source,
Journal of Instrumentation, 8 (2013), no 10, http://dx.doi.org/10.1088/1748-0221/8/10/P10001
2. Results from calculations published in the previous paper were obtained using McStas ISIS_moderator component (Face="W5")
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| l_min | AA | Low wavelength sampled. | 1 |
| l_max | AA | High wavelength sampled. | 10 |
| t_min | t | Low detector time limit. | 0 |
| t_max | t | High detector time limit. | 0.2 |
| theta | deg | Angle that the detector banks are set at. | 90 |

## Links

- [Source code](ISIS_IMAT.instr) for `ISIS_IMAT.instr`.
- <a href="http://www.isis.stfc.ac.uk/instruments/imat">

---
