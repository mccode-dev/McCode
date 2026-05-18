# The `SOLEIL_MARS` Instrument

*McXtrace: The MARS beam-line at Synchrotron SOLEIL.*

## Identification

- **Site:** SOLEIL
- **Author:** IGUILIZ Salah-Eddine, MENUT Denis and FARHI Emmanuel.
- **Origin:** SOLEIL
- **Date:** July 2022

## Description

```text
MARS beamline is aiming to extend the research capabilities on radioactive
matter towards the use of synchrotron radiation in multidisciplinary fields
biology, chemistry, physics) with respect to national and European safety law*.

The design of MARS beamline (infrastructure and optics) is also optimized to
alternatively run two experimental stations in orlder to perform
characterizations with transmission and high resolution X-ray powder-diffraction
(XRD), Wide Angle X-ray Scattering (WAXS), Small Angle X-ray scattering (SAXS),
standard and high resolution X-ray absorption spectroscopy (XANES, EXAFS and
HERFD-XANES) and microbeam techniques (microXRF, XAS, XRD).

This model implements the XRD station for powders.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central energy to be emitted by the source | 16.99 |
| reflec_material_M12 | str | reflecting coating on curved mirrors, e.g. Pt | "Pt.dat" |
| reflections | str | Sample structure file, LAU/CIF format. | "LaB6_660b_AVID2.hkl" |
| dEr | 1 | Relative half width to emitted by the source, e.g. 1e-4 | 1e-4 |
| alpha | deg | Asymmetry angle for the crystals. | 0 |

## Links

- [Source code](SOLEIL_MARS.instr) for `SOLEIL_MARS.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/mars

---
