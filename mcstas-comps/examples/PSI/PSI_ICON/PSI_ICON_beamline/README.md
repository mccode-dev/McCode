# The `PSI_ICON_beamline` Instrument

*McStas: Beamline of the ICON imaging instrument at PSI, CH, up to the sample goniometer.*

## Identification

- **Site:** PSI
- **Author:** Peter Willendrup (DTU/ESS), Estrid B. Naver (DTU), based on earlier model from Ludovic Giller.
- **Origin:** DTU/ESS
- **Date:** June, 2021

## Description

```text
Beamline of the ICON imaging instrument at PSI, CH, up to the sample
goniometer (use all_mons=1 for monitors). Based on earlier model from Ludovic Giller.

Building block, %include'd by PSI_ICON (open beam), PSI_ICON_foils,
PSI_ICON_porous, PSI_ICON_porous_Union and PSI_ICON_porous_Union_NCrystal:
%include "PSI/PSI_ICON/PSI_ICON_beamline/PSI_ICON_beamline.instr"
(sample, AT ... RELATIVE Goniometer)
%include "PSI/PSI_ICON/PSI_ICON_camera/PSI_ICON_camera.instr"
```

## Examples

- **Test: PSI_ICON_beamline.instr all_mons=1 Detector: PSD_sample_I=745699**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda_min | AA | Minimum wavelength on wavelength monitors | 0.3 |
| lambda_max | AA | Maximum wavelength on wavelength monitors | 10.0 |
| all_mons | 1 | Flag to enable all monitors in the instrument | 0 |
| nx | 1 | Number of x-pixels, camera | 512 |
| ny | 1 | Number of y-pixels, camera | 512 |
| Lambda_Min | AA | Minimum wavelength produced from source | 4.1 |
| Lambda_Max | AA | Maximum wavelength produced from source | 10 |
| sp | m | Overall goniometer z-position wrt. camera | 0 |
| tx | m | Goniometer x-translation | 0 |
| ty | m | Goniometer y-translation | 0 |
| tz | m | Goniometer z-translation | 0 |
| rx | m | Goniometer x-rotation | 0 |
| ry | m | Goniometer y-rotation | 0 |
| rz | m | Goniometer z-rotation | 0 |

## Links

- [Source code](PSI_ICON_beamline.instr) for `PSI_ICON_beamline.instr`.
- <a href="https://doi.org/10.1016/j.nima.2007.11.061">Ludovic Gilller et. al., NIMA 586 (2008) pp. 59-63</a>

---
