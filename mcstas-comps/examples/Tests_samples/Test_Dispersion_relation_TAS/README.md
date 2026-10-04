# The `Test_Dispersion_relation_TAS` Instrument

*McStas: Triple axis test instrument for the Dispersion_relation component*

## Identification

- **Site:** 
- **Author:** Daniel Lomholt Christensen
- **Origin:** Niels Bohr Institute
- **Date:** September 2026

## Description

```text
A simple triple axis spectrometer with a flat PG(002) monochromator and
analyzer, operated at fixed final energy. For a given (h, 0, l) and energy
transfer dE the instrument calculates the monochromator, sample and analyzer
angles, so scans over h, l and dE map the dispersion of the sample, for
example a constant energy cut in (h, l) or a dispersion curve in (h, dE).

The samples take the scattering vector as Q = k_i - k_f in the sample frame,
with h along the sample x axis, k along y and l along z, so the horizontal
scattering plane holds h and l. The sample rotation A3 turns (h, 0, l) into
the scattering vector set up by the scattering angle A4.

sample_choice selects the sample. The phonon samples (0, 1) share the Pb
phonon of Phonon_simple, and the magnon samples (2, 3) the MnF2 magnon of
SpinWave_BCO:
0: Dispersion_relation reading the phonon files written by
generate_phonon_dispersion.py
1: Phonon_simple, the phonon reference
2: SpinWave_BCO, the magnon reference. Its shared functions are prefixed
with disp_SBCO_ so it can be in the same instrument as Phonon_simple
3: Dispersion_relation reading the magnon files written by
generate_magnon_dispersion.py
All samples get the same geometry, absorption, incoherent scattering,
Debye-Waller factor and temperature.

For the phonon, h and l are in units of 4 pi/a of Pb (a = 4.95 AA): the
dispersion files are written on the cube of side a/2, over which the fcc
dispersion repeats, and the files hold the cross section for the zone around
(1, 0, 1). With primitive_cell = 1, Dispersion_relation instead gets the
primitive fcc lattice vectors, a non-orthogonal cell with 60 degree angles,
and reads the files written with --cell primitive; h and l keep their units.
For the magnon, h, k, l are the MnF2 indices (a = b = 4.873 AA, c = 3.130 AA,
along x, y, z for both samples), and (h, l) = (1, 0) is the magnetic zone
centre MnF2 (1 0 0).

Example: mcrun Test_Dispersion_relation_TAS.instr -M -N 21,21 -n 2e5 h=0.6,1.4 l=0.6,1.4 dE=5 sample_choice=0

The dispersion files are written by the SHELL command below when the
instrument is compiled, if they do not exist yet, by
generate_dispersion_files.py (Python with numpy).
```

## Examples

- **Test: h=1.2 l=1 dE=5 sample_choice=0 Detector: detector_I=1.46e-11**
- **Test: h=1.2 l=1 dE=5 sample_choice=1 Detector: detector_I=1.41e-11**
- **Test: h=1.2 l=1 dE=5 sample_choice=0 primitive_cell=1 Detector: detector_I=1.46e-11**
- **Test: h=1.15 l=0 dE=3 T=2 sample_choice=2 Detector: detector_I=8.91e-10**
- **Test: h=1.15 l=0 dE=3 T=2 sample_choice=3 Detector: detector_I=9.41e-10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| h | r.l.u. | Scattering vector component along a* (sample x axis) | 1 |
| l | r.l.u. | Scattering vector component along c* (sample z axis) | 1 |
| dE | meV | Energy transfer to the sample, positive for neutron energy loss | 5 |
| Ef | meV | Fixed final energy selected by the analyzer | 14.7 |
| T | K | Sample temperature | 300 |
| sigma_abs | barns | Absorption cross section of the samples | 0 |
| sigma_inc | barns | Incoherent scattering cross section of the samples | 0 |
| sample_choice | 1 | 0: Dispersion_relation (phonon), 1: Phonon_simple, 2: SpinWave_BCO, 3: Dispersion_relation (magnon) | 0 |
| primitive_cell | 1 | setting this to 1 gives the phonon Dispersion_relation sampling the primitive fcc lattice vectors | 0 |

## Links

- [Source code](Test_Dispersion_relation_TAS.instr) for `Test_Dispersion_relation_TAS.instr`.
- [Additional information](Test_Dispersion_relation_TAS.md)

---
