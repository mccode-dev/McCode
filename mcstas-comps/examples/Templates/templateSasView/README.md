# The `templateSasView` Instrument

*McStas: Test instrument for some SasView_model components. No guide / velocity selector
etc.*

## Identification

- **Site:** Templates
- **Author:** Torben Nielsen
- **Origin:** DTU Physics / ESS DMSC
- **Date:** 

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
| lambda | AA | Mean wavelength of neutrons | 6 |
| dlambda | AA | Wavelength spread of neutrons | 0.05 |
| model_index |  | SASmodels model index (see SasView_model.comp mcdoc page) | 47 |
| par1 |  | Slot 1 in SASmodel parameter vector | 0 |
| par2 |  | Slot 2 in SASmodel parameter vector | 0 |
| par3 |  | Slot 3 in SASmodel parameter vector | 0 |
| par4 |  | Slot 4 in SASmodel parameter vector | 0 |
| par5 |  | Slot 5 in SASmodel parameter vector | 0 |
| par6 |  | Slot 6 in SASmodel parameter vector | 0 |
| par7 |  | Slot 7 in SASmodel parameter vector | 0 |
| par8 |  | Slot 8 in SASmodel parameter vector | 0 |
| Ncount |  |  | 0 |

## Links

- [Source code](templateSasView.instr) for `templateSasView.instr`.

---
