# The `Test_FocalisationMirrors` Instrument

*McStas: Test instrument for neutron focalisation with a set of supermirrors.
No guide / velocity selector
One parabolic SM converges the incoming beam to its focal point, then one
elliptic SM (with primary focal point at same location as parabolic SM focal point)
images the focal point onto the detector.*

## Identification

- **Site:** Tests_optics
- **Author:** <a href="mailto:desert@drecam.cea.fr">Sylvain Desert</a>
- **Origin:** <a href="http://www-llb.cea.fr/">LLB</a>
- **Date:** 2007.

## Description

```text
Test instrument for the MirrorElli and MirrorPara components
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Qmin | AA^-1 | Minimum momentum transfer on the detector | .0001 |
| G | 1 | Ratio of the mirror focal length = increase in accepted divergence angle | 1 |
| H | m | Height of mirrors | .0001 |
| F | m | Parabolic focal | .00066 |
| DET | m | Distance between the two elliptic focal points | 8.2 |
| lambda | AA | Mean wavelength of neutrons | 14 |
| divergence | deg | Source divergence | .1 |
| BeamWidth | m | Length of the source to focalize | .05 |
| TetaMin | deg | Minimum angle that must be reflected by the device assuming m=3 SM (TetaMin=3 for 5A and above neutron) | 3 |

## Links

- [Source code](Test_FocalisationMirrors.instr) for `Test_FocalisationMirrors.instr`.

---
