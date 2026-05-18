# The `ESS_IN5_reprate` Instrument

*McStas: An IN5 type (cold chopper) multi-frame spectrometer at ESS LPTS*

## Identification

- **Site:** ESS
- **Author:** Kim Lefmann (kim.lefmann@risoe.dk), Helmuth Schober, Feri Mezei
- **Origin:** ESS
- **Date:** September 20, 2006

## Description

```text
McStas instrument for simulating IN5-TYPE (cold chopper) multi-frame spectrometer at ESS LPTS.
The sample is incoherent with an inelastic tunneling line.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lmin | AA | Minimum wavelength emitted by the moderator | 4.9 |
| Lmax | AA | Maximum wavelength emitted by the moderator | 5.1 |
| lambda0 | AA | Target wavelength for the monochromating crystal | 5 |
| Pulse_width | s | Length of the long pulse. Default to 2 ms. | 2.857e-3 |
| Num_pulses | 1 | Number of pulses to simulate. Default 1. | 1 |
| GUI_start | m | Distance from moderator to guide start | 2.0 |
| FO1_DIST | m | Distance from moderator to FO1 chopper | 6 |
| L_ballistic_begin | m | Length of expanding ballistic section in the beginning of the guide | 19.5 |
| L_ballistic_end | m | Length of compressing ballistic section in the end of the guide | 17 |
| Length | m | Total length of spectrometer (moderator to F2) | 100 |
| SAMPLE_DIST | m | Distance from F2 to sample | 1.2 |
| DETECTOR_DIST | m | Length sample-detector | 4 |
| GUI_h | m | Height of guide opening at entry | 0.105 |
| GUI_w | m | Width of guide opening at entry | 0.1 |
| GUI_GAP | m | Gap between guides due to chopper hardware | 0.05 |
| H1 | m | Height of guide elements | 0.167 |
| W1 | m | Width of guide elements | 0.116 |
| H2 | m | Height of guide elements | 0.185 |
| W2 | m | Width of guide elements | 0.15 |
| H3 | m | Height of guide elements | 0.19 |
| W3 | m | Width of guide elements | 0.15 |
| H4 | m | Height of guide elements | 0.213 |
| W4 | m | Width of guide elements | 0.14 |
| H_chop | m | Height at F2 chopper position | 0.075 |
| W_chop | m | Width at F2 chopper position | 0.03 |
| H_end | m | Height of guide opening at exit | 0.042 |
| W_end | m | Width of guide opening at exit | 0.0215 |
| ALPHA | m | Parameter for supermirrors in guides, Swiss Neutronics best at 3.4 | 3.4 |
| M | 1 | Parameter for supermirrors in guides, Swiss Neutronics best at 4 | 3.5 |
| F_slow1 | Hz | Frequency of the FO1 chopper | 16.6667 |
| F_slow2 | Hz | Frequency of the FO2 chopper | 0 |
| F_fast1 | Hz | Frequency of the F1 chopper | 0 |
| F_fast2 | Hz | Frequency of the F2 chopper | 200 |
| N_fast | 1 | Number of symmetric openings in the fast F choppers | 1 |
| SLOW1_THETA | deg | Opening angle of the FO1 chopper | 120 |
| FO3 | 1 | Inverse frequency of FO3 chopper (if 1, same as F2; if 2, half of F2, etc.) | 1 |
| THETA_fast1 | deg | Opening angle of the F1 chopper | 180 |
| FAST_THETA | deg | Opening angle of the F2 chopper | 5 |
| Gamma | meV | Width of sample quasielastic scattering (0=elastic) | 0 |
| Etun | meV | Tunneling energy | 1 |
| V_HOLE | m | Radius of hole in V sample (must be smaller than the outer radius: 0.01) | 0 |
| FRAC_QUASIEL | 1 | Fraction of the sample that scatters quasielastically | 0 |
| FRAC_TUNNEL |  | Fraction of the sample that scatters on the tunneling peaks | 0 |
| TT | deg | 2Theta (or a4) scattering angle | 50 |
| RES_DE | meV | Adjustable zoom for energy monitor at detector position | 0.5 |
| cold |  | Fraction of neutrons emitted from the cold moderatorx | 0.95 |

## Links

- [Source code](ESS_IN5_reprate.instr) for `ESS_IN5_reprate.instr`.
- The <a href="http://wwwold.ill.fr/Computing/links/meetings/ESS-LP/">ESS-Rencurel</a> website.
- <a href="http://www.ill.fr/in5">The IN5@ILL Yellow Pages</a>

---
