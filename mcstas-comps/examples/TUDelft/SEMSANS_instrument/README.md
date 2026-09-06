# The `SEMSANS_instrument` Instrument

*McStas: SEMSANS-instrument*

## Identification

- **Site:** TUDelft
- **Author:** Morten Sales
- **Origin:** Copenhagen, Berlin, Delft
- **Date:** 2014

## Description

```text
SEMSANS instrument with 2 isosceles triangular field coils
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| triacoil_depth | m | Half of the triangular coil depth in z-direction (one right triangle) | 1.935000e-01 |
| triacoil_width | m | xwidth of triangular coil | 7.042824e-02 |
| triacoil_height | m | yheight of triangular coil | 3.000000e-01 |
| pol_zdepth | m | Depth of the polariser along z | 9.300000e-01 |
| vcoil_zdepth | m | Depth along z of the v-coil flipper | 1.500000e-01 |
| Guide1_depth | m | Depth of first guide/precession field section | 1.650000e-02 |
| Guide2_depth | m | Depth of second guide/precession field section | 3.315000e-01 |
| Guide3_depth | m | Depth of third guide/precession field section | 3.315000e-01 |
| Guide4_depth | m | Depth of fourth guide/precession field section | 5.650000e-02 |
| Bguide | T | Field strength of guide/precession field | -5.000000e-04 |
| Bextra | T | Field strength of extra field in guide field 1 to acheive echo | -2.120000e-02 |
| Bt2 | T | Field in first triangular coil | -4.440000e-03 |
| Bt1 | T | Field in second triangular coil | -2.560000e-03 |
| DLambda | AA | Wavelength spread from source (half of it) | 4.166366e+00 |
| Lambda | AA | Mean wavelength | 4.559500e+00 |
| flippos | m | Position (away from position set by vcoil_34_pos) of pi-flipper | 3.100000e-02 |
| FLIP | 1 | Choose polarisation direction using v-coil pi/2-flipper (1 is down, -1 is up) | 1.000000e+00 |
| chop1_pos | m | Position of first chopper | 5.000000e-01 |
| chop2_pos | m | Position of second chopper | 9.740000e-01 |
| pol_pos | m | Position of polariser | 1.907000e+00 |
| analyser_pos | m | Position of analyser | 6.072000e+00 |
| grating_w | m | Width of transmitting part of grating | 1.570000e-03 |
| grating_a | m | Width of absorbing part of grating | 2.930000e-03 |
| slit_1_pos | m | Position of first slit | 2.957000e+00 |
| slit_2_pos | m | Position of second slit | 5.437000e+00 |
| Guide1_pos | m | Position of first guide/precession field | 3.307000e+00 |
| Guide2_pos | m | Position of second guide/precession field | 3.710500e+00 |
| Guide3_pos | m | Position of third guide/precession field | 4.342000e+00 |
| Guide4_pos | m | Position of fourth guide/precession field | 5.060500e+00 |
| vcoil_12_pos | m | Position of first v-coil pair | 3.157000e+00 |
| vcoil_34_pos | m | Position of second v-coil pair | 4.192000e+00 |
| vcoil_56_pos | m | Position of third v-coil pair | 5.267000e+00 |
| triacoil_1_pos | m | Position of first triangular coil (centre) | 3.517000e+00 |
| triacoil_2_pos | m | Position of second triangular coil (centre) | 4.867000e+00 |
| grating_pos | m | Position of grating | 6.757000e+00 |
| detector_pos | m | Position of detector | 6.957000e+00 |
| tlow | mu-s | tmin of detector | 2.190523040779461e+03 |
| thigh | mu-s | tmax of detector | 1.214744595341338e+04 |

## Links

- [Source code](SEMSANS_instrument.instr) for `SEMSANS_instrument.instr`.

---
