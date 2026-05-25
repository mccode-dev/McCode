# The `SEMSANS_Delft` Instrument

*McStas: Idealised SESANS instrument using magnetised foil flippers with realistic sample*

## Identification

- **Site:** TUDelft
- **Author:** Wim Bouwman (w.g.bouwman@tudelft.nl), Peter Willendrup and Erik Knudsen
- **Origin:** Delft University of Technology
- **Date:** December 2019

## Description

```text
A full description of the instrument can be found in J. Appl. Cryst. (2021) V. 54 https://doi.org/10.1107/S1600576720015496
The instrument has the bare essentials to simulate the Larmor precession of the neutron spins in the foil flippers and the
geometry and wavelength distribution of a version of SEMSANS that can be made from the SESANS in Delft as described in
Physica B 406(2011)2357–2360 https://doi.org/10.1016/j.physb.2010.11.069
An essential component is the realistic SANS sample, describing both the scattered and transmitted neutron, including multiple scattering.
For a full SEMSANS scan the applied magnetic field By has to be scanned.
It is best to run it twice with the analyser in up and down orientation.
In principle the sample can be removed to study the empty beam instrument, which with this idealised setup have perfect polarisation.
It is possible to monitor the evolution of the polarisation with several polarisation monitors that have beeen commented in this version.
The foil-angle, thickness and wavelength yield now matched parameters to have the SESANS working.
These parameter and the sphere radius match now with the magnetic fields below to get a good measurement range.
A pretty high number of neutrons is needed to get good statistics in the fine position sensitive detector.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L0 | AA | nominal wavelength | 2.165 |
| DL | AA | wavelength band width | 0.02 |
| By | T | Applied magnetic field in foil flippers | 4.68e-3 |
| AnaSign |  | Direction of analyser (-1 or 1) | 1 |

## Links

- [Source code](SEMSANS_Delft.instr) for `SEMSANS_Delft.instr`.
- <reference/HTML link>
- <a href="https://doi.org/10.1107/S1600576720015496">https://doi.org/10.1107/S1600576720015496</a>

---
