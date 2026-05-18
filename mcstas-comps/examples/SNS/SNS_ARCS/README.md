# The `SNS_ARCS` Instrument

*McStas: Model of the ARCS spectrometer from SNS.*

## Identification

- **Site:** SNS
- **Author:** G. Granroth
- **Origin:** SNS
- **Date:** Nov 2014

## Description

```text
When using this ARCS instrument model, please reference
<ul>
<li>Granroth, G. E. and Abernathy, D. L., "Performance Comparisons of Four Direct Geometry
Spectrometers Planned for Spallation Neutron Source",  Proceedings of ICANS-XVI, 289 (2003).
<li>D. L. Abernathy,  M. B. Stone,  M. J. Loguillo, M. S. Lucas, O. Delaire, X.Tang, J. Y. Y. Lin, and
B. Fultz,"Design and operation of the wide angular-range chopper spectrometer ARCS at the
Spallation Neutron Source", Review of Scientific Instruments, 83 , 015114 (2012)</ul>
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| filename |  |  | "source_sct521_bu_17_1.dat" |
| Fermi_nu | Hz | Frequency of the Fermi chopper | 420 |
| T0_nu | Hz | Frequency of the T0 chopper | 90 |
| nrad | m | Radius of the Fermi chopper blades | 0.58 |
| nchans | 1 | Number of channels in the Fermi chopper | 40 |
| Edes | meV | Desired/target energy | 50 |
| Et | meV | Energy transfer of the Spot_sample | 25 |
| ttheta | deg | Scattering angle of the Spot_sample | 25 |
| T0_off |  |  | 0 |
| sxmin | m | Sample slit horz min value | -0.04 |
| sxmax | m | Sample slit horz max value | 0.04 |
| symin | m | Sample slit vert min value | -0.04 |
| symax | m | Sample slit vert max value | 0.04 |
| run_num | 1 | Virtual source run number (unused at present) | 1 |

## Links

- [Source code](SNS_ARCS.instr) for `SNS_ARCS.instr`.
- <br>Granroth, G. E. and Abernathy, D. L., "Performance Comparisons of Four Direct Geometry Spectrometers Planned for Spallation Neutron Source",  Proceedings of ICANS-XVI, 289 (2003).
- <br>D. L. Abernathy,  M. B. Stone,  M. J. Loguillo, M. S. Lucas, O. Delaire, X.Tang, J. Y. Y. Lin, and  B. Fultz,"Design and operation of the wide angular-range chopper spectrometer ARCS at the Spallation Neutron Source", Review of Scientific Instruments, 83 , 015114 (2012)

---
