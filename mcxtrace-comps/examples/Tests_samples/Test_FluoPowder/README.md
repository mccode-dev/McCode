# The `Test_FluoPowder` Instrument

*McXtrace: Unit-test instrument for the FluoPowder sample component.*

## Identification

- **Site:** Tests_samples
- **Author:** Emmanuel Farhi (emmanuel.farhi.synchrotron-soleil.fr)
- **Origin:** Synchrotron Soleil
- **Date:** 2025

## Description

```text
Simply a model source illuminating a fluo/powder/sx sample.
The default sample itself is an LaB6-powder.
The idea is to compare the fluorescence and diffraction patterns:
- index=1: use PowderN              (no fluorescence)
- index=2: use Single_crystal       (no fluorescence, slower, powder mode)
- index=3: use FluoPowder           (exact)
- index=4: use Fluorescence         (no diffraction)
- index=5: use Fluorescence+PowderN (exact)
```

## Examples

- **Test: Test_FluoPowder.instr E0=15 index=1 Detector: Sph_mon_pow_I=1.77806e-19**
- Example: Test_FluoPowder.instr E0=15 index=2 Detector: Sph_mon_pow_I=1.5129e-19
- **Test: Test_FluoPowder.instr E0=15 index=3 Detector: Sph_mon_pow_I=1.77226e-19**
- **Test: Test_FluoPowder.instr E0=15 index=4 Detector: Sph_mon_flu_I=6.18074e-19**
- Example: Test_FluoPowder.instr E0=15 index=5 Detector: Sph_mon_pow_I=1.77403e-19
- 
- (Fluorescence+PowderN GROUP is unstable on GPU, disable index=5 test for now)

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Source mean energy of xrays. | 15 |
| dE | keV | Source energy half spread of x-rays. | 0.05 |
| L1 | m | Source-sample distance. | 10 |
| material | str | Material structure/composition as formula or LAU/CIF format. | "LaB6.cif" |
| index | 1 | Index of the sample component to use. 1=PowderN, 2=Single_crystal (much slower), 3=FluoPowder, 4=Fluorescence, 5=Fluorescence+PowderN in a GROUP (slower) | 3 |
| delta_d_d | 1 | Lattice spacing variance, gaussian RMS | 3e-4 |

## Links

- [Source code](Test_FluoPowder.instr) for `Test_FluoPowder.instr`.

---
