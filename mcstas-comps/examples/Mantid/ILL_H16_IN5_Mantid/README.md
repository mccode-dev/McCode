# The `ILL_H16_IN5_Mantid` Instrument

*McStas: The full ILL IN5B ToF spectrometer adapted for use with Mantid-friendly NeXus output.*

## Identification

- **Site:** Mantid
- **Author:** E. Farhi, J. Ollivier, Celia Castan Guerrero
- **Origin:** Mantid
- **Date:** Jan 2004-2009

## Description

```text
The full ILL IN5B: H16 guide & chopper system + sample + PSD and tof detector, adapted for
use with Mantid-friendly NeXus output. To compile and run, use these steps:

<ol>
<li>Compile the instrument with NeXus support - zero neutrons:
<ul>
<li>export MCSTAS_CFLAGS="-g -lm -O2 -DUSE_NEXUS -lNeXus"
<li>mcrun -c -n0 --format=NeXus ILL_H16_IN5_Mantid.instr --no-output-files
</ul>
<li>Generate the IDF in XML format using the (Perl) mcdisplay tool:
<ul>
<li>mcdisplay ILL_H16_IN5_Mantid.instr -n0 --format=Mantid
</ul>
<li>Run a simulation with NeXus output
<ul>
<li>mcrun ILL_H16_IN5_Mantid.instr --format=NeXus
</ul>
</ol>

The IN5@ILL TOF spectrometer from cold source to final detector, with sample.
The detector model includes Fe housing and tube cross talk absorbing masks
with angle restriction (neutrons that scatter in Fe in front of a tube and
enter a different tube are absorbed).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | mean incident wavelength | 4.5 |
| dlambda | AA | wavelength half width. Use e.g. 0.8*lambda for white beam. | 0.09 |

## Links

- [Source code](ILL_H16_IN5_Mantid.instr) for `ILL_H16_IN5_Mantid.instr`.
- The <a href="http://www.ill.eu/in5">IN5@ILL</a> cold time of flight instrument

---
