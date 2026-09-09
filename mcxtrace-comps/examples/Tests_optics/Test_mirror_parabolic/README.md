# The `Test_mirror_parabolic` Instrument

*McXtrace: Unit test instrument for Mirror_parabolic*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jan '21

## Description

```text
This is a test instrument for Mirror parabolic. A collimated beam impinges on a
with normal incidence on a parabolic mirror, is reflected to the focal point, and
back to flat detector some distance, D, away.
```

## Examples

- **Test: D=0.0025 MM=1 Detector: detBB_I=1**
- **Test: D=1 MM=1 Detector: detBB_I=0.104959**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| D | m | Distance between mirror apex and detector plane. | 1 |
| MM |  | Flag to deactivate the mirror. | 1 |

## Links

- [Source code](Test_mirror_parabolic.instr) for `Test_mirror_parabolic.instr`.

---
