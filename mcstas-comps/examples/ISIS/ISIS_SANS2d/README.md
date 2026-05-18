# The `ISIS_SANS2d` Instrument

*McStas: This instrument models the ISIS TS2 SANS2d instrument.*

## Identification

- **Site:** ISIS
- **Author:** Richard Heenan with edits by Peter Willendrup
- **Origin:** ISIS, DTU Fysik
- **Date:** 2014

## Description

```text
This instrument models the ISIS TS2 SANS2d instrument up to the sample position.

From the author:
<p><b>IMPORTANT:</b> The instrument model does not currently include a correct moderator description
- and the results it produces have not been validated against experimental results.
<p>The actual bender is continuously curved, here use straight segments approx.with gravity.
1/7/10 L1=4m  increased gap between bender exit and S1 slit from .247 to .286m
At 4m we actually use 1.75 to 16.5 angstrom.
<p>Note "2m guides" are actually 1.985m long (new ones will be 1.981m) so "real gaps" are included below, losses are pro-rata the gap/guide lengths.
<p>Both rear & front detectors are 980mm square and offset 150 above beam (would only use 100mm with hindsight)
<p>Rear detector can be offset -300mm or +269mm sideways.
<p>Front detector is closest at approx 1.4m,  furthest at approx((read det position) - 1.6m) , offset  up to -1200mm sideways
there are 2 baffles between front & rear detector
<p>NIMROD (W5) spectrum might better than SANS2d (E2) spectrum for liquid H2 face incurent use.
Stu Ansell's McStas spectra for TS-2 actually have no angle variation included.
In reality spectra and backgrounds will vary, fast background is likely much higher on ZOOM(E1) than SANS2d (E2)
<p>RKH 2014
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L1 | m | Variable distance from 1st to 2nd variable slit | 3.926 |
| A1w | m | Width of first collimation slit, rectangular slit | 0.030 |
| A1h | m | Height of first collimation slit, rectangular slit | 0.02 |
| S6 | m | Radius of slit S6 (last of the optis slits) | 0.006 |
| A2 | m | Radius of second collimation slit, circular slit | 0.006 |
| Lmin | AA | Minimum wavelength to produce at the source | 1 |
| Lmax | AA | Maximum wavelength to produce at the source | 14 |

## Links

- [Source code](ISIS_SANS2d.instr) for `ISIS_SANS2d.instr`.

---
