# The `FZJ_BenchmarkSfin2` Instrument

*McStas: Test instrument for the H Frielinghaus SANS_benchmark2 component.

The below test uses defaults of the instrument (SANS_benchmark2 modnum=11)*

## Identification

- **Site:** FZ_Juelich
- **Author:** Henrich Frielinghaus
- **Origin:** FZ Juelich
- **Date:** 2013

## Description

```text

```

## Examples

- **Test: -y Detector: PSDnear_I=7400**
- 
- Many sample-modes are available, from the SANS_benchmark2 component:
- The component includes 19 sample-model-settings:
- case 0 - no coherent scattering
- case 1 - polymer with Mw = 2.000g/mol
- case 2 - polymer with Mw = 1.000.000g/mol
- case 3 - microemulsion
- case 4 - wormlike micelle
- case 5 - sphere, R = 25 AA
- case 6 - sphere, R = 500 AA
- case 7 - polymer blend
- case 8 - diblock copolymer
- case 9 - multilamellar vesicles
- case 10 - logarithmically spaced series of sharp peaks
- case 11 - logarithmically spaced series of sharp delta peaks !!!!!!
- ...
- case 15 - sphere, R = 150 AA
- case 18   free

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lbdmin | AA | Minimum wavelength produced at source | 2.00 |
| lbdmax | AA | Maximum wavelength produced at source | 8.5 |
| NGbend | m | Bender dimension, horizontal | 0.053 |
| NGblen | m | Bender dimension, vertical | 0.03 |
| Clen | m | Collimation length | 8.0 |
| SampD | m | Sample 'half-dimension' (geometry is actually 2 x value) | 0.01 |
| bendL | rad | Bender angle of deflection | 5.0 |
| bendR | m | Bender radius of curvature | 385.0 |
| bendM | 1 | Bender 'outer curve mirror m-value' | 7.0 |
| bendiM | 1 | Bender 'inner curve mirror m-value' | 7.0 |
| dfltM | 1 | Bender 'top/bottom mirror m-value' | 1.5 |
| vorPl | m | If >0, post-bender Guide-element | 0.5 |
| PolLen | m | If >0, length of polariser | 0.492 |
| PolTot | m | Total primary optic length | 2.30 |
| MDOWN | 1 | m-value of the Fe/Si-wafer for spin down neutrons | 3.6 |
| Min | 1 | m-value of material for top. horz. mirror of the outer guide | 1.5 |
| cnum | 1 | 'Degree of beam-polarisation'. Set to 0.0 for no polarizer as for benchmarking the normal SANS samples, to 2.0 for two cavities, keep default values as much as possible | 0.00 |
| ROT | deg | Pre-polariser cavity-rotation angle arounz (only if polarised mode) | 0.0 |
| modnum | 1 | Sample 'mode' (see mcdoc for SANS_benchmark2 component) | 11.0 |
| sglscatt | 1 | Sample control of single/multiple scattering (see mcdoc for SANS_benchmark2 component) | 1.0 |
| incs | 1 | Sample fraction of incoherently scattered neutrons (see mcdoc for SANS_benchmark2 component) | 0.0005 |

## Links

- [Source code](FZJ_BenchmarkSfin2.instr) for `FZJ_BenchmarkSfin2.instr`.

---
