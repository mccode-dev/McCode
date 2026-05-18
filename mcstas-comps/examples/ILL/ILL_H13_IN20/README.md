# The `ILL_H13_IN20` Instrument

*McStas: Thermal neutron three-axis spectrometer IN20@ILL (unpolarized configuration)*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 2006

## Description

```text
IN20 is installed at the H13 thermal beam tube (Phi 170 mm) in the reactor hall.

Both the primary and the secondary spectrometer employ a monochromatic
horizontal focusing geometry. A heavy input slit of an adjustable size, placed
in the casemate, serves as a virtual source, providing a large solid angle for
the monochromatic beam, while reducing, together with a sapphire filter window,
the fast neutron background. The neutron energy is selected either by a doubly
focusing polarizing Heusler 111 monochromator (230 x 150 mm2 w x h) or by an
unpolarised Si 111 monochromator (elastically bent crystals, 195 x 200 mm2 w x
h) free of higher-order contamination in the incident beam at wave-numbers ki >
3 Angs-1. The analysis of the energy and polarisation state of the scattered
neutrons is effectuated by a similar horizontally focusing Heusler crystal
analyzer. Further PG 002 and Si 111 analyzers are available for occasional
unpolarised work.

The energy transfer range accessible in the present configuration of IN20
extends to 100 meV with maximum incident neutron energies reaching 150 meV. The
typical energy widths (FWHM) measured with a reference vanadium sample at the
graphite filter wave-numbers ki = 2.66 A-1 and 4.1 A-1 are 0.82(3) meV and 3.05
(15) meV, respectively.

This model uses two Si111 monochromator and analyzers (unpolarized
configuration).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| KF | Angs-1 | Outgoing neutron wavevector | 3 |
| KI | Angs-1 | Incoming neutron wavevector | 0 |
| QM | Angs-1 | Wavevector transfer in crystal | 0.5 |
| EN | meV | Energy transfer in crystal | 0 |
| verbose | 1 | print TAS configuration. 0 to be quiet | 1 |
| WM | m | Width of monochromator | 0.20 |
| HM | m | Height of monochromator | 0.19 |
| RMH | m | Monochromator horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| RMV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| DM | Angs | Monochromator d-spacing | 3.155 |
| NHM | 1 | Number of horizontal slabs composing the monochromator | 15 |
| NVM | 1 | Number of vertical slabs composing the monochromator | 15 |
| WA | m | Width of analyzer | 0.16 |
| HA | m | Height of analyzer | 0.08 |
| RAH | m | Analyzer horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| RAV | m | Analyzer vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| DA | Angs | Analyzer d-spacing | 3.155 |
| NHA | 1 | Number of horizontal slabs composing the analyzer | 15 |
| NVA | 1 | Number of vertical slabs composing the analyzer | 15 |
| L1 | m | Source-Monochromator distance. Contains 1st Collimator of length 5.34 | 2.33 |
| ALF1 | arc min | Horizontal collimation from Source to Monochromator | 60 |
| ALF2 | arc min | Horizontal collimation from Monochromator to Sample A | 60 |
| ALF3 | arc min | Horizontal collimation from Sample to Analyzer | 60 |
| ALF4 | arc min | Horizontal collimation from Analyzer to Detector | 60 |
| BET1 | arc min | Vertical collimation from Source to Monochromator | 120 |
| BET2 | arc min | Vertical collimation from Monochromator to Sample A | 120 |
| BET3 | arc min | Vertical collimation from Sample to Analyzer | 120 |
| BET4 | arc min | Vertical collimation from Analyzer to Detector | 120 |

## Links

- [Source code](ILL_H13_IN20.instr) for `ILL_H13_IN20.instr`.
- Rescal for Matlab at http://www.ill.eu/instruments-support/computing-for-science/cs-software/all-software/matlab-ill/
- Restrax at http://omega.ujf.cas.cz/restrax/

---
