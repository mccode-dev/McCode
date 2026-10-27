# The `Geometry_test` Instrument

*McStas: Test of all Union geometry components in one file*

## Identification

- **Site:** Tests_union
- **Author:** Mads Bertelsen
- **Origin:** University of Copenhagen
- **Date:** September 2015

## Description

```text
Use of all basic geometries, mesh(as a torus), sphere, cylinder, cone and box
```

## Examples

- **Test: meshfile="torus.off" Detector: detector_scat_I=47.5**
- **Test: meshfile="torus.STL" Detector: detector_scat_I=47.5**
- **Test: meshfile="torus.ply" Detector: detector_scat_I=47.5**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| meshfile | str | Name of the mesh geometry file: torus.STL (binary STL), torus.off or torus.ply | "torus.STL" |

## Links

- [Source code](Geometry_test.instr) for `Geometry_test.instr`.

---
