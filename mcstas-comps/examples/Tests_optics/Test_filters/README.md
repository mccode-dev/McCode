# The `Test_filters` Instrument

*McStas: Test instrument for McStas components as Be-filters*

## Identification

- **Site:** PSI
- **Author:** <a href="mailto:peter.willendru@ess.eu">Peter Willendrup</a>
- **Origin:** <a href="http://www.ess.eu">ESS</a>
- **Date:** March 2026

## Description

```text
Test instrument for cross-checking McStas components for use as Be-filters.
Where applicable, the temperature is set to 80K (corresponding to the header
of the Be.trm file used in Filter_gen
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lmin | Angs | Lowest wavelength from source | 0.5 |
| Lmax | Angs | Highest wavelength from source | 10 |
| Filter | 1 | Choice of filter 0: Filter_gen, 1: NCrystal, 2: PowderN, 3: Isotropic_Sqw | 0 |
| nL | 1 | Number of wavelength bins in [Lmin Lmax] interval | 101 |
| zdepth | m | Depth of Be-filter | 0.15 |
| T | K | Filter temperature (NCrystal only) | 80 |

## Links

- [Source code](Test_filters.instr) for `Test_filters.instr`.
- <a href="https://doi.org/10.1107/S0567739478000121">Acta Cryst. (1978). A34, 61-65</a>
- <a href="https://doi.org/10.1063/1.1140212">Rev. Sci. Instrum. 59, 380-381 (1988)</a>
- Test instrument written by P Willendrup <peter.willendrup@ess.eu> ESS, March 2026

---
