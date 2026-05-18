# The `XFEL_SPB` Instrument

*McXtrace: European XFEL SPB beamline (on SASE1)*

## Identification

- **Site:** XFEL
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Fysik
- **Date:** End of the World

## Description

```text
A simple and early model of the SPB-beamline at the European XFEL in Hamburg.
This model includes the long beam transport from the source all the way into
experimental hutch.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L0 | AA | The central wavelength to be simulated. | 1.3051 |
| DL | AA | Half width of energy ointerval to be simulated | 0.01 |
| D | m | Sample to detector distance | 10 |
| offset_omega_rad | rad | Incidence angle of the upstream offset mirror. | 3.6e-3 |
| offset2_rad | m | Radius of curvature of 2nd offset mirror. | 50000 |
| preCRL | 0/1 | Flag to choose whether pre-focusing CRLs are active. | 0 |
| FXE | 0/1 | Flag to enable the FXE distribution mirror. Useful for debugging. | 0 |
| R1 | m | Radius of curvature of 1st focusing mirror. | 20000 |
| R2 | m | Radius of curvature of 2nd focusing mirror. | 20000 |

## Links

- [Source code](XFEL_SPB.instr) for `XFEL_SPB.instr`.

---
