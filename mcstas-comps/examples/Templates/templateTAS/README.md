# The `templateTAS` Instrument

*McStas: Template RESCAL type triple-axis machine (TAS)*

## Identification

- **Site:** Templates
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 2006

## Description

```text
This instrument is a simple model of triple-axis spectrometer.
It is directly illuminated by the moderator,
and has curved monochromator and analyzer.
Sample can be specified as a powder, liquid/amorphous or a pure incoherent
scatterer (e.g. vanadium, which is the default).
Sample geometry can be spherical or cylindrical.
For PG002 monochromator/analyzer setting, a reflectivity curve vs. wavelength
is used, else reflectivity is set to 70 %. To suppress collimators, set their
divergence to 0 (ALFn BETn).
Default instrument geometry is from IN20@ILL with PG monochromator/analyzer.

Monochromator lattice parameter
PG       002 DM=3.355 AA (Highly Oriented Pyrolythic Graphite)
PG       004 DM=1.607 AA
Heusler  111 DM=3.362 AA (Cu2MnAl)
CoFe         DM=1.771 AA (Co0.92Fe0.08)
Ge       111 DM=3.266 AA
Ge       311 DM=1.714 AA
Ge       511 DM=1.089 AA
Ge       533 DM=0.863 AA
Si       111 DM=3.135 AA
Cu       111 DM=2.087 AA
Cu       002 DM=1.807 AA
Cu       220 DM=1.278 AA
Cu       111 DM=2.095 AA

IN22 configuration (at H25 thermal m=2 guide end):
KI=3.84, QM=1.0, EN=0.0, verbose=1,
WM=0.15, HM=0.12,    NHM=1, NVM=9, RMV=-1,
WA=0.20, HA=0.10,  NHA=11, NVA=3, RAV=-1, RAH=-1,
SM=-1, SS=1, SA=-1,
L1=10.0, L2=1.7, L3=1.0, L4=0.8

IN8 Configuration: with Copper optics
KF=5, KI=0, QM=0.5, EN=5, verbose=1
WM=0.23, HM=0.19, RMH=-1, RMV=-1, DM=1.807, NHM=15, NVM=15,
WA=0.16, HA=0.08, RAH=-1, RAV=-1, DA=2.087, NHA=15, NVA=15,
L1=2.33
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| KI | Angs-1 | Incoming neutron wavevector | 2.662 |
| KF | Angs-1 | Outgoing neutron wavevector | 0 |
| EI | meV | Incoming neutron energy | 0 |
| EF | meV | Outgoing neutron energy | 0 |
| QH | rlu | Measurement QH position in crystal | 0 |
| QK | rlu | Measurement QK position in crystal | 0 |
| QL | rlu | Measurement QL position in crystal | 0 |
| EN | meV | Energy transfer in crystal | 0 |
| QM | Angs-1 | Wavevector transfer in crystal | 0 |
| KFIX | Angs-1 | Fixed KI or KF value for Rescal compatibility | 0 |
| FX | 1:KI,2:KF | Fixed KI or KF type for Rescal compatibility | 0 |
| L1 | m | Source-Monochromator distance. Contains 1st Collimator of length 5.34 | 9 |
| L2 | m | Monochromator-Sample distance. Contains 2nd Collimator of length 0.35 | 2.1 |
| L3 | m | Sample-Analyzer distance. Contains 3rd Collimator of length 0.40 | 1.5 |
| L4 | m | Analyzer-detector distance. Contains 4th Collimator of length 0.24 | 0.7 |
| SM | 1:left, -1:right | Scattering sense of beam from Monochromator | 1 |
| SS | 1:left, -1:right | Scattering sense of beam from Sample | -1 |
| SA | 1:left, -1:right | Scattering sense of beam from Analyzer | 1 |
| DM | Angs | Monochromator d-spacing | 3.3539 |
| DA | Angs | Analyzer d-spacing | 3.3539 |
| RMV | m | Monochromator vertical curvature, 0 for flat, -1 for automatic setting | -1 |
| RMH | m | Monochromator horizontal curvature, 0 for flat, -1 for automatic setting | 0 |
| RAV | m | Analyzer vertical curvature, 0 for flat, -1 for automatic setting | 0 |
| RAH | m | Analyzer horizontal curvature, 0 for flat, -1 for automatic setting | -1 |
| ETAM | arc min | Monochromator mosaic | 30 |
| ETAA | arc min | Analyzer mosaic | 30 |
| ALF1 | arc min | Horizontal collimation from Source to Monochromator | 60 |
| ALF2 | arc min | Horizontal collimation from Monochromator to Sample A | 60 |
| ALF3 | arc min | Horizontal collimation from Sample to Analyzer | 60 |
| ALF4 | arc min | Horizontal collimation from Analyzer to Detector | 60 |
| BET1 | arc min | Vertical collimation from Source to Monochromator | 120 |
| BET2 | arc min | Vertical collimation from Monochromator to Sample A | 120 |
| BET3 | arc min | Vertical collimation from Sample to Analyzer | 120 |
| BET4 | arc min | Vertical collimation from Analyzer to Detector | 120 |
| AS | Angs | Sample lattice parameter A | 6.28 |
| BS | Angs | Sample lattice parameter B | 6.28 |
| CS | Angs | Sample lattice parameter C | 6.28 |
| AA | deg | Angle between lattice vectors B,C | 90 |
| BB | deg | Angle between lattice vectors C,A | 90 |
| CC | deg | Angle between lattice vectors A,B | 90 |
| AX | rlu | First reciprocal lattice vector in scattering plane, X | 1 |
| AY | rlu | First reciprocal lattice vector in scattering plane, Y | 0 |
| AZ | rlu | First reciprocal lattice vector in scattering plane, Z | 0 |
| BX | rlu | Second reciprocal lattice vector in scattering plane, X | 0 |
| BY | rlu | Second reciprocal lattice vector in scattering plane, Y | 1 |
| BZ | rlu | Second reciprocal lattice vector in scattering plane, Z | 0 |
| verbose | 1 | print TAS configuration. 0 to be quiet | 1 |
| A1 | deg | Monohromator rotation angle | 0 |
| A2 | deg | Monohromator take-off angle | 0 |
| A3 | deg | Sample rotation angle | 0 |
| A4 | deg | Sample take-off angle | 0 |
| A5 | deg | Analyzer rotation angle | 0 |
| A6 | deg | Analyzer take-off angle | 0 |
| NHM | 1 | Number of horizontal slabs composing the monochromator | 1 |
| NVM | 1 | Number of vertical slabs composing the monochromator | 9 |
| WM | m | Width of monochromator | 0.10 |
| HM | m | Height of monochromator | 0.12 |
| NHA | 1 | Number of horizontal slabs composing the analyzer | 9 |
| NVA | 1 | Number of vertical slabs composing the analyzer | 1 |
| WA | m | Width of analyzer | 0.10 |
| HA | m | Height of analyzer | 0.12 |
| Sqw_coh | str | sample coherent S(q,w) file name. Use LAZ/LAU or SQW file | "NULL" |
| Sqw_inc | str | sample incoherent S(q,w) file name. Use NULL to scatter incoherently | "NULL" |
| radius | m | outer radius of sample hollow cylinder/sphere | 0.01 |
| thickness | m | thickness of sample hollow cylinder. 0 for bulk | 0.005 |
| height | m | sample height. Use 0 for a spherical shape | 0.05 |

## Links

- [Source code](templateTAS.instr) for `templateTAS.instr`.
- Rescal for Matlab at http://www.ill.eu/instruments-support/computing-for-science/cs-software/all-software/matlab-ill/
- Restrax at http://omega.ujf.cas.cz/restrax/

---
