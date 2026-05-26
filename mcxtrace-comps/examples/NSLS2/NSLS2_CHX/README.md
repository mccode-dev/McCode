# The `NSLS2_CHX` Instrument

*McXtrace: Coherent hard x-ray beamline @ NSLS II*

## Identification

- **Site:** NSLS2
- **Author:** Jana Baltser
- **Origin:** NBI, BNL
- **Date:** March 2011

## Description

```text
This is a lay-out of the coherent hard x-ray (CHX) beamline at NSLS-II.
Since the ray-tracing method doesn't allow to account for partial coherence of the
undulator source, the best approximation was to use the gaussian photon beam and to neglect
diffraction effects on apertures.
The choice of the source sizes along with angular divergence corresponds to the situation of
high emittance (e=0.99nm).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L | m | Distance from the source to the SS1, secondary source aperture. | 33.5 |
| L2 | m | Position of the CRL. | 35.3 |
| L3 | m | Position of the Kinoform lens. | 44 |
| Energy | keV | primary energy used at the beamline. | 10 |

## Links

- [Source code](NSLS2_CHX.instr) for `NSLS2_CHX.instr`.

---
