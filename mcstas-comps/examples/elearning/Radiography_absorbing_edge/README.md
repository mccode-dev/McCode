# The `Radiography_absorbing_edge` Instrument

*McStas: Instrument to study radiographic imaging of an absobing edge sample used for "Spacial resolution in neutron imaging" Moodle quiz in the Virtual Neutrons for Teaching project
http://vnt.nmi3.org/moodle/mod/quiz/view.php?id=56*

## Identification

- **Site:** elearning
- **Author:** Linda Udby and Peter Willendrup
- **Origin:** University of Copenhagen
- **Date:** July 4th, 2014

## Description

```text
Instrument to study radiographic imaging of an absobing edge sample used for "Spacial resolution in neutron imaging" Moodle quiz in the Virtual Neutrons for Teaching project
http://vnt.nmi3.org/moodle/mod/quiz/view.php?id=56

Example: mcrun  Radiography_absorbing_edge.instr -n5e8 l=0.2 -d EdgeImaging
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| D | m | Diameter of pinhole before sample | 0.01 |
| L | m | Distance between pinhole and sample | 5 |
| l | m | Distance between sample and detector | 0.10 |
| sigma_abs | barns | Absorption cross-section of the sample | 5.08 |
| Vc | AA^3 | Unit cell volume in the sample | 13.827 |
| sample_z |  |  | 0.01 |

## Links

- [Source code](Radiography_absorbing_edge.instr) for `Radiography_absorbing_edge.instr`.
- http://vnt.nmi3.org/mcstas-web

---
