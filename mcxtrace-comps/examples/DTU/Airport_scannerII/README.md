# The `Airp_scannerII` Instrument

*McXtrace: Example instrument to display how to use the absorption multiobject component*

## Identification

- **Site:** DTU
- **Author:** E. B. Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jan '21

## Description

```text
This instrument simply has a lab source, a few monitors and a multi-off object scene
by means of the Abs_objects component. By supliying a control file and a set of
off/ply-files a scene is put together which may be used for tomography style
simulations. The default input file contains a single object: a mechanical socket.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SFILE |  | Name of file that contains the off/ply parameters for the scene | "input_abs_objects_template.dat" |
| ANGLE |  | Rotation around y-axis | 0 |
| posX | m | Displacement of scene along x-axis | 0 |
| posY | m | Displacement of scene along y-axis | 0 |
| posZ | m | Displacement of scene along z-axis | 0 |
| Ncount | 1 | Set X-ray particle count for the simulation (same as -n #) | 0 |

## Links

- [Source code](Airport_scannerII.instr) for `Airport_scannerII.instr`.

---
