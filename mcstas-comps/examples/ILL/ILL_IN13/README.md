# The `ILL_IN13` Instrument

*McStas: IN13 Thermal neutron backscattering spectrometer (without guide)*

## Identification

- **Site:** ILL
- **Author:** E. Farhi and A. Dennison
- **Origin:** ILL
- **Date:** March 25, 2015

## Description

```text
IN13 Thermal neutron backscattering spectrometer (without guide)

Because of its high energy resolution and high momentum transfer the
backscattering spectrometer IN13 is particularly useful for the microscopic
study of single particle motions (jump reorientation, rotational and
translational diffusion, tunnelling) observed by incoherent neutron scattering.

The high energy resolution of the order of a few μeV together with the
availability of high momentum transfer (Q<4.9 Å-1) makes the spectrometer IN13
particularly useful for the microscopic study of single particle motions (jump
reorientation, rotational and translational diffusion, tunnelling) observed by
incoherent neutron scattering. The instrument partly fills the gap of (Q, ω)
space between IN10 and IN5.

Temperature gradient monochromator
The monochromator and analyser CaF2(422) crystals are oriented in near
backscattering geometry thereby achieving an energy resolution of a few μeV. The
energy of the incident neutrons is scanned by variation of the temperature of
the monochromator at a fixed Bragg-angle. In an optional mode the 10 mm thick
monochromator crystals are kept at a fixed temperature gradient and energy
variation is performed by scanning the monochromator Bragg-angle. This achieves
an increased flux at the sample position and slightly increases the energy
resolution width.

A vertically curved Graphite deflector focusses the beam onto
the sample. The scattered neutrons are energy analysed by a set of seven
spherically curved composite crystal analysers, each covering a large solid
angle of 0.18 sr. An additional three circular analysers centred around the
transmitted beam cover the small-angle region.

The neutron time-of-flight is used to suppress (i) the background of neutrons
scattered directly from the sample into the detectors and (ii) second order
contamination. The neutrons are counted with a cylindrical multidetector
consisting of 35 3He detector tubes, arranged in staggered circular rows. The
small Q range from 0.2 to 0.8 Å-1 is covered by a 3He Position Sensitive
Detector (PSD) arranged to see the circular analysers in exact backscattering.

Monochromator: CaF2(422)
temperature range:  -125 < ΔE/µeV < 150
angular range:       81° < θM < 89°
incident energy:     16.45 meV (TM≥25°C)
incident wavelength: 2.23 Å
energy resolution:   8 µeV

Deflector: PG(002)

Sample: here incoherent elastic scatterer (Vanadium)
sample size          3.5 x 3.5 cm2
flux at sample       2e4 n cm-2 s-1 (when fed from H24, NOT in this model)

Analyser: CaF2(422)
Q-range:             0.2 < Q/Å-1 < 4.9
Q-resolution:        ΔQ/Å-1 < 0.1
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| RMV | m | monochromator vertical curvature. Use 0=flat, -1=auto. | 0 |
| RDV | m | deflector vertical curvature. Use 0=flat, -1=auto. | 4.6 |
| RDH | m | deflector horizontal curvature. Use 0=flat, -1=auto. | 0 |
| TM | K | monochromator temperature, from 77 to 500 K | 301 |
| LMD | m | monochromator-deflector distance | 1.8 |
| mos_ana | arcmin | analyser mosaic | 2 |
| CaF2mos | arcmin | monochromator mosaic | 10 |
| gW |  |  | 0.030 |

## Links

- [Source code](ILL_IN13.instr) for `ILL_IN13.instr`.
- The <a href="http://www.ill.fr/YellowBook/IN3">IN3 TAS</a> at the ILL

---
