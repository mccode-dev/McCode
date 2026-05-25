# The `Reflectometer` Instrument

*McStas: Simple reflectometer with two slits, a sample (either none, mirror or multilayer),
and a detector. For use in the OMIC summer school 2012.*

## Identification

- **Site:** elearning
- **Author:** Pia Jensen (bozack@bozack.dk)
- **Origin:** Niels Bohr Instute, University of Copenhagen
- **Date:** 13.08.2012

## Description

```text
This simple reflectometer consists of a source (using the standard PSI parameters
for three Maxwellian distributions), on which the user can control the bandwidth
by simply choosing a minumum and maximum value. Two slits handle the divergence
distribution on the sample. The sample itself can either be an empty spot, a simple
mirror, or a multilayer. A simple PSD detector is used for detecting the scattered
beam. The scattering is in the horizontal plane.

Example: mcrun reflectometer.instr <parameters=values>
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda_min | AA | Minimum wavelength from source | 5.3 |
| lambda_max | AA | Maximum wavelength from source | 5.45 |
| slittranslation | m | Translation of slit (horizontal) | 0 |
| sampletranslation | m | Sample translation (horizontal) | 0 |
| slitwidth | m | Width of slit pinholes | 0.001 |
| slitheight | m | Height of slit pinholes | 0.002 |
| dist_source2slit | m | Distance between source and first slit | 1 |
| dist_slit2slit | m | Distance between slits | 3.2 |
| dist_slit2sample | m | Distance between second slit and sample | 0.18 |
| dist_sample2detector | m | Distance between sample and detector | 2 |
| sampletype | 1 | Sample type: 0 none, 1 mirror, 2+ multilayer | 1 |
| samplesize | m | Side-length of the (quadratic) sample plate | 0.15 |
| substratethickness | m | Thickness of the substrate | 0.003 |
| MR_Qc | AA | Critical Q-vector length of mirror sample | 0.15 |
| sampleangle | deg | Rotation angle of sample (theta) | 2.5 |
| detectorangle | deg | Rotation angle of detector (2 theta) | 5 |

## Links

- [Source code](Reflectometer.instr) for `Reflectometer.instr`.

---
