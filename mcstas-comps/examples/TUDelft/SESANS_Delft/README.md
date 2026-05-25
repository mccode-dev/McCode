# The `SESANS_Delft` Instrument

*McStas: Idealised SESANS instrument using magnetised foil flippers with realistic sample*

## Identification

- **Site:** TUDelft
- **Author:** Wim Bouwman (w.g.bouwman@tudelft.nl), Peter Willendrup and Erik Knudsen
- **Origin:** Delft University of Technology
- **Date:** December 2019

## Description

```text
<instrument description>
A full description of the instrument can be found in J. Appl. Cryst. (2021) V. 54 https://doi.org/10.1107/S1600576720015496
The instrument has the bare essentials to simulate the Larmor precession of the neutron spins in the foil flippers and the
geometry and wavelength distribution of the SESANS in Delft as described in
Rev. Sci. Instrum. 76, 033901 (2005) https://doi.org/10.1063/1.1858579
An essential component is the realistic SANS sample, describing both the scattered and transmitted neutron, including multiple scattering.
In one scattering simulation both the up- and down intensities are simulated.
For a full SESANS scan the applied magnetic field By has to be scanned.
In principle the sample can be removed to study the empty beam instrument, which with this idealised setup have perfect polarisation.
It is possible to monitor the evolution of the polarisation with several polarisation monitors that have beeen commented in this version.
The foil-angle, thickness and wavelength yield now matched parameters to have the SESANS working.
These parameter and the sphere radius match now with the magnetic fields below to get a good measurement range.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L0 | AA | nominal wavelength | 2.165 |
| DL | AA | wavelength band width | 0.02 |
| By | T | Applied magnetic field in foil flippers | 4.68e-3 |

## Links

- [Source code](SESANS_Delft.instr) for `SESANS_Delft.instr`.
- <reference/HTML link>
- <a href="https://doi.org/10.1107/S1600576720015496">https://doi.org/10.1107/S1600576720015496</a>

---
