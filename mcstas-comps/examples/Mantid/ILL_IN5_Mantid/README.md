# The `ILL_IN5_Mantid` Instrument

*McStas: The IN5B instrument: chopper system + sample + PSD and tof detector, adapted for
use with Mantid-friendly NeXus output.*

## Identification

- **Site:** Mantid
- **Author:** E. Farhi, J. Ollivier, Celia Castan Guerrero
- **Origin:** ILL
- **Date:** Jan 2004-2009

## Description

```text
The ILL IN5@ILL TOF spectrometer from chopper system to final detector, with sample.
The detector model includes Fe housing and tube cross talk absorbing masks
with angle restriction (neutrons that scatter in Fe in front of a tube and
enter a different tube are absorbed). This model does not include the H16 guide.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | mean incident wavelength | 4.5 |
| dlambda | AA | wavelength half width. | 0.05 |
| speed | rpm | chopper speed (60*frequency) | 8500 |
| ratio | 1 | velocity of chopper3 = velocity of chopper1 * ratio | 0.5 |
| housing | string | material used as detector housing. Use 0 or NULL for none. | "Fe.laz" |
| coh | string | sample coherent data file or NULL. Use powder LAZ/LAU or SQW file. | "Y3Fe5O12_YIG.laz" |
| inc | string | sample incoherent Sqw data file or NULL | "NULL" |
| thickness | m | thickness of sample. 0=filled | 0 |
| height | m | height of sample. 0=sphere | 0.025 |
| radius | m | radius of sample (outer). | 0.005 |
| order | 1 | order for scattering in sample. O=all, 1=single | 0 |

## Links

- [Source code](ILL_IN5_Mantid.instr) for `ILL_IN5_Mantid.instr`.
- The <a href="http://www.ill.eu/in5">IN5@ILL</a> cold time of flight instrument

---
