# The `ESS_Testbeamline_HZB_V20` Instrument

*McStas: McStas model of the ESS testbeamline V20 at HZB in Berlin*

## Identification

- **Site:** ESS
- **Author:** <a href="mailto:alaa.falahat@gmail.com">Ala'a Al-Falahat</a>
- **Origin:** HZB
- **Date:** March 17, 2017

## Description

```text
McStas model of the ESS testbeamline V20 at HZB in Berlin. Please note that not
all geometrical sizes, distances and parameters have been fully validated.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda_min | AA | Minimum wavelength emitted by the source | 1 |
| lambda_max | AA | Maximum wavelength emitted by the source | 15 |
| frequency | Hz | Frequency of the choppers | 14 |
| Choppers_WFM1_in | 1 | Flag to indicate if WFM1 is active | 1 |
| Choppers_WFM2_in | 1 | Flag to indicate if WFM2 is active | 1 |
| Choppers_FOC1_in | 1 | Flag to indicate if FOC1 is active | 1 |
| Choppers_FOC2_in | 1 | Flag to indicate if FOC2 is active | 1 |
| s1_x | m | slit 1 width | 0.05 |
| s1_y | m | slit 1 height | 0.1 |
| s2_x | m | slit 2 width | 0.02 |
| s2_y | m | slit 2 height | 0.04 |
| s3_x | m | slit 3 width | 0.0138 |
| s3_y | m | slit 3 height | 0.033 |
| Offset_deg_SC1 | deg | Phase source chopper disk 1 | 0 |
| Offset_deg_SC2 | deg | Phase source chopper disk 2 | 0 |
| Offset_deg_BC1 | deg | Phase bandwidth chopper disk 1 | -25 |
| Offset_deg_BC2 | deg | Phase bandwidth chopper disk 2 | 57 |
| Offset_deg_WFM1 | deg | Phase wavelength frame multiplication chopper disk 1 | 47.1 |
| Offset_deg_WFM2 | deg | Phase wavelength frame multiplication chopper disk 2 | 76.76 |
| Offset_deg_FOC1 | deg | Phase frame overlap chopper disk 1 | 62.4 |
| Offset_deg_FOC2 | deg | Phase frame overlap chopper disk 2 | 12.27 |
| Z | m | Distance between Wavelength frame multiplication chopper disks 0.1 - 0.5 | 0.28 |
| sp | m | sample position | 50.6 |
| npulses | 1 | Number of pulses to simulate | 1 |
| emulate_reactor_emmision | 1 | Flag to emulate all emission times from the reactor | 0 |

## Links

- [Source code](ESS_Testbeamline_HZB_V20.instr) for `ESS_Testbeamline_HZB_V20.instr`.
- <a href="https://www.helmholtz-berlin.de/pubbin/igama_output?modus=einzel&gid=1845">V20 page</a> at the HZB website
- <a href="https://europeanspallationsource.se/workshops-facilities#v20-test-beamline">V20 info section</a> at the ESS website

---
