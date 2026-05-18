# The `Tagging_demo` Instrument

*McStas: Union demo: How to use tagging*

## Identification

- **Site:** Union_demos
- **Author:** Mads Bertelsen
- **Origin:** Johns Hopkins University, Baltimore
- **Date:** May 2016

## Description

```text
Powder in Al can

Example: Detector: Banana_monitor_I=9.84391e-09
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| material_data_file | string | Name of data file for powder sample | "Cu.laz" |
| E0 | meV | Source mean energy | 100 |
| dE | meV | Source energy spread | 2 |
| sample_radius | m | Sample radius | 0.01 |
| sample_height | m | Sample height | 0.01 |
| pack | 1 | Packing factor, 1 is full density | 1 |
| sigma_inc | barns | Incoherent cross section for powder sample | 2.2 |
| sigma_abs | barns | Absorption cross section for powder sample | 15.12 |
| Vc | AA | Unit cell volume for powder sample | 47.22 |
| geometry_interact | 1 | Fraction of beam to interact with geometry | 0.5 |
| incoherent_fraction | 1 | Fraction of scattered events that select the powder process | 0.2 |

## Links

- [Source code](Tagging_demo.instr) for `Tagging_demo.instr`.

---
