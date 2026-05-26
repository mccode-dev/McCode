# The `SAFARI_PITSI` Instrument

*McStas: Powder Instrument for Transition in Structure Investigations*

## Identification

- **Site:** Necsa
- **Author:** Deon Marais (deon.marais@necsa.co.za)
- **Origin:** Necsa
- **Date:** September 2013

## Description

```text
Necsa Neutron Powder Diffractometer located at beam port 5 of the SAFARI-1 research reactor, South Africa
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| source_lam_min | Angs | Minimum wavelenth of source | 0.5 |
| source_lam_max | Angs | Maximum wavelenth of source | 2.0 |
| hi_res |  | Selects hi-resolution(1) or hi-intensity(0) reactor beam through primary shutter | 0 |
| mono_Si_type |  | Monochromator Silicon type options: 422 400 311 511 111 331 (Mostly used: 331 and 551) | 551 |
| mono_mosh | arc min | Monochromator horizontal mosaicity | 30 |
| mono_mosv | arc min | Monochromator vertical mosaicity | 30 |
| mono_dx | m | Monochromator delta x - positive to left of reactor beam | 0 |
| mono_dy | m | Monochromator delta y - positive upward of reactor beam | 0 |
| mono_dz | m | Monochromator delta z - positive along reactor beam | 0 |
| mono_takeoff | deg | Monochromator takeoff angle - positive anti-clockwise from reactor beam | 90.0 |
| mono_dtilt | deg | Monochromator tilt angle - not implemented yet | 0 |
| mono_r_h | m | Monochromator horizontal focus radius (min 5.0 m) | 5.0 |
| mono_r_v | m | Monochromator vertical focus radius (min 0.9 m) | 3.572 |
| port_takeoff | deg | Port takeoff angle - positive anti clockwise from reactor beam (70 or 90 deg) | 90.0 |
| inc_slit_rot | deg | Incident slit delta rotation - not implemented yet | 0 |
| inc_slit_dx | m | Incident slit delta x position - positive to left of incident beam | 0 |
| inc_slit_to_cor | m | Incident slit to sample stage center of rotation | 0.01 |
| inc_slit_width | m | Incident slit width 0.00013m to 0.006m | 0.006 |
| inc_slit_height | m | Incident slit height 0m to 0.05m | 0.05 |
| inc_slit_sep |  | Incident slit separation between width and height. <0:use emperical calc, >=0:distance in m | 0 |
| mono_to_cor | m | Distance between monochromator and center of rotation | 2.5 |
| sample_dx | m | Sample delta x - positive to left of incident beam if sample_dom=0 | 0 |
| sample_dy | m | Sample delta y - positive upword of incident beam | 0 |
| sample_dz | m | Sample delta x - positive along of incident beam if sample_dom=0 | 0 |
| sample_dom | deg | Sample delta omega - positive anti-clockwise from incident beam | 0 |
| det_takeoff | deg | Detector takeoff angle - positive anti-clockwise from incident beam | -114.375 |
| cor_to_det | m | Distance between sample centre of rotation and detector | 1.179 |
| dangle_interest | deg | Delta angle of interenterest. | 125 |
| full_instrument |  |  | 1 |

## Links

- [Source code](SAFARI_PITSI.instr) for `SAFARI_PITSI.instr`.
- The South African Nuclear Energy Corporation <a href="http://www.necsa.co.za"> website</a>

---
