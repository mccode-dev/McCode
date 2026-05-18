# The `McStas_PowderN` Instrument

*McStas: Wrapper instrument for use of PowderN in SIMRES*

## Identification

- **Site:** SINE2020
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** December 2018

## Description

```text
This instrument provides an MCPL-based interface for the use of PowderN in the SIMRES package.
The instrument has been developed in the context of WP8 in the SINE2020 project and is part of
deliverable D8.8.

(EU Horizon 2020 research and innovation programme under grant agreement No 654000).

The default material is Al as a cylinder of radius 0.01 m x height 0.07 m

Example: McStas_PowderN reflections=Al.lau radius=0.01 yheight=0.07
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| reflections | string | Input file for reflections, laz and lau formats from McStas accepted | "Al.lau" |
| geometry | string | Name of an Object File  Format  (OFF)  or  PLY file for complex geometry. | "" |
| radius | m | Sample radius | 0.01 |
| xwidth | m | Sample width along x | 0 |
| yheight | m | Sample height along y | 0.07 |
| zdepth | m | Sample depth along z | 0 |
| thickness | m | Thickness of hollow sample | 0 |
| pack | m | Packing factor. | 1 |
| d_omega | deg | Horizontal (incoherent only) focusing limit [deg] | 0 |
| d_phi | deg | Vertical focusing limit [deg] | 0 |
| focus_flip | 1 | Controls the sense of d_phi. If 0 d_phi is measured against the xz-plane. If nonzero, d_phi is measured against zy-plane. | 0 |
| tth_sign | 1 | Sign of the scattering angle. If 0, the sign is chosen randomly | 0 |
| barns | 1 | Flag to indicate if \|F 2\| from "reflections" is in barns or fm2 (barns = 1 for laz, barns = 0 for lau type files). | 0 |
| rot_x | deg | Sample rotation around x | 0 |
| rot_y | deg | Sample rotation around y | 0 |
| rot_z | deg | Sample rotation around z | 0 |

## Links

- [Source code](McStas_PowderN.instr) for `McStas_PowderN.instr`.
- Website for the <a href="https://mctools.github.io/mcpl">MCPL</a> particle exchange format
- Website for the <a href="http://neutron.ujf.cas.cz/ restrax/">SIMRES</a> package
- Website for <a href="https://www.sine2020.eu/randd-activities/e-tools.html">WP8 in EU-SINE2020</a>

---
