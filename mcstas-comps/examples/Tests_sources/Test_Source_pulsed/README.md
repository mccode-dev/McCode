# The `Test_Source_pulsed` Instrument

*McStas: A test instrument to check the component 'Source_pulsed'*

## Identification

- **Site:** Tests_optics
- **Author:** Klaus LIEUTENANT (k.lieutenant@fz-juelich.de)  based on 'Test_9-Sources' by Emmanuel FARHI (farhi@ill.fr)
- **Origin:** FZJ
- **Date:** Mar 17, 2021

## Description

```text
A test instrument to check if the component 'Source_pulsed' provides valid spectra and intensities.
```

## Examples

- **Test: source=1 Detector: m1_I=4.680e+09**
- **Test: source=2 Detector: m1_I=2.305e+09**
- **Test: source=3 Detector: m1_I=5.292e+09**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| source | 1 | selection of the source to use in 1=Source_pulsed for HBS thermal, 2=Source_pulsed for HBS cold, 3=Source_pulsed for HBS bispectral | 1 |
| Lmin | Ang | Minimum wavelength produced at source | 0.1 |
| Lmax | Ang | Maximum wavelength produced at source | 10.1 |

## Links

- [Source code](Test_Source_pulsed.instr) for `Test_Source_pulsed.instr`.

---
