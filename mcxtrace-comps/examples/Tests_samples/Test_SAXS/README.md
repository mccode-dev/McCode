# The `Test_SAXS` Instrument

*McXtrace: Toy model used for testing various sample components for solution-SAXS.*

## Identification

- **Site:** Tests_samples
- **Author:** Martin Cramer Pedersen (mcpe@nbi.dk) and Søren Kynde (kynde@nbi.dk)
- **Origin:** KU-Science
- **Date:** October 29th, 2012

## Description

```text
Toy model used for testing various sample components for solution-SAXS.

The following SAXS samples are handled:
SAXSSpheres               SAMPLE=0
SAXSShells                SAMPLE=1
SAXSCylinders             SAMPLE=2
SAXSEllipticCylinders     SAMPLE=3
SAXSLiposomes             SAMPLE=4
SAXSNanodiscs             SAMPLE=5
SAXSNanodiscsWithTags     SAMPLE=6
SAXSPDB                   SAMPLE=7 (very slow, rather use SAXSPDBFast)
SAXSCurve                 SAMPLE=8 (inactivated)
SAXSNanodiscsFast         SAMPLE=9
SAXSNanodiscsWithTagsFast SAMPLE=10
SAXSPDBFast               SAMPLE=11
```

## Examples

- **Test: SAMPLE=0 Detector: PSDMonitor_I=2.93715e-14**
- **Test: SAMPLE=1 Detector: PSDMonitor_I=2.7118e-14**
- **Test: SAMPLE=2 Detector: PSDMonitor_I=2.64954e-14**
- **Test: SAMPLE=3 Detector: PSDMonitor_I=9.77905e-15**
- **Test: SAMPLE=4 Detector: PSDMonitor_I=1.16433e-10**
- **Test: SAMPLE=5 Ncount=1e4 Detector: PSDMonitor_I=1.2001e-14**
- **Test: SAMPLE=6 Ncount=1e4 Detector: PSDMonitor_I=1.2400e-14**
- **Test: SAMPLE=7 Ncount=1e4 Detector: PSDMonitor_I=7.07865e-14**
- **Test: SAMPLE=9 Detector: PSDMonitor_I=1.11073e-14**
- **Test: SAMPLE=10 Detector: PSDMonitor_I=1.02884e-14**
- **Test: SAMPLE=11 Detector: PSDMonitor_I=5.47707e-15**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DistanceFromSourceToFirstPinhole | m | Distance to first pinhole from source. | 0.05 |
| DistanceFromSourceToSecondPinhole | m | Distance to second pinhole - used for focusing rays. | 0.7 |
| DistanceFromSecondPinholeToSample | m | Collimation length. | 0.6 |
| DistanceFromSampleToDetector | m | Sample-detector-distance. | 0.48 |
| RadiusOfDetector | m | Radius of the circular detector. | 0.1 |
| Lambda | AA | Wavelength of the rays emitted from source. | 1.54 |
| DLambda |  | Relative deviation of wavelength of the rays emitted from source. | 0.01 |
| SAMPLE |  | Index of sample model, see above. | 0 |
| Ncount | 1 | Override the number of rays to simulate. | 0 |

## Links

- [Source code](Test_SAXS.instr) for `Test_SAXS.instr`.

---
