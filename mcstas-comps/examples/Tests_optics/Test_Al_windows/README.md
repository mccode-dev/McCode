# The `Test_Al_windows` Instrument

*McStas: Small test instrument for studying Al-window performance*

## Identification

- **Site:** Tests_optics
- **Author:** Peter Willendrup
- **Origin:** ESS
- **Date:** 20260316

## Description

```text
Test instrument for studying transmission through Al-windows.

Includes 3 models of increasing complexity / sophistication:
1) Al_window(),:
- simple linear absorption + 4th order scattering model
- infinite-plane geometry
2) PowderN()
- Material specified from Al.laz input-file
- Parametrized for 80% statistics -> transmission
3) NCrystal()
- Material specified from local Al as .ncmat format input
(generated from CIF file COD / 1502689)
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Window | 1 | Choice of Al_Window (1), PowderN (2) or NCrystal (3) model | 1 |
| thickness | m | Thickness of Al window | 0.005 |

## Links

- [Source code](Test_Al_windows.instr) for `Test_Al_windows.instr`.
- <reference/HTML link>

---
