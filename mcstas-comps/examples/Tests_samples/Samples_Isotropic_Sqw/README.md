# The `Samples_Isotropic_Sqw` Instrument

*McStas: A test instrument for the S(q,w) sample*

## Identification

- **Site:** Tests_samples
- **Author:** E. Farhi
- **Origin:** ILL
- **Date:** Jan 2004

## Description

```text
This instrument is a test instrument for the S(q,w) sample.
It produces a tof-angle and angle-energy detectors and also exports the
S(q,w) and S(q) data.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | source energy | 3.4 |
| sample_coh | str | name of coherent Sqw data file | "Rb_liq_coh.sqw" |
| sample_inc | str | name of incoherent Sqw data file | "Rb_liq_inc.sqw" |

## Links

- [Source code](Samples_Isotropic_Sqw.instr) for `Samples_Isotropic_Sqw.instr`.
- The Isotropic_Sqw sample

---
