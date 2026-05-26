# The `RTP_SANS` Instrument

*McStas: The SANS instrument installed at Reactor TRIGA PUSPATI (Malaysia)*

## Identification

- **Site:** TRIGA
- **Author:** E. Farhi and Megat Harun Al-Rashid
- **Origin:** ILL/RTP
- **Date:** June 2014

## Description

```text
This is a 4m long SANS installed on a radial beam port 4 at the Reactor TRIGA
PUSPATI (RTP). Te beam port 4 is a radial tube that originates from the core,
through the graphite reflector. A thick Be filter selects the cold tail of the
thermal spectrum, and removes higher order PG reflections.
A PG(002) monochromator selects the 5A neutrons that are sent into a 4m - 4m
SANS with a 60 cm PSD detector 128x128 pixels.
The monochromator is here used in fixed double focusing geometry.
The accessible Q range is then 0.01-0.1 Angs-1.

This model contains a detailed description of the Be filter, monochromator
and SANS set-up. The Be filter is in the monochromator block.

Example: mcrun RTP_SANS.instr lambda=5
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | monochromator selected wavelength | 5 |
| DM | Angs | d-spacing for the monochromator reflection | 3.355 |
| dlambda | Angs | monochromator wavelength spread | .2 |
| Be_Filter_depth | m | Depth of Be filter | .15 |
| Mono_tilt | deg | angle tilt between the 3 monochromator layers | -1 |
| mono_rotation | deg | additional monochromator offset rotation for e.g rocking curves | 0 |

## Links

- [Source code](RTP_SANS.instr) for `RTP_SANS.instr`.
- <a href="http://www.nuclearmalaysia.gov.my/Plant&Facilities/reactor.php">Nuclear Malaysia</a>
- M. Sufi et al., J. AppL Cryst. (1997). 30, 884-888 [doi:10.1107/S0021889897001738]

---
