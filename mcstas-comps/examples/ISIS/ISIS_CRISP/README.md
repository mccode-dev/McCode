# The `ISIS_CRISP` Instrument

*McStas: Model of the ISIS CRISP reflectometer, including the Multilayer_Sample reflectivity sample.*

## Identification

- **Site:** ISIS
- **Author:** <a href="robert.dalgliesh@stfc.ac.uk">Robert Dalgliesh</a>
- **Origin:** <a href="http://www.isis.stfc.ac.uk/">ISIS (UK)</a>
- **Date:** 2010

## Description

```text
This model of the ISIS CRISP reflectometer demonstrates the use of the Multilayer_Sample component.

The algorithm of the component requires complex numbers, facilitated by the
<a href="http://www.gnu.org/software/gsl/">GNU Scientific Library (GSL)</a>. To link
your the instrument with an installed GSL, you should use MCSTAS_CFLAGS like

MCSTAS_CFLAGS = -g -O2 -lm -lgsl -lgslcblas
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| glen |  |  | 1.4 |
| flen |  |  | 0.4 |
| w1 |  |  | 0.05 |
| vm |  |  | 3.0 |
| FRAC |  |  | 0 |
| sampleconf |  |  | 0 |

## Links

- [Source code](ISIS_CRISP.instr) for `ISIS_CRISP.instr`.

---
