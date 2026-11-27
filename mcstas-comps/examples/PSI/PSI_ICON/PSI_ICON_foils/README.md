# The `PSI_ICON_foils` Instrument

*McStas: McStas model of the ICON imaging instrument at PSI, CH.*

## Identification

- **Site:** PSI
- **Author:** Peter Willendrup (DTU/ESS), Estrid B. Naver (DTU), based on earlier model from Ludovic Giller.
- **Origin:** DTU/ESS
- **Date:** June, 2021

## Description

```text
McStas model of the ICON imaging instrument at PSI, CH. Based on earlier model from Ludovic Giller, see links below.
Beamline and camera are %include'd from PSI_ICON_beamline and PSI_ICON_camera.

The model was developed for an ICON beamtime focussing on exeriments using 'Propagation-based phase-contrast' aka. the Paganin-method.
The sample in the model is a series of metal sheets, each with different scattring/refracting properties. A refined version of this
instrument file was later used in the publication by Estrid B. Naver et. al., see links below.
```

## Examples

- **Test: PSI_ICON_foils.instr -y Detector: VarDetectorZoom_I=852147**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| eventmode | 1 | Flag to store neutron events with calculated sample-detector travel | 0 |
| ty | m | Goniometer y-translation | -0.0075 |
| delta_eps | m | Finite distance separating metal-slabs in sample | 1e-9 |
| delta_y | 1 | Size-factor between slabs of Zr and Al | 0.1 |

## Links

- [Source code](PSI_ICON_foils.instr) for `PSI_ICON_foils.instr`.
- <a href="https://doi.org/10.1016/j.nima.2007.11.061">Ludovic Gilller et. al., NIMA 586 (2008) pp. 59-63</a>
- <a href="https://doi.org/10.1107/S1600576724003030">Estrid B. Naver et. al., JAC 57 (2024) pp.701-713</a>

---
