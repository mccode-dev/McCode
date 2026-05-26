# The `ILL_IN4` Instrument

*McStas: The IN4 thermal Time-of-Flight spectrometer at the ILL (H12 tube).*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** March 5th 2015.

## Description

```text
IN4C is a high-flux time-of-flight spectrometer used for the study of excitations
in condensed matter. It works in the thermal neutron energy range (10-100 meV).

Primary spectrometer

The main components of the beam conditioning part are the two background
choppers, the double curvature mono-chromator with four faces and the Fermi
chopper. The background choppers are rapidly pulsating beam shutters which act
as a low-pass filter. Thus they eliminate from the beam most of the fast
neutrons and gamma rays that would give background noise in the spectra. The
modular shielding encloses the background choppers in separate compartments in
order to cut off these undesired neutrons as early as possible. A suitable
energy is selected from the thermal neutron spectrum with the crystal
monochromator. The monochromator, an assembly of 55 crystal pieces,
concentrates the divergent incident beam onto a small area at the sample
position. The full use of the available solid angle gives a high incident
flux. The vertical curvature is fixed, and the horizontal
variable curvature of the monochromator is essential in controlling
the time and space focussing conditions for optimal performance (see H. Mutka,
Nucl. Instr. and Meth. A 338 (1994) 144). The Fermi chopper rotates at speeds
of up to 40000 rpm. It transmits short neutron pulses (10 ... 50 µs) to the
sample. The time-of-flight of neutrons between the chopper and the sample (1
... 5 ms) can be measured by using precise electronic circuitry.
A sapphire (Al2O3) filter can be inserted in the beam to remove the fast neutrons
background.

Monochromators:
PG       002 DM=3.355 AA (Highly Oriented Pyrolythic Graphite)
PG       004 DM=1.677 AA (used for lambda=1.1)
PG       006 DM=1.118 AA
Cu       220 DM=1.278 AA
Cu       111 DM=2.095 AA
Take-off:       39-65 deg
flux at sample: 5e5 n/s/cm2 (at 1.1 Angs)

Secondary spectrometer

The sample environment is designed to accommodate standard
cryostats and furnaces. A radial collimator around the sample position is used
to cut the scattering from the sample environment. The secondary flight-path
is in vacuum to avoid parasitic scattering of the transmitted neutrons. The
detector bank covers scattering angles of up to 120°. In addition to the 3He
detector tubes (length 300 mm, width 30 mm, elliptical section, pressure 6
bar) a 3He filled multidetector (eight sectors with 12 radial cells each;
outer diameter Phi 60 cm) will allow us to observe forward scattering. The
time-of-flight spectra measured at various angles are further treated in order
to obtain the scattering function S(Q,w) using e.g. LAMP.

In this model, the sample is a cylindrical liquid/powder/glass scatterer
surrounded by a container and an Al cryostat.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | wavelength | 2.2 |
| dlambda | Angs | wavelength HALF spread at source | 0.1 |
| DM | Angs | monochromator d-spacing | 3.355 |
| ETAM | arcmin | monochromator mosaic FWHM | 35 |
| RMH | m | Monochromator horizontal curvature. Use -1 for auto. | -1 |
| ratio | 1 | Disk Chopper ratio (nu=nu_FC/ratio) | 4 |
| dE | meV | Inelastic energy for time focusing | 0 |
| Sapphire_present | 1 | Flag, when 1 the Al2O3 filter is active | 1 |
| sample_coh | str | Sample coherent dynamic structure factor (.sqw) or NULL | "Dirac2D.sqw" |
| sample_inc | str | Sample incoherent dynamic structure factor (.sqw) or NULL | "NULL" |
| order | 1 | The number of multiple orders at the monochromator | 1 |

## Links

- [Source code](ILL_IN4.instr) for `ILL_IN4.instr`.
- H. Mutka, Nucl. Instr. and Meth. A 338 (1994) 144
- http://www.ill.eu/fr/instruments-support/instruments-groups/instruments/in4c

---
