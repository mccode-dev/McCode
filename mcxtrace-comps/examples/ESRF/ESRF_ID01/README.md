# The `ESRF_ID01` Instrument

*McXtrace: Nano-diffraction imaging beamline at ESRF, Grenoble*

## Identification

- **Site:** ESRF
- **Author:** Martin Cramer Pedersen (mcpe@nbi.dk)
- **Origin:** University of Copenhagen
- **Date:** March, 2015

## Description

```text
This model of ID01 is designed specifically to conduct virtual scanning nano-diffraction imaging
experiments such as the one demonstrated Chahine et al., J. Appl. Cryst 47, 762-769. The model
includes beam-defining slits, a double Si111-monochromator, a Fresnel Zone Plate, a polycrystalline
sample designed specifically for this type of virtual experiments, and a detector set up mimicking the
set up at the actual beam-line. The nanodiffraction experiment is performed by locating a suitable peak
in the diffraction of the crystal (using the variables NominalEnergy, Delta, and Eta)
and then scan across the sample (using the variables Pix and ySamplePosition). By plotting
e.g. total detector intensity as a function of position, one can map out impurities in the sample by
their scattering properties.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DistanceSampleToDetector | m | Distance from sample to detector | 0.5 |
| NominalEnergy | keV | Nominal energy of photons after monochromation | 8.0 |
| Delta | deg | Angle rotating the detector around the sample in the yz-plane | 0.0 |
| Nu | deg | Horizontal plane rotation angle of the detector (for asymmetric peaks) | 0.0 |
| Eta | deg | Angle between the incoming beam and the sample normal in the yz-plane | 0.0 |
| Phi | deg | Horizontal plane rotation angle of the sample (for asymmetric peaks) | 0.0 |
| Pix | um | Horizontal offset of sample | 0.0 |
| Piy | um | Vertical offset of sample | 0.0 |
| SampleMosaicity | moa | Mosaicity of the sample crystal lattice | 1.0 |
| SampleDeltadoverd | 1 | Uncertainty in lattice parameter for the sample crystal lattice | 0.001 |

## Links

- [Source code](ESRF_ID01.instr) for `ESRF_ID01.instr`.
- http://www.esrf.eu/UsersAndScience/Experiments/StructMaterials/ID01

---
