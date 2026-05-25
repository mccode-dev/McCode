# The `ESRF_ID11` Instrument

*McXtrace: Model of the ESRF ID11 Transfocator based beamline.*

## Identification

- **Site:** ESRF
- **Author:** E. Knudsen (erkn@risoe.dtu.dk)
- **Origin:** Ris&oslash; DTU
- **Date:** Nov. 26th, 2009

## Description

```text
Model of the ESRF ID11 Transfocator based beamline.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| ANGLE | deg | Rotation (misalignment) of first transfocator | 0 |
| SOURCE |  | 1) Choose rectangular source with defined divergence. 2) Choose Gaussian cross-section source | 0 |
| T1_N |  | Number of Be lenses in 1st IVT transfocator @31.5m | 16 |
| T2_N |  | Number of Al lenses in 1st IVT transfocator @31.5m _currently deactivated by default_ | 16 |
| T3_N |  | Number of Al lenses in 2nd IVT transfocator @92.25 m | 16 |
| SI_N |  | Number of Si lenses in Si microfocus chip transfocator @94 m | 2 |
| IVT1BE |  | If nonzero the set of Be-lenses is chosen for the IVT @31.5m. If 0 the Al set is active. | 1 |

## Links

- [Source code](ESRF_ID11.instr) for `ESRF_ID11.instr`.
- http://www.esrf.eu/UsersAndScience/Experiments/StructMaterials/ID11/ID11Source

---
