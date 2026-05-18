# The `PBD_BT` Instrument

*McXtrace: Mockup of NIST 219/B004 PBD*

## Identification

- **Site:** NIST
- **Author:** Marcus H. Mendenhall (marcus.mendenhall@nist.gov)
- **Origin:** Your institution
- **Date:** Current Date

## Description

```text
This is a preliminary setup of the PBD, using single-bounce crystals instead of 3-bounce crystals
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| source_y | m | Vertical offset of the source exit-widow. | 0.0 |
| source_rotation | deg | Rotation around the X-axis of the source. | 0.0 |
| beam_vert | m | Height of beam exit.window. | 0.002 |
| beam_hor | m | Width of beam exit-window. | 0.001 |
| mirror_length | m | Length of the focusing mirror | 0.05 |
| mirror_full_deflection_angle | deg | Deflection angle of mirror | 2.32 |
| mirror_center_distance | m | Distance from | 0.1 |
| mirror_focal_error | m | Additional shift on the mirror focal distance | 0.0 |
| mirror_rotation_angle | deg | Mirror tilt angle | -2.32 |
| mirror_y_offset | m |  | 0.0 |
| use_mirror |  | Flag to set whether to use the mirror or no. | 1 |
| mono_rotation | deg | Rotation angle of the monochromator crystal. | 0.0 |
| use_mono |  | Flag enabling/disabling the monochromator. | 1 |
| mono_detuning_seconds | arcsec | Detuning angle for the monochromator crystal(s). | 34 |
| mono_misalignment_angle_arcsec | arcsec | Mislignment of the monochromator. | 0 |
| mono_incident_angle_deg | deg | Nominal angle of incidence for beam vs. monochromator. | 3.0 |
| slit_1_vert | m | 1st aperture vertical. | 0.001 |
| slit_1_hor | m | 1st aperture horizontal. | 0.0015 |
| slit_2_vert | m | 2nd aperture vertical. | 0.005 |
| slit_2_hor | m | 2nd aperture horizontal. | 0.002 |
| emin | keV | Minimal energy to sample. | 8.046 |
| emax | keV | Maximal energy to sample. | 8.050 |
| omega | deg | Sample roation around the Y-axis. | 106.715 |
| theta | deg | Sample rotation around the X-axis. | -160.0725 |
| crystal_central_angle | deg | sample_crystal_angle. | 53.355 |
| hh |  | 1st Miller index of the sample crystal reflection under study. | 4 |
| kk |  | 2nd Miller index of the sample crystal reflection under study. | 4 |
| ll |  | 3rd  Miller index of the sample crystal reflection under study. | 0 |
| use_flat_source |  | Use a flat source instead of Lab_source, for debugging. | 0 |
| stop_at_energy |  | Flag | 0 |
| use_3_bounce |  | Use a 3-bounce set up instead of 1-bounce. | 1 |
| run_setup | string | Which setup to use? | "nosetup" |
| polarization | string | Should polarization be taken into effect. | "0" |

## Links

- [Source code](PBD_BT.instr) for `PBD_BT.instr`.
- <a href="https://www.nist.gov/programs-projects/diffraction-metrology-and-standards/parallel-beam-diffractometer-pbd-laboratory"></a>

---
