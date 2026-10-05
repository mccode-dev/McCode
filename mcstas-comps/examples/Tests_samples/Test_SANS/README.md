# The `Test_SANS` Instrument

*McStas: Toy model used for testing various sample components for solution-SANS.*

## Identification

- **Site:** Tests_samples
- **Author:** Martin Cramer Pedersen (mcpe@nbi.dk) and Søren Kynde (kynde@nbi.dk)
- **Origin:** KU-Science
- **Date:** October 29th, 2012

## Description

```text
Toy model used for testing various sample components for solution-SANS.

The following SANS samples are handled:
SANSSpheres               SAMPLE=0
SANSShells                SAMPLE=1
SANSCylinders             SAMPLE=2
SANSEllipticCylinders     SAMPLE=3
SANSLiposomes             SAMPLE=4
SANSNanodiscs             SAMPLE=5
SANSNanodiscsWithTags     SAMPLE=6
SANSPDB                   SAMPLE=7 (very slow, rather use SANSPDBFast)
SANSCurve                 SAMPLE=8 (inactivated)
SANSNanodiscsFast         SAMPLE=9
SANSNanodiscsWithTagsFast SAMPLE=10
SANSPDBFast               SAMPLE=11
```

## Examples

- **Test: SAMPLE=0 Detector: PSDMonitor_I=2.529e-09**
- **Test: SAMPLE=1 Detector: PSDMonitor_I=2.486e-10**
- **Test: SAMPLE=2 Detector: PSDMonitor_I=2.297e-10**
- **Test: SAMPLE=3 Detector: PSDMonitor_I=8.720e-11**
- **Test: SAMPLE=4 Detector: PSDMonitor_I=1.326e-05**
- **Test: SAMPLE=5 Ncount=1e6 Detector: PSDMonitor_I=2.714e-09**
- **Test: SAMPLE=6 Ncount=1e6 Detector: PSDMonitor_I=2.051e-09**
- **Test: SAMPLE=7 Ncount=1e5 Detector: PSDMonitor_I=1.687e-09**
- **Test: SAMPLE=9 Detector: PSDMonitor_I=2.071e-09**
- **Test: SAMPLE=10 Detector: PSDMonitor_I=2.079e-09**
- **Test: SAMPLE=11 Detector: PSDMonitor_I=1.642e-09**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DistanceFromSourceToFirstPinhole | m | Distance to first pinhole from source. | 1.0 |
| DistanceFromSourceToSecondPinhole | m | Distance to second pinhole - used for focusing rays. | 10.0 |
| DistanceFromSecondPinholeToSample | m | Collimation length. | 1.0 |
| DistanceFromSampleToDetector | m | Sample-detector-distance. | 10.0 |
| RadiusOfDetector | m | Radius of the circular detector. | 4.0 |
| Lambda | AA | Wavelength of the rays emitted from source. | 4.5 |
| DLambda |  | Relative deviation of wavelength of the rays emitted from source. | 0.1 |
| SAMPLE |  | Index of sample model, see above. | 0 |
| Ncount | 1 | Override the number of rays to simulate. | 0 |

## Links

- [Source code](Test_SANS.instr) for `Test_SANS.instr`.

---
