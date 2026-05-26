# The `HZB_NEAT` Instrument

*McStas: V3 Time-of-Flight-Spectrometer (NEAT) at BENSC, 1995 version.*

## Identification

- **Site:** HZB
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a> and R. Lechner
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>/<a href="http://www.helmholtz-berlin.de/user/neutrons/index_en.html">BENSC (Germany)</a>
- **Date:** 2011

## Description

```text
In time-of-flight spectrometers neutron pulses are created by us-
ing mechanical chopper devices, realised on NEAT by fast rotating
discs with speeds up to 20000 RPM. The discs are coated with neu-
tron absorbing materials except for the narrow windows. Phased to
each other according to the flight time of neutrons between them,
the choppers 'cut out' pulses of neutrons with a desired wave-
length from the white beam. An interaction with moving atoms in
the sample changes the velocities of the scattered neutrons and
this is detected by the secondary part of the spectrometer on the
basis of the time of flight between the sample and the neutron de-
tectors at 2.5 m distance. The secondary spectrometer of NEAT
contains an array of 388 3He 60 cm2 area single counter detectors
(SD) for the large-angle scattering.

The NL 2 (upper part) guide is modelled.
This model only contains the first and last choppers, and has only a single
pulse (no frame overlap). The sample is a 2mm thick plate rotated by 45 degrees,
which material can be any powder/liquid/amorphous sample.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | incident wavelength on sample | 6 |
| dlambda | Angs | wavelength spread shot from the source | 0.05 |
| rpm | rpm | disk chopper rotation speed, setting the resolution, Hz=rpm/60. | 10000 |
| coh | str | sample coherent S(q,w) file name. Use LAZ/LAU or SQW file | "Rb_liq_coh.sqw" |
| inc | str | sample incoherent S(q,w) file name. Use NULL to scatter incoherently | "Rb_liq_inc.sqw" |
| tmin | s | Minimum ToF measured | 0.01806 |
| tmax | s | Maximum ToF measured | 0.01826 |

## Links

- [Source code](HZB_NEAT.instr) for `HZB_NEAT.instr`.
- <a href="http://www.helmholtz-berlin.de/user/neutrons/instrumentation/neutron-instruments/v3/index_en.html">NEAT at HZB/BENSC</a>

---
