# The `FZJ_KWS2_Lens` Instrument

*McStas: FZ Juelich KWS2 SANS, serving as test instrument for the Lens_simple component.*

## Identification

- **Site:** FZ_Juelich
- **Author:** Henrich Frielinghaus
- **Origin:** FZ Juelich
- **Date:** June 2010

## Description

```text
This instrument is a test instrument for the Lens_simple component.

Notice effect of gravitation on the final beam.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons | 20.15 |
| dlambda | AA | Wavelength spread of neutrons (fwhm) | 2.015 |
| FLUX | 1 | Source flux normalization constant | 1e8 |
| NGblen | 1 | Defining source radius (is \sqrt(2)*NGblen) | 0.03 |
| Cblen | m | Dimension of rectangular guide | 0.005 |
| Clen | m | Distance source-guide | 20.0 |
| Dlen | m | Placement of final detector | 20.0 |
| Rlense | m | Radius of lens | 0.025 |

## Links

- [Source code](FZJ_KWS2_Lens.instr) for `FZJ_KWS2_Lens.instr`.
- The Lens_simple component

---
