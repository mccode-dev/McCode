# The `SAXS_saxlab` Instrument

*McXtrace: SAXSLab small-angle scattering (SAXS)*

## Identification

- **Site:** SAXSLAB
- **Author:** 
- **Origin:** 
- **Date:** 

## Description

```text
The small-angle scattering (SAXS) instrument generally consists of the following parts:
the X-ray source ( the system includes rotating copper anode and KB multilayermirrors),
a pinhole collimation system and a detector.

The geometry of this instument is such that an x-ray source (turned 45 deg.) shines light
on Montel mirror pair. The mirror pair is mounted such that the doubly reflected radiation
is deflected downwards (negative y-axis). This means the the mirror join is pointing upwards, and th
mirrors themselves extending +-45 degrees downwards.
There are slits attached to the mirror exits
This arrangement is designed to closesly resemble the SAXSlab system installed at
NBI (University of Copenhagen)

The evolution of the beam is tracked using a series of 2D PSD monitors situated along the beam axis,
which points slightly down.
In real life the beam axis is parallel to the ground and the source is shining slightly upwards.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Gamma | deg | Nominal mirror glancing  angle | 1.2 |
| GammaP | deg | When given, Gamma=Gamma/sqrt(2) | 0 |
| S1 | m | Distance from the source to the multilayer's surface | .045 |
| S2 | m | Focal distance of the multilayer | .9 |
| Energy | keV | Primary energy of the photon beam | 8.05 |
| mirrorin |  | Flag controlling whether or not the mirror is active. | 3 |
| pin1 |  | Flag controlling whther the 1st pinhole is active. | 1 |
| pin2 |  | Flag controlling whther the 2nd pinhole is active. | 1 |
| pin3 |  | Flag controlling whther the 3rd pinhole is active. | 1 |
| Lpsd1 | m | Distance from mirror centre to 1st diagnostic PSD. | 0.160 |
| Lpsd2 | m | Distance from vacumm chamber entry to 2nd diagnostic PSD. | 0.15 |
| Lpsd3 | m | Distance from vacumm chamber entry to 3rd diagnostic PSD. | 1.2 |
| detPos | m | Distance from vacuum chamber entry to Detector. | 1.41 |
| detOffSet | m | Final detector shift along Y | 3e-2 |

## Links

- [Source code](SAXS_saxlab.instr) for `SAXS_saxlab.instr`.

---
