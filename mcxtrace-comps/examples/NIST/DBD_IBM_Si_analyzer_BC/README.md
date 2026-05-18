# The `DBD_IBM_Si_analyzer_BC` Instrument

*McXtrace: Mockup of 219/B002 DBD*

## Identification

- **Site:** NIST
- **Author:** Marcus H. Mendenhall (marcus.mendenhall@nist.gov)
- **Origin:** NIST
- **Date:** May '17

## Description

```text
This is a preliminary setup of the DBD with the Ge111 Johansson IBM
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| a_distance_error | m | Logitudinal offset of source. | 0.0 |
| figure_radius_multiplier |  | Extra multiplicative factor to the crystal curvature. | 1.0 |
| lattice_radius_multiplier |  | Extra factor to the lattice curvature. Currently IGNORED. | 1.0 |
| beam_hor | m | Beam width of source. | 0.01 |
| beam_vert | m | Unscaled Beam height of source. Will be scaled with take_off angle. | 0.00004 |
| tails_integral |  | Integrated beam power in the source tails. | 0.05 |
| tails_width | m | Width of the source tails. | 0.002 |
| slit_1_vert | m | Vertical opening of 1st slit. | 0.004 |
| slit_1_hor | m | Horizontal opening of 1st slit. | 0.01 |
| slit_2_vert | m | Vertical opening of 2nd slit. | 0.0002 |
| slit_2_hor | m | Horizontal opening of 2nd slit. | 0.02 |
| slit_3_hor | m | Horizontal opening of 3rd slit. | 0.02 |
| slit_3_vert | m | Vertical opening of 3rd slit. | 0.01 |
| ibm_offset | m | Offset (rotational) of optical arm. | 0.00 |
| two_theta_si | deg. | Monochromator crystal scattering angle. | 94.963 |
| emin | keV | Lower energy to be simulated. | 8.047 |
| emax | keV | i                   Upper energy to be simulated. | 8.0471 |
| take_off | deg | Take off angle of source. | 6.0 |
| use_flat_source |  | Use a soimplified flat source approximation. | 0 |
| use_si_crystal |  | Activate the monochromator crystal. | 1 |
| si_hh |  | 1st Miller index (h) of monochromator crystal. | 3 |
| si_kk |  | 2nd index (k) of monochromator crystal. | 3 |
| si_ll |  | 3rd Miller index (l) of monochromator crystal. | 3 |

## Links

- [Source code](DBD_IBM_Si_analyzer_BC.instr) for `DBD_IBM_Si_analyzer_BC.instr`.
- <a href="https://www.nist.gov/programs-projects/diffraction-metrology-and-standards/divergent-beam-diffractometer-dbd-laboratory"></a>
- <a href="https://tsapps.nist.gov/publication/get_pdf.cfm?<pub_id=927309">"International Tables for Crystallography (2018). Vol. H, Chapter 3.1, pp. 224–251."</a>

---
