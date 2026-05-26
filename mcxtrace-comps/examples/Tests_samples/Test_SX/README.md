# The `Test_SX` Instrument

*McXtrace: Unit-test instrument for the Single_crystal sample component.

Simply a model source illuminating a SX sample.
The default sample itself is a Mo bulk crystal.
The idea is to compare the fluorescence and diffraction patterns:
- index=1: use Single_crystal
- index=2: use FluoCrystal
- index=3: use Fluorescence+Single_crystal in a GROUP*

## Identification

- **Site:** Tests_samples
- **Author:** E. Farhi
- **Origin:** Synchrotron Soleil
- **Date:** Sept 26th 2019

## Description

```text

```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| reflections | str | List of powder reflections, LAU/CIF format. | "Mo.lau" |
| index | 1 | Index of the sample component to use. 1=Single_crystal, 2=FluoCrystal, 3=Fluorescence+Single_crystal in a GROUP | 1 |
| E0 | keV | Source mean energy of xrays. | 7 |
| dE | keV | Source energy half spread of x-rays. | 6.9 |

## Links

- [Source code](Test_SX.instr) for `Test_SX.instr`.

---
