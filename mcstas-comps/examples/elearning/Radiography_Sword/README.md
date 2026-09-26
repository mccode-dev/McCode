# The `Radiography_Sword` Instrument

*McStas: Viking Sword in Radiography instrument. Developed for PaNOSC (EU H2020 GA No. 823852).*

## Identification

- **Site:** elearning
- **Author:** Mads Bertelsen and Viktor L. Holm
- **Origin:** University of Copenhagen
- **Date:** August 2019

## Description

```text
This is a simulation of bragg-edge imaging preformed on a viking sword at an imaging beamline. Developed for PaNOSC (EU H2020 GA No. 823852).
The beam delivered from the source approximates that of the ODIN instrument at ESS and the model is meant for educational purposes.
```

## Examples

- **Test: Radiography_Sword -y Detector: screen_I=4.34897e+15**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| chopper_mode | 1 | Choose between 6 different chopper modes from 0 to 5. Chooper mode 5 is a white beam. | 5 |
| Lambda | AA | Choose a specific wavelength 1AA-10AA to use in the simulation. The wavelength must correspond to choosen chopper mode. Lambda=0 means that all wavelengths permitted by the chopper mode are used. | 0 |
| Sample | 0 | Choose if the sample is pressent in the simulation or not, Sample=0: not in the beam, Sample=1: sample is in the beam. | 1 |
| pinhole_diameter | m | Diameter of the pinhole. Allowed sizes are 0.01-0.1m. | 0.1 |
| pinhole_detector_distance | m | Distance between the pinhole and the detector. Allowed distances are 10m-25m. | 10.0 |
| pinhole_sample_distance | m | Distance between the pinhole and the sample. Allowed distances are between 1-25m. | 9 |
| X_sample_pos | m | Translation of sample in x-direction. | 0.0 |
| Y_sample_pos | m | Translation of sample in x-direction. | 0.0 |
| angle | deg | Sample-stage rotation (around y). | 0 |
| Zoom | 1 | Detector zooom: Reduce area and increase resolution detector by the same fact. Values between 1 and 10. | 1 |

## Links

- [Source code](Radiography_Sword.instr) for `Radiography_Sword.instr`.
- Originally developed for the <a href="https://e-learning.pan-training.eu/">PaNOSC e-learning portal</a>

---
