# The `SOLEIL_ROCK` Instrument

*McXtrace: ROCK beam-line at SOLEIL*

## Identification

- **Site:** SOLEIL
- **Author:** Stephane Bac, Antoine Padovani, Emmanuel Farhi
- **Origin:** SOLEIL
- **Date:** 23/02/2022

## Description

```text
ROCK : Rocking Optics for Chemical Kinetics ROCK time-resolved X-ray

Absorption spectroscopy (XAS) beamline Energy range 4 - 40 keV
The ROCK beamline (ROCK being the acronym for Rocking Optics for Chemical Kinetics)
is devoted to the study of fast kinetic processes in nanomaterials used in
catalysis and batteries. The objective is to contribute to the development of
more efficient catalysts and batteries which should find successful industrial
applications in the field of energy generation and storage in compliance with
the protection of public health and environment. The better knowledge at the
atomic scale of nanomaterials involved in catalysis or energy storage provided
by time-resolved XAS is recognized by the concerned communities as mandatory
for establishing synthesis strategies leading to important breakthroughs in
the production of energy from renewable sources and in the development of
advanced energy storage devices.

Position | Element
---------|--------------------------------------------------------------------
0        | Bending_magnet 1.72 T
8.5336   | Slit 1/2
10.15    | Toroidal mirror M1
11.69    | Slit 3
16.82    | Mirror M2a
18.15    | Slit 4
19       | Channel-cut Si monochromator at 20 (Si220), 18.92 or 19.25 m (Si111)
21.13    | Slit 5/6
22.44    | Mirror M2b
32.44    | Sample

Examples:
Copper scan: mxrun SOLEIL_ROCK.instr E0=8.500,9.500 scan=1 cc=2 -N100
Manganese and chrome scan: mxrun SOLEIL_ROCK.instr E0=5.700,6.800 scan=1 sample_file=MnCr cc=2 -N100
Pretty energy repartition monitor result: mxrun SOLEIL_ROCK.instr E0=17.000 cc=2 -n1e8
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Energy to hit, i.e. selected by the channel-cut monochromator | 15.918 |
| dE | keV | Energy spread at the source | 1 |
| scan | 0-1 | 0 no energy scan, 1 energy scan | 0 |
| angle_m2a_m2b | rad | M2A/M2B mirror's deviation angle, can vary from 0.0035 to 0.0104 | 0.008 |
| angle_m1 | rad | M1 mirror's deviation angle | 0.0045 |
| sample_file | string | Sample chemical formulae | "CuMo" |
| reflec_material_M1 | str | Material reflectivity file name for M1 mirror, e.g. "Ir.dat" | "Ir.dat" |
| reflec_material_M2A_M2B | str | Material reflectivity file name for M2A and M2B mirror. Use NULL for automatic setting. | "NULL" |
| cc | 0-3 | Channel-cut monochromator type. 0 for Si 220 with an energy range of  5.62883-46.2834 eV/4-35 deg (hit-table:11.752-34.055 ev/5.44-15.94 deg); 1 for Si 111 long 3.44694-28.3427 eV/4-35 deg (hit-table:7.196-20.854 ev/5.44-15.94 deg); 2 for Si 111 short 3.44694-18.914.3 eV/6-35 deg (hittable:5.323-18.914 ev/6-21.8 deg); 3 changes cc dynamically | 0 |

## Links

- [Source code](SOLEIL_ROCK.instr) for `SOLEIL_ROCK.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/rock
- https://gitlab.synchrotron-soleil.fr/grades/mcxtrace-rock
- https://github.com/antoinepado/glitch_runner_rock

---
