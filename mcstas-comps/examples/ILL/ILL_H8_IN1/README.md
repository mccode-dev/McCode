# The `ILL_H8_IN1` Instrument

*McStas: Hot neutron three-axis spectrometer IN1@ILL*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 2006

## Description

```text
IN8 is installed on beamtube H10 (diameter F = 200 mm).

IN1 works in a time-sharing mode. This means that the same monochromator is
also used by the Be-filter spectrometer IN1-BeF and by the liquids
diffractometer D4. Changing over between the three different instruments can be
done without difficulty in about two hours.

The monochromator unit carries three different vertically focussing
monochromators built from copper single crystals (available reflecting planes
Cu(200), Cu(220) and Cu(331)). The exchange of the monochromator planes is
controlled by the instrument computer. The radius of curvature can be
automatically adjusted as function of reflected energy in order to maintain
maximal flux at the sample position in the course of energy scans. The
scattering angles on the monochromator cover a range of 10<2theta_m <90
allowing for scanning neutron energies from 13 meV to more than 1 eV..

The IN1-TAS spectrometer: the scattering angles at the sample and the analyser
can be changed in the intervals -115<2theta_S<115 and -120<2theta_S<120. Three
different analysers (PG(002), Cu(200), Cu(220)) can be installed in order to
optimise intensity and resolution for a given experiment. Various resonance
absorption filters (e.g. Er, Sm, Hf ...) can be used to suppress higher order
contaminations from the incident beam or in the scattered beam. An oriented
Pyrolytic Graphite filter is designed for experiments eventually demanding
thermal neutron energy range.

In this TAS configuration, Cu220 are used as monochromator and analyser,
with a single type detector.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| KF | Angs-1 | Outgoing neutron wavevector | 10 |
| KI | Angs-1 | Incoming neutron wavevector | 0 |
| QM | Angs-1 | Wavevector transfer in crystal | 0.5 |
| EN | meV | Energy transfer in crystal | 0 |
| verbose | 1 | print TAS configuration. 0 to be quiet | 1 |
| WM | m | Width of monochromator | 0.18 |
| HM | m | Height of monochromator | 0.20 |
| RMH | m | Monochromator horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| RMV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| DM | Angs | Monochromator d-spacing | 1.278 |
| NHM | 1 | Number of horizontal slabs composing the monochromator | 15 |
| NVM | 1 | Number of vertical slabs composing the monochromator | 15 |
| WA | m | Width of analyzer | 0.16 |
| HA | m | Height of analyzer | 0.12 |
| RAH | m | Analyzer horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| RAV | m | Analyzer vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| DA | Angs | Analyzer d-spacing | 1.278 |
| NHA | 1 | Number of horizontal slabs composing the analyzer | 15 |
| NVA | 1 | Number of vertical slabs composing the analyzer | 15 |
| L1 | m | Source-Monochromator distance. Contains 1st Collimator of length 5.34 | 7 |
| ALF1 | arc min | Horizontal collimation from Source to Monochromator | 120 |
| ALF2 | arc min | Horizontal collimation from Monochromator to Sample A | 120 |
| ALF3 | arc min | Horizontal collimation from Sample to Analyzer | 120 |
| ALF4 | arc min | Horizontal collimation from Analyzer to Detector | 120 |
| BET1 | arc min | Vertical collimation from Source to Monochromator | 120 |
| BET2 | arc min | Vertical collimation from Monochromator to Sample A | 120 |
| BET3 | arc min | Vertical collimation from Sample to Analyzer | 120 |
| BET4 | arc min | Vertical collimation from Analyzer to Detector | 120 |

## Links

- [Source code](ILL_H8_IN1.instr) for `ILL_H8_IN1.instr`.
- Rescal for Matlab at http://www.ill.eu/instruments-support/computing-for-science/cs-software/all-software/matlab-ill/
- Restrax at http://omega.ujf.cas.cz/restrax/

---
