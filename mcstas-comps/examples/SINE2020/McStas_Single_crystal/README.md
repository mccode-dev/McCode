# The `McStas_Single_crystal` Instrument

*McStas: Wrapper for use of Single_crystal in SIMRES*

## Identification

- **Site:** SINE2020
- **Author:** Erik B Knudsen erkn@fysik.dtu.dk
- **Origin:** DTU Physics
- **Date:** Dec. 2018

## Description

```text
This instrument provides an MCPL-based interface for the use of Single_crystal in the SIMRES package.
The instrument has been developed in the context of WP8 in the SINE2020 project  and is part of
deliverable D8.8.

(EU Horizon 2020 research and innovation programme under grant agreement No 654000).

The default material is Al as a cylinder of radius 0.01 m x height 0.02 m

Example: McStas_Single_crystal reflections=Al.lau radius=0.01 yheight=0.02
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| reflections | string | Input file for reflections, laz and lau formats from McStas accepted | "Al.lau" |
| geometry | string | Name of an Object File  Format  (OFF)  or  PLY file for complex geometry. | "" |
| radius | m | Sample radius | 1e-2 |
| xwidth | m | Sample width along x | 0 |
| yheight | m | Sample height along y | 2e-2 |
| zdepth | m | Sample depth along z | 0 |
| delta_d_d | 1 | Lattice spacing variance, gaussian RMS | 1e-3 |
| mosaic | arc minutes | Crystal mosaic (isotropic), gaussian RMS. Puts the crystal in the isotropic mosaic model state, thus disregarding other mosaicity parameters. | 1 |
| mosaic_a | arc minutes | Horizontal (rotation around lattice vector a) mosaic (anisotropic), gaussian RMS. Put the crystal in the anisotropic crystal vector state. I.e. model mosaicity through rotation around the crystal lattice vectors. Has precedence over in-plane mosaic model. | 0 |
| mosaic_b | arc minutes | Vertical (rotation around lattice vector b) mosaic (anisotropic), gaussian RMS. | 0 |
| mosaic_c | arc minutes | Out-of-plane (Rotation around lattice vector c) mosaic (anisotropic), gaussian RMS | 0 |
| barns | 1 | Flag to indicate if \|F 2\| from "reflections" is in barns or fm2 (barns = 1 for laz, barns = 0 for lau type files). | 0 |
| order | 1 | Limit multiple scattering up to given order (0: all, 1: first, 2: second, ...) | 0 |
| rot_x | deg | Sample rotation around x | 0 |
| rot_y | deg | Sample rotation around y | 0 |
| rot_z | deg | Sample rotation around z | 0 |

## Links

- [Source code](McStas_Single_crystal.instr) for `McStas_Single_crystal.instr`.
- Website for the <a href="https://mctools.github.io/mcpl">MCPL</a> particle exchange format
- Website for the <a href="http://neutron.ujf.cas.cz/ restrax/">SIMRES</a> package
- Website for <a href="https://www.sine2020.eu/randd-activities/e-tools.html">WP8 in EU-SINE2020</a>

---
