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

## Examples

- **Test: FRAC=0.1 sampleconf=0 Detector: PSDdet1_I=117.629**
- **Test: FRAC=0.1 sampleconf=1 Detector: PSDdet1_I=18.684**
- **Test: FRAC=0.1 sampleconf=2 Detector: PSDdet1_I=18.684**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| glen | m | Length of the elliptical guides | 1.4 |
| flen | m | Focal-length of elliptical guides | 0.4 |
| w1 | m | Entry-width of elliptical guides | 0.05 |
| vm | 1 | m-value of material for left and right vertical guide mirrors | 3.0 |
| FRAC | 1 | Fraction of statistics used to model incoherent scattering from sample | 0 |
| sampleconf | 1 | Choose between three different sample configurations, see Multilayer_sample for details | 0 |

## Links

- [Source code](ISIS_CRISP.instr) for `ISIS_CRISP.instr`.
- <a href="http://www.isis.stfc.ac.uk/instruments/crisp/">Website of the CRISP instrument</a>

---
