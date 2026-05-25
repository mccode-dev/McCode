# The `SNS_BASIS` Instrument

*McStas: Written by: N.Tsapatsaris (nikolaos.tsapatsaris@esss.se) and Peter K. Wilendrup (pkwi@fysik.dtu.dk)*

## Identification

- **Site:** SNS
- **Author:** N.Tsapatsaris (nikolaos.tsapatsaris@esss.se) and Peter K. Wilendrup (pkwi@fysik.dtu.dk)
- **Origin:** NBI
- **Date:** 2013-2015

## Description

```text
ESCRIPTION
The approximative, analytic SNS Source description of this instrument is realised
by use of the ESS_moderator_short model, but weighted due to the opposite moderator
arrangement at the SNS and at the once-planned ESS short pluse facility. The correct
SNS Source File to use with SNS_Source is source_sct091_tu_02_1.dat.

The instrument serves as a test instrument for the components
Spherical_Backscattering_Analyser, also written by by Niko Tsapatsaris with help from
Peter Willendrup and Guide_m by Niko Tsapatsaris.

The instrument is based on models initially written by
A) REL (ruep.lechner@gmail.com) and HNB (bordallo@nbi.ku.dk) with contributions
from Johan Jacobsen (johan.fett@gmail.com)
B) G. Granroth

A virtual experiment on BASIS i.e. a comparison with real and simulation results were
published in AIP conference,DOI: 10.1051/epjconf/20158303015 and RSI, DOI: 10.1063/1.4961569

Instrument geometry parameters etc. were taken from the publication:
"A time-of-flight backscattering spectrometer at the Spallation Neutron Source, BASIS" by
Mamontov, E.;   Herwig, K. W.; Neutron Scattering Science Division, Oak Ridge National Laboratory,
Oak Ridge, Tennessee 37831, USA, in Review of Scientific Instruments,82(8),085109 - 085109-10, 2011

This simulation uses the sample component Isotropic_Sqw. We are using the description for a Vanadium
hollow cylindrical sample. One Arm Component is used to place the Analyser Component at an angle
defined as ROT1.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lam | AA | Wavelength selected by the chopper system | 6.4 |
| Lambda_min | AA | Minimum wavelength produced at the source | 5 |
| Lambda_max | AA | Maximum wavelength produced at the source | 7 |
| RadCurv | m | Radius of Curvature of the guide system | 1000 |
| omega1 | Hz | Frequency of the first DiskChopper | 60 |
| omega2 | Hz | Frequency of the second DiskChopper | 60 |
| omega3 | Hz | Frequency of the third DiskChopper | 60 |
| ch1_open | deg | Angular opening of the first Diskchopper | 51.4 |
| ch2_open | deg | Angular opening of the second Diskchopper | 57.6 |
| ch3_open | deg | Angular opening of the third Diskchopper | 171.1 |
| ROT1 | deg | Positioning of central analyser wrt. the incoming beam, in the scattering plane | 90 |
| AN_ROT | deg | Out-of-plane rotation of analysers wrt. scattering plane | 2 |
| TOTAL_LENGTH | m | Total length of the guide system | 84 |
| dROT | deg | Positioning of neighbouring analysers wrt. central analyser | 11 |

## Links

- [Source code](SNS_BASIS.instr) for `SNS_BASIS.instr`.
- A reference/HTML link for more information

---
