# The `ILL_BRISP` Instrument

*McStas: Time of Flight Neutron Spectrometer for Small Angle Inelastic Scattering BRISP*

## Identification

- **Site:** ILL
- **Author:** E. Farhi and N. Formissano [formisan@ill.fr]
- **Origin:** ILL
- **Date:** June 4th, 2009

## Description

```text
BRISP is a new concept thermal neutron Brillouin scattering spectrometer which
exploits the time-of-flight technique and is optimized to operate at small
scattering angles with good energy resolution.

Keywords in the design of the BRISP spectrometer were :

Thermal neutron energies: allowing for investigations in systems characterized
by sound velocities up to 3000 m/s (three different incident energies between
20 and 80 meV are presently available).
Easy small-angle access: enabling low-Q spectroscopy with thermal neutrons.
Elastic wavevector transfer values Qel as low as 0.03 Å -1 at 20 meV incident
energy can be reached. The position of the two-dimensional detector can be
adjusted to cover different small-angle ranges between 1° and 15°.
Time-of-Flight technique: for an efficient data collection allowing also for
accurate neutron measurements as a function of external parameters such as
temperature, pressure and magnetic field.
Carefull optimization of monochromator-collimators-Fermi chopper:  leading to
0.5 meV energy resolution and 0.02 Å-1 Q resolution in a typical
configuration (20 meV incident energy and 4 m sample-detector distance),
along with acceptable counting rates (flux at the sample 104 n s-1 cm-2).
For this purpose, innovatory solutions were specially developed for some of
the BRISP components.

Main components:

a Soller collimator defining the beam impinging on the monochromator, with a
collimation angle of 0.4°
two focusing multi-crystal monochromators, PG and Cu(111), that allow for the
selection of three incident energies in the range from 20 to 80 meV.
Fixed/variable curvatures are adopted in/outside the Brisp vertical scattering plane.
a disk chopper used for background reduction and selection of the desired
monochromator reflection through proper phasing with the Fermi chopper.
three honeycomb converging collimators [1] to define the incident beam on the
sample with a collimation angle of 0.4°, and to optimize convergence at three
detector positions (2, 4, 6 m from the sample). A coarse resolution option
is also available, without honeycomb collimator.
a Fermi chopper producing short neutron pulses which enable the time-of-flight
analysis.
a high-vacuum sample chamber possibly equipped with 1.5-300 K MAXI Orange
cryostat (100 mm) and 300-1900 K furnace
a ~2 m2-area position sensitive gas detector (3He) whose distance from the
sample can be varied between 2 and 6 m in order to access the required Q-range.
A huge vacuum tank hosts the detector. An elastobore – polyethylene shielding
surrounds the vacuum tank to reduce the environmental background.
the long vacuum line ensures an under-vacuum neutron flight path from the
background chopper to the detector.

Configurations:
crystal d-spacing (Å)   lambda0 (Å)    E0(meV)
PG(002) 3.355(nominal)  1.977(expt.)   20.9 (expt.)
Cu(111) 2.087           1.28  (expt.)  49.9 (expt.)
PG(004) 1.677(nominal)  0.989(expt.)   83.6 (expt.)

In this model, the sample is a plate of thickness e=4 mm, surrounded by an
Al or Nb container, inside an Al shield (phi=10 cm).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DM | Angs | Monochromator d-spacing. Use 3.355 for PG002, 1.677 for PG004 and 2.087 for Cu111. | 3.355 |
| coh | str | Sample coherent specification (use laz, lau or Sqw file, or NULL to disable). Sample is a 5x5 cm plate, e=4 mm. | "V.lau" |
| inc | str | Sample incoherent specification (use laz, lau or Sqw file, or NULL to scatter isotropically, using cross sections read from the coherent file) | "NULL" |
| container | str | sample container material. Thickness is .2 mm. Use NULL, Al or Nb. | "NULL" |
| LSD | m | Distance sample-detector | 4.5 |
| Frequency | Hz | Disk-chopper frequency | 0 |
| LCC | m | Distance between the Disk-chopper and the Fermi-chopper | 0 |
| DELAY | s | Fermi phase wrt. Disk-chopper opening | 0 |

## Links

- [Source code](ILL_BRISP.instr) for `ILL_BRISP.instr`.
- The BRISP spectrometer at the ILL <a href="www.ill.fr/brisp">BRISP</a>

---
