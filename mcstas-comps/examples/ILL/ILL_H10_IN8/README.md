# The `ILL_H10_IN8` Instrument

*McStas: Thermal neutron three-axis spectrometer IN8@ILL*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 2006

## Description

```text
IN8 is installed on beamtube H10 (diameter F = 200 mm).
The incident wavelength selection is obtained through a double focusing
monochromator , which has three faces equipped with PG002, Cu200 and bent
perfect Si111 crystals, respectively. Horizontal focusing allows increasing the
monochromatic flux at the expense of momentum but not energy resolution when
the horizontal virtual source (an adjustable entrance slit) is introduced at a
distance before the monochromator which matches the monochromator-sample
distance.

The aperture of the horizontal virtual source can be varied but is typically
kept below 50 mm. This reduces the background level of the instrument.
Converging collimators as well as diaphragms can be placed in the beam path
before and after the monochromator to optimize the beam dimension and
definition. For special high-resolution experiments using a flat monochromator,
Soller collimators are available.

The shielding drum allows varying the monochromator take-off angle in the range
10 < 2theta_m < 90. The secondary spectrometer has heavy borated polyethylene
shielding installed around the analyzer and detector elements. The scattering
angle at the sample position is in the range 0 < 2theta_s < 130 independent of
the monochromator take-off angle.

In this TAS configuration, Cu200 is used as monochromator and Cu111 as
analyser, with a single type detector.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| KF | Angs-1 | Outgoing neutron wavevector | 5 |
| KI | Angs-1 | Incoming neutron wavevector | 0 |
| QM | Angs-1 | Wavevector transfer in crystal | 0.5 |
| EN | meV | Energy transfer in crystal | 0 |
| verbose | 1 | print TAS configuration. 0 to be quiet | 1 |
| WM | m | Width of monochromator | 0.233 |
| HM | m | Height of monochromator | 0.197 |
| RMH | m | Monochromator horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| RMV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| DM | Angs | Monochromator d-spacing | 1.807 |
| NHM | 1 | Number of horizontal slabs composing the monochromator | 15 |
| NVM | 1 | Number of vertical slabs composing the monochromator | 15 |
| WA | m | Width of analyzer | 0.16 |
| HA | m | Height of analyzer | 0.08 |
| RAH | m | Analyzer horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| RAV | m | Analyzer vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| DA | Angs | Analyzer d-spacing | 2.087 |
| NHA | 1 | Number of horizontal slabs composing the analyzer | 15 |
| NVA | 1 | Number of vertical slabs composing the analyzer | 15 |
| L1 | m | Source-Monochromator distance. Contains 1st Collimator of length 5.34 | 2.3 |
| ALF1 | arc min | Horizontal collimation from Source to Monochromator | 60 |
| ALF2 | arc min | Horizontal collimation from Monochromator to Sample A | 60 |
| ALF3 | arc min | Horizontal collimation from Sample to Analyzer | 60 |
| ALF4 | arc min | Horizontal collimation from Analyzer to Detector | 60 |
| BET1 | arc min | Vertical collimation from Source to Monochromator | 120 |
| BET2 | arc min | Vertical collimation from Monochromator to Sample A | 120 |
| BET3 | arc min | Vertical collimation from Sample to Analyzer | 120 |
| BET4 | arc min | Vertical collimation from Analyzer to Detector | 120 |

## Links

- [Source code](ILL_H10_IN8.instr) for `ILL_H10_IN8.instr`.
- Rescal for Matlab at http://www.ill.eu/instruments-support/computing-for-science/cs-software/all-software/matlab-ill/
- Restrax at http://omega.ujf.cas.cz/restrax/

---
