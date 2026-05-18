# The `Simple_1shell` Instrument

*McXtrace: Single shell model of a true Wolter Type I optic*

## Identification

- **Site:** AstroX_ESA
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk> & Desiree D. M. Ferreira <desiree@space.dtu.dk> (email)
- **Origin:** DTU Physics/DTU Space
- **Date:** 12/12/2016

## Description

```text
Single shell example telescope using a combinatiopn pf parabolic/hyperbolic shell
optical plates covering the full circle. Reflectivity is 1 for all energies.

Example: Simple_1shell.instr FL=12
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| FL | m | The focal length of the optical system | 12 |
| plate_zdepth | m | Plate length. | 0.5 |
| channel_yheight | m | Channel height. | 1e-2 |
| radius_m | m | Radius at the optics centre. | 0.534927 |
| radius_p | m | Radius at the entry to the primary (parabolic) shell. | 0.535532 |
| radius_h | m | Radius at the exit of the secondary (hyperbolic) shell. | 0.533113 |

## Links

- [Source code](Simple_1shell.instr) for `Simple_1shell.instr`.

---
