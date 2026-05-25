# The `ILL_Lagrange` Instrument

*McStas: IN1-Lagrange hot neutrons spectrometer for liquids at the ILL.*

## Identification

- **Site:** ILL
- **Author:** E. Farhi
- **Origin:** LLB/ILL
- **Date:** 13 Apr 2006

## Description

```text
IN1-Lagrange hot neutrons spectrometer for liquids at the ILL.

The dedicated liquid and amorphous instrument Lagrange is a spectrometer with
constant final neutron energy, and variable incoming neutron energy.
The analyser is a focusing barrel covered with PG analyser, all focusing to
the detector. The flux is thus very high, and the resolution is that given
buy the PG crystal mocaicity.

The monochromator take-off angle is 2Theta_M ~ 20-25 deg, variable. The available
monochromators are all vertically focusing, Cu (200) for 0.7 Angs, Cu (220)
for 0.5 Angs.

Cu       002 DM=1.807 AA
Cu       220 DM=1.278 AA
PG       002 DM=3.355 AA
--------------------------
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator | 0.897 |
| DM | Angs | d-spacing of monochromator | 1.807 |
| RV | m | Monochromator vertical curvature, 0=flat, -1=automatic | -1 |
| coh | str | File name for sample coherent scattering contribution | "Rb_liq_coh.sqw" |
| inc | str | File name for sample incoherent scattering contribution | "Rb_liq_inc.sqw" |
| L1 | m | Source-Monochromator distance | 6.35 |
| L2 | m | Monochromator-Sample distance | 2.55 |
| L3 | m | Sample-Detector distance | 0.901 |
| verbose | 1 | Print spectrometer configuration. 0 to be quiet | 1 |

## Links

- [Source code](ILL_Lagrange.instr) for `ILL_Lagrange.instr`.
- The <a href="http://www.ill.eu/d4">D4 diffractometer</a> at the ILL

---
