# The `Supermirror_thin_substrate` Instrument

*McStas: Test instrument for a thin-substrate supermirrror (component SupermirrorFlat)*

## Identification

- **Site:** Tests_polarization
- **Author:** Hal Lee
- **Origin:** ESS
- **Date:** 2024

## Description

```text
Test instrument for a thin-substrate supermirrror (component SupermirrorFlat)
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| src_x | m | Width of source | 0.01 |
| src_y | m | Height of source | 0.09 |
| W_Centre | AA | Central wavelength from source spectrum | 5 |
| W_Half_Width | AA | Source spectrum wavelength half-width | 3 |
| A_FWHM | deg | Angular divergence from source | 0.05 |
| Detector_Distance | m | End-of-supermirror to detector distance | 1 |
| Length | m | Supermirrror length,projection along z-axis | 4 |
| Thickness_In_mm | mm | Supermirror thickness in mm | 0.5 |
| Mirror_Coated_Side | str | Specification of coating type | "BothCoated" |
| Mirror_Plus | str | Specification of coating on positive mirror side | "FeSiPlus" |
| Mirror_Plus_m | 1 | Specification m value, positive mirror side | 3.5 |
| Mirror_Minus | str | Specification of coating on negative mirror side | "FeSiMinus" |
| Absorber_Coated_Side | str | Specification of coating type, absorber coated side | "BothNotCoated" |
| Absorber | str | Specification of absorber material | "Empty" |
| Absorber_Thickness_In_Micron | mu-m | Absorber thickness in mu-m | 0 |
| Substrate | str | Specification of substrate material | "GlassNoAttenuation" |
| Initial_Placement_At_Origin | str | Mirror orientation specifier ("TopFrontEdgeCentre","FrontSubstrateCentre","BottomFrontEdgeCentre") | "TopFrontEdgeCentre" |
| Tilt_Axis_Location | str | Mirror axis location specifier ("TopFrontEdge","TopMirrorCentre","TopBackEdge","FrontSubstrateCentre","SubstrateCentre","BackSubstrateCentre","BottomFrontEdge","BottomMirrorCentre","BottomBackEdge") | "TopMirrorCentre" |
| Tilt_Angle_First_In_Degree | deg | Mirror tilt angle (around global y) | 0.64 |
| Translation_Second_Y | m | Mirror translation (along global y) | 0 |
| Rot_Angle_Third_In_Degree | deg | Mirror rotation angle (around global z) | 0 |
| Tracking | str | Mirror event-tracking option specifier ("NoTracking", "DetailTracking") | "DetailTracking" |

## Links

- [Source code](Supermirror_thin_substrate.instr) for `Supermirror_thin_substrate.instr`.
- A reference/HTML link for more information

---
