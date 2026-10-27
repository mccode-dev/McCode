# The `Test_mesh_boxes` Instrument

*McStas: A small instrument to investigate mesh boxes in a user configuration*

## Identification

- **Site:** Tests_union
- **Author:** Sam Lambrick with Python McStas Instrument Generator
- **Origin:** ESS DMSC / ISIS
- **Date:** 10:07:56 on March 05, 2026

## Description

```text
Instrument provided by Sam Lambrick from ISIS, that uses three meshes to
make their boxes.
```

## Examples

- **Test: Detector: psd_detector_I=10.7752**
- **Test: box_file="box.off" blade_file="single_blade.off" Detector: psd_detector_I=10.7752**
- **Test: box_file="box.ply" blade_file="single_blade.ply" Detector: psd_detector_I=10.7752**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| box_file | str | Mesh file of the Be envelope: box.stl (ascii STL), box.off or box.ply | "box.stl" |
| blade_file | str | Mesh file of the Cd blades: single_blade.stl (ascii STL), single_blade.off or single_blade.ply | "single_blade.stl" |

## Links

- [Source code](Test_mesh_boxes.instr) for `Test_mesh_boxes.instr`.

---
