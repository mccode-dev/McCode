# The `SAFARI_MPISI` Instrument

*McStas: Materials Probe for Internal Strain Investigations*

## Identification

- **Site:** Necsa
- **Author:** Deon Marais (deon.marais@necsa.co.za)
- **Origin:** Necsa
- **Date:** September 2013

## Description

```text
Necsa Neutron Strain Scanner located at beam port 5 of the SAFARI-1 research reactor, South Africa
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| source_lam_min | Angs | Minimum wavelenth of source | 0.5 |
| source_lam_max | Angs | Maximum wavelenth of source | 2.0 |
| hi_res |  | Selects hi-resolution(1) or hi-intensity(0) reactor beam through primary shutter | 0 |
| mono_Si_type |  | Monochromator Silicon type options: 422 400 311 511 111 331 | 311 |
| mono_mosh | arc min | Monochromator horizontal mosaicity | 30 |
| mono_mosv | arc min | Monochromator vertical mosaicity | 30 |
| mono_dx | m | Monochromator delta x - positive to left of reactor beam | 0 |
| mono_dy | m | Monochromator delta y - positive upward of reactor beam | 0 |
| mono_dz | m | Monochromator delta z - positive along reactor beam | 0 |
| mono_takeoff | deg | Monochromator takeoff angle - positive anti-clockwise from reactor beam | -83.5 |
| mono_dtilt | deg | Monochromator tilt angle - not implemented yet | 0 |
| mono_Rh | m | Monochromator horizontal focus radius | 3.572 |
| port_takeoff | deg | Port takeoff angle - positive anti clockwise from reactor beam | -83.5 |
| chamber_window_rad | m | Chamber window radius. If this is 0, there is no window | 0.05534 |
| inc_slit_rot | deg | Incident slit delta rotation - not implemented yet | 0 |
| inc_slit_dx | m | Incident slit delta x position - positive to left of incident beam | 0 |
| inc_slit_to_cor | m | Incident slit to sample stage center of rotation | 0.005 |
| inc_slit_width | m | Incident slit width 0.00013m to 0.005m | 0.005 |
| inc_slit_height | m | Incident slit height 0m to 0.02m | 0.02 |
| inc_slit_sep |  | Incident slit separation between width and height. <0:use emperical calc, >=0:distance in m | -1 |
| mono_to_cor | m | Distance between monochromator and center of rotation | 2.5 |
| sample_dx | m | Sample delta x - positive to left of incident beam if sample_dom=0 | 0 |
| sample_dy | m | Sample delta y - positive upword of incident beam | 0 |
| sample_dz | m | Sample delta x - positive along of incident beam if sample_dom=0 | 0 |
| sample_dom | deg | Sample delta omega - positive anti-clockwise from incident beam | 0 |
| det_takeoff | deg | Detector takeoff angle - positive anti-clockwise from incident beam | 90 |
| cor_to_det | m | Distance between sample centre of rotation and detector | 1.148 |
| diff_slit_dx | m | Diffracted slit delta x position - positive to left of diffracted beam | 0 |
| diff_slit_to_cor | deg | Distance between centre of rotation and diffracted slit | 0.005 |
| diff_slit_width | m | Diffracted slit width | 0.005 |
| diff_slit_height | m | Diffracted slit height | 0.02 |
| full_instrument |  | When 1, simulates the complete instrument. When 0, only simulate from the outlet collimator | 1 |

## Links

- [Source code](SAFARI_MPISI.instr) for `SAFARI_MPISI.instr`.
- The South African Nuclear Energy Corporation <a href="http://www.necsa.co.za"> website</a>

---
