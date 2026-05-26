# The `Template_SasView` Instrument

*McXtrace: Test instrument for the SasView_model component. No optics.
etc.*

## Identification

- **Site:** Tests_samples
- **Author:** Torben Nielsen
- **Origin:** DTU Physics / ESS DMSC
- **Date:** Jan 2017.

## Description

```text
Very simple test instrument for 3 examples from the suite of SasView_ components.
Select sample model with 'model_index':
1= SasView_barbell
3= SasView_bcc_paracrystal
47= SasView_parallelepiped
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of X-rays | 1.54 |
| dlambda | AA | Wavelength spread of X-rays | 0.05 |
| model_index |  | SASmodels model index (see SasView_model.comp mcdoc page) | 10 |
| par1 |  | Slot 1 in SASmodel parameter vector | 0 |
| par2 |  | Slot 2 in SASmodel parameter vector | 0 |
| par3 |  | Slot 3 in SASmodel parameter vector | 0 |
| par4 |  | Slot 4 in SASmodel parameter vector | 0 |
| par5 |  | Slot 5 in SASmodel parameter vector | 0 |
| par6 |  | Slot 6 in SASmodel parameter vector | 0 |
| par7 |  | Slot 7 in SASmodel parameter vector | 0 |
| par8 |  | Slot 8 in SASmodel parameter vector | 0 |
| Ncount | 1 | Override the number of rays to simulate. | 0 |

## Links

- [Source code](Template_SasView.instr) for `Template_SasView.instr`.

---
