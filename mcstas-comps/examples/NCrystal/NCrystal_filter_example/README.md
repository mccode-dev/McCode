# The `NCrystal_filter_example` Instrument

*McStas: Example instrument for NCrystal_filter use*

## Identification

- **Site:** NCrystal
- **Author:** Thomas Kittelmann
- **Origin:** ESS
- **Date:** 2026

## Description

```text
A beam with wavelengths from 0.5 to 9.5 Aa passes through a filter
(NCrystal_filter), with wavelength monitors before and after it. With the
default cooled Be filter, the transmission is low below the Bragg cut-off at
3.96 Aa, and high above it.
```

## Examples

- **Test: thickness=0.1 Detector: mon_after_I=0.000105**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| cfg | str | NCrystal cfg-string of the filter material. | "stdlib::Be_sg194.ncmat;temp=80K" |
| thickness | m | Thickness of the filter. | 0.1 |

## Links

- [Source code](NCrystal_filter_example.instr) for `NCrystal_filter_example.instr`.
- NCrystal_filter

---
