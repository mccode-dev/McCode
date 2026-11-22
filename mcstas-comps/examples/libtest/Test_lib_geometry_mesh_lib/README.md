# The `Test_lib_geometry_mesh_lib` Instrument

*McStas: Smoke test: builds and runs with the library geometry-mesh-lib in the instrument.
Fails to compile or link if the library does not build with the runtime.
INITIALIZE also checks gmesh_init_fit, gmesh_inside, gmesh_entry_exit and
gmesh_intersect_gravity, and exits with an error when a check fails:
- gmesh_init_fit resizes cube.off about the origin with center=0, and about
its box centre, moved to the origin, with center=1
- gmesh_init counts the folded edges of a folded cube, which is then not
closed, and gmesh_check_closed fails it and the open Guide.off, and passes
cube.off and socket.off
- gmesh_inside on cube.off
- gmesh_entry_exit on cube.off, and its inside/outside from the first hit
against gmesh_inside for random points and directions around socket.off
- with zero acceleration it gives the hits of gmesh_intersect, to rounding
- parabolas through cube.off give the analytic hit times and in_out
- random parabolas through socket.off give the hits of a fine polyline of
straight gmesh_intersect pieces*

## Identification

- **Site:** libtest
- **Author:** 
- **Origin:** 
- **Date:** 

## Description

```text

```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Ncount |  |  | 10 |

## Links

- [Source code](Test_lib_geometry_mesh_lib.instr) for `Test_lib_geometry_mesh_lib.instr`.

---
