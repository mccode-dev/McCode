# The `SOLEIL_ANATOMIX` Instrument

*McXtrace: The ANATOMIX tomography beam-line at Synchrotron SOLEIL.*

## Identification

- **Site:** SOLEIL
- **Author:** J. Perrin and E. Farhi
- **Origin:** Synchrotron SOLEIL
- **Date:** 2022-03-23

## Description

```text
The beamline ANATOMIX (Advanced Nanotomography and Imaging with coherent X rays)
works at photon energies between 5 and 50 keV. It is dedicated to full-field
radiography and tomography in absorption and phase contrast, with pixel sizes
from 20 nm to 20 µm.

ANATOMIX is a beamline for X-ray tomography on the micro- and nanoscale, in
absorption and phase contrast. It operates in the energy range from 5 keV upward
and allows its users to obtain two- and three-dimensional radiographic images of
bulk volume samples of macroscopic size (up to several cm thickness). For
smaller samples, a spatial resolution down to 50 nm (20 nm pixel size) can be
achieved. Real-time studies are possible at speeds of currently up to one
microtomography scan per second; higher speeds up to 20 volume scans per second
(50 ms per scan) have been demonstrated.

A flexible sample interface enables in situ and/or operando studies under
conditions similar to the natural or working environment of the samples
(temperature, humidity, mechanical load, transport processes). Biological
samples can be measured without dehydration and, in many cases, without chemical
fixation. With suitable sample preparation, cellular imaging without cryogenic
environment is possible.

This is a simplified model with an Undulator, a double channel-cut monochromator,
a sample stage and a detector. It models a parallel beam full field tomograph.
The monochromator is an Si(111), the sample is made of Ge. The sample is 2 cm
wide.

List of important beamline elements. Distances are given from the center of the undulator.

Element | Distance (m) | Description  |Hutch
--------|--------------|--------------|-------------------
Diaphragm       | 22.7 | Rectangular aperture | 2.5 × 2 mm2 (h×v). | OH1
Vertical slit   | 23.2 | Horizontal aperture 100. . .300 μm, removable. Increases horizontal transverse coherence where needed. |
Primary slits   | 25.9 | Define footprint on mirror and other optics. |
Attenuators     | 34.6 | Protect transfocator and other downstream elements. | OH3
Mirror M1       | 35.3 | Horiz. deflection. Useful length 400 mm. Bender for horiz. focusing (focal length 3.5 m). Coatings: B4C, Rh, Pt. Grazing angle ≈ 3 mrad. |
Mirror M2       | 36.3 | Horiz. deflection. Plane mirror, steers beam back into direction parallel to direct beam, offset 5 mm. Same length, angle and coatings as M1. |
Transfocator    | 37.7 | Beryllium lenses. |
Vertical slit   | 38.8 | Defines horizontal secondary source when mirror is used. |
Secondary slits | 48.4 | Define footprint on monochromators. | OH4
Attenuators     | 48.8 | Filter out low energies, especially when using DMM. |
DMM             | 49.8 | Under design. Vertical deflection, beam offset 20 mm. Tentative specs: two strips Ru/B4C, Ir/B4C, d = 2.4 nm. 400-mm substrates. |
DCM             | 52.7 | Vertical deflection, beam offset 20 mm. Si(111), LN2-cooled. |
EH3             | 165.7-174.1 | Guard slits; TXM sample stage and zone-plate table;  microtomography sample stage; detector table | EH3
EH4             | 197.2-208.5 | Guard slits; microtomography sample stage; detector table; X-ray grating interferometer | EH4
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Energy selected at the Undulator. | 11 |
| Emono | keV | Energy selected at the monochromator. When 0, it is set to E0. | 0 |
| dE | keV | Energy spread at the Undulator. | 1 |
| ANGLE | deg | Rotation angle of the sample stage. | 0 |
| sample | str | Sample geometry file, OFF/PLY format. | "wire.ply" |

## Links

- [Source code](SOLEIL_ANATOMIX.instr) for `SOLEIL_ANATOMIX.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/anatomix
- T Weitkamp et al 2017 J. Phys.: Conf. Ser. 849 012037 DOI: 10.1088/1742-6596/849/1/012037

---
