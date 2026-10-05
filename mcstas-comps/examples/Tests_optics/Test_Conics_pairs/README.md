# The `Test_Conics_pairs` Instrument

*McStas: Unit test instrument for Conics Pairs components*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Dec '21

## Description

```text
* Example instrument that shows some ways of using the Conics pair components

*
```

## Examples

- **Test: Test_Conics_pairs OPTIC=1 Detector: psd_i_I=6.91402e-13**
- * %Example: Test_Conics_pairs OPTIC=1 Detector: psd_i_reflections_2_I=6.91402e-13
- * %Example: Test_Conics_pairs OPTIC=2 fs=100000 Detector: psd_i_I=2.85546e-21
- * %Example: Test_Conics_pairs OPTIC=2 fs=100000 Detector: psd_i_reflections_2_I=2.85515e-21
- * %Example: Test_Conics_pairs OPTIC=3 Detector: psd_i_I=6.91402e-13
- * %Example: Test_Conics_pairs OPTIC=4 ssize=1e-4 Detector: psd_i_I=8.66267e-09
- * %Example: Test_Conics_pairs quadratic=1 Detector: psd_i_I=5.03026e-13
- * %Example: Test_Conics_pairs quadratic=1 fs=8 fi=6 Detector: psd_i_I=1.26085e-12
- * %Example: Test_Conics_pairs OPTIC=2 quadratic=1 fs=100000 Detector: psd_i_I=2.09716e-21
- * %Example: Test_Conics_pairs OPTIC=3 quadratic=1 Detector: psd_i_I=5.03026e-13
- * %Example: Test_Conics_pairs OPTIC=3 quadratic=1 fs=8 fi=6 Detector: psd_i_I=9.00287e-13
- * %Example: Test_Conics_pairs OPTIC=4 quadratic=1 ssize=1e-4 Detector: psd_i_I=6.56678e-09

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| OPTIC |  | Flag to choose between 1: EH pair, 2: PH pair, 3: HE pair, 4: PP pair | 1 |
| ssize | m | Source radius | 1e-6 |
| fs | m | Distance betwee nsource and optic mid plane | 10 |
| fi | m | Distance between optics mid plane and focal point. | 10 |
| R0 | 1 | Mirror substrate reflectivity | 0.99 |
| m | 1 | m-value of supermirrors | 3 |
| W | AA^-1 | Width of supermirror cut-off | 0.003 |
| alpha | AA | Slope of reflectivity for reflectivity curve approximation | 6.07 |
| nshells | 1 | Number of Wolter-optic shells | 4 |
| rmin | m | Radius of the innermost shell | 0.0031416 |
| rmax | m | Radius of the outermost shell | 0.05236 |
| quadratic |  | Use the quadratic shell-radius mode instead of the radii vector | 0 |

## Links

- [Source code](Test_Conics_pairs.instr) for `Test_Conics_pairs.instr`.
- <reference/HTML link>

---
