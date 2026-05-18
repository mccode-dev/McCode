# The `templateSasView_Mantid` Instrument

*McStas: Test instrument for the SasView_model component generating event data for Mantid. No guide / velocity selector
etc.*

## Identification

- **Site:** Mantid
- **Author:** Peter Willendrup and Torben Nielsen
- **Origin:** DTU, European Spallation Source ERIC
- **Date:** 25 Jan 2016

## Description

```text
Very simple test instrument for the SasView_model component.

Modified to show a proof of concept method for storing a 'Mantid friendly' type of NeXus file.

Example: model_index=1 Ncount=1e6 par1=4 par2=1 par3=40 par4=20 par5=400 Detector: detector_I=5e6
Example: model_index=3 Ncount=1e6 par1=220 par2=0.06 par3=40 par4=4 par5=1 Detector: detector_I=3.3e4
Example: model_index=47 Ncount=1e6 par1=4 par2=2 par3=35 par4=75 par5=400 Detector: detector_I=2.5e6

Needed steps (McStas):
1) Compile your instrument with NeXus library and ISO c99 support
2) Generate an IDF using mcdisplay templateSANS_Mantid --format=Mantid -n0
3) mcrun templateSasView_Mantid -n1e6 --format=NeXus

Needed steps (Mantid):
a) Load McStas NeXus file
b) Run Mantid algorithm: 'ConvertUnits' using the 'wavelenth' and 'elastic' options
c) Run Mantid algorithm: 'Qxy' using the options 'MaxQxy=0.6', 'DeltaQ=0.003', 'SolidAngleWeighting=False'
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons | 6 |
| dlambda | AA | Wavelength spread of neutrons | 0.05 |
| model_index |  | SASmodels model index (see SasView_model.comp mcdoc page) | 47 |
| par1 |  | Slot 1 in SASmodel parameter vector | 0 |
| par2 |  | Slot 2 in SASmodel parameter vector | 0 |
| par3 |  | Slot 3 in SASmodel parameter vector | 0 |
| par4 |  | Slot 4 in SASmodel parameter vector | 0 |
| par5 |  | Slot 5 in SASmodel parameter vector | 0 |
| par6 |  | Slot 6 in SASmodel parameter vector | 0 |
| par7 |  | Slot 7 in SASmodel parameter vector | 0 |
| par8 |  | Slot 8 in SASmodel parameter vector | 0 |
| Ncount |  |  | 0 |

## Links

- [Source code](templateSasView_Mantid.instr) for `templateSasView_Mantid.instr`.

---
