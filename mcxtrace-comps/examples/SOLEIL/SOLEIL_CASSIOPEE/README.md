# The `SOLEIL_CASSIOPEE` Instrument

*McXtrace: CASSIOPEE beamline at SOLEIL*

## Identification

- **Site:** SOLEIL
- **Author:** François Bertran
- **Origin:** SOLEIL
- **Date:** 11/03/2022

## Description

```text
CASSIOPEE : Combined Angle- and Spin-resolved SpectroscopIes Of PhotoEmitted Electrons

The CASSIOPEE beamline is dedicated to photoemission experiments in the 8 eV -
1500 eV photon energy range. The beamline uses two undulators, and the main
optical elements are the entrance optics and the monochromator. After the
monochromator, the beamline is divided into two branches, supplying photons to
two endstations (Spin-resolved Photoemission, and High Resolution Angle-resolved
Photoemission), both connected to a Molecular Beam Epitaxy chamber.

Position | Element
---------|--------------------------------------------------------------------
0        | Undulator HU60 / HU256
21.83    | Plane mirror M1a
22.18    | Spherical mirror M1b
28.83    | Plane grating (reflection)
29.02    | Plane mirror M2
30.83    | Slit
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| mirror_material | str | Mirror M1 coating (e.g. Pt.dat) | "Pt.dat" |
| undulator_index | 1-2 | Undulator 1=HU60; 2=HU256 | 1 |
| M1_angle | deg | M1 mirror angle. 0=auto | 0 |
| PG_angle | deg | Reflective grating monochromator. 0=auto | 0 |
| PM_angle | deg | M2 mirror angle. 0=auto | 0 |
| E0 | keV | Undulator emission energy, e.g. 40e-3 or 300e-3 keV. 0=auto | 0 |
| dE | keV | Undulator energy bandwidth, e.g. 1e-3. | 1e-3 |
| lambda | Ang | Undulator emission wavelength. Used when E0=0. | 0 |
| beta_mono | deg | Monochromator angle. 0=auto | 0 |
| r_rho | /mm | Grating number of lines/mm. 0=auto. | 0 |

## Links

- [Source code](SOLEIL_CASSIOPEE.instr) for `SOLEIL_CASSIOPEE.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/cassiopee
- https://www.researchgate.net/deref/https%3A%2F%2Ftel.archives-ouvertes.fr%2Ftel-01064523

---
