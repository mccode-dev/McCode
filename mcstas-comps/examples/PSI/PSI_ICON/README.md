# The `PSI_ICON` Instrument

*McStas: McStas model of the ICON imaging instrument at PSI, CH.*

## Identification

- **Site:** PSI
- **Author:** Peter Willendrup (DTU/ESS), Estrid B. Naver (DTU), based on earlier model from Ludovic Giller.
- **Origin:** DTU/ESS
- **Date:** June, 2021

## Description

```text
McStas model of the ICON imaging instrument at PSI, CH. Based on earlier model from Ludovic Giller, see links below.

The model was developed for an ICON beamtime focussing on exeriments using 'Propagation-based phase-contrast' aka. the Paganin-method.
The sample in the model is a series of metal sheets, each with different scattring/refracting properties. A refined version of this
instrument file was later used in the publication by Estrid B. Naver et. al., see links below.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda_min | AA | Minimum wavelength on wavelength monitors | 0.3 |
| lambda_max | AA | Maximum wavelength on wavelength monitors | 10.0 |
| eventmode | 1 | Flag to store neutron events with calculated sample-detector travel | 0 |
| nx | 1 | Number of x-pixels, camera | 512 |
| ny | 1 | Number of y-pixels, camera | 512 |
| Lambda_Min | AA | Minimum wavelength produced from source | 4.1 |
| Lambda_Max | AA | Maximum wavelength produced from source | 10 |
| sp | m | Overall goniometer z-position wrt. camera | 0 |
| tx | m | Goniometer x-translation | 0 |
| ty | m | Goniometer y-translation | -0.0075 |
| tz | m | Goniometer z-translation | 0 |
| rx | m | Goniometer x-rotation | 0 |
| ry | m | Goniometer y-rotation | 0 |
| rz | m | Goniometer z-rotation | 0 |
| delta_eps | m | Finite distance separating metal-slabs in sample | 1e-9 |
| delta_y | 1 | Size-factor between slabs of Zr and Al | 0.1 |

## Links

- [Source code](PSI_ICON.instr) for `PSI_ICON.instr`.
- <a href="https://doi.org/10.1016/j.nima.2007.11.061">Ludovic Gilller et. al., NIMA 586 (2008) pp. 59-63</a>
- <a href="https://doi.org/10.1107/S1600576724003030">Estrid B. Naver et. al., JAC 57 (2024) pp.701-713</a>

---
