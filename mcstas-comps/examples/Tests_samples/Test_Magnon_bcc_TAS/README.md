# The `Test_Magnon_bcc_TAS` Instrument

*McStas: Generic TAS instrument for test of samples with dispersions.*

## Identification

- **Site:** Tests_samples
- **Author:** Kim Lefmann (lefmann@nbi.ku.dk)
- **Origin:** UCPH
- **Date:** 22.07.2018

## Description

```text
Generic TAS instrument for test of samples with dispersions. Modeled over the RITA-2 instrument at PSI (one analyzer only). The instrument is able to position in q-space at q=(h 0 0) and fixed Ei and Ef. This can be used to longitudinal constant-E scans or constant-q scans. In addition, transverse constant-E scans can be made by scanning the sample orientation A3.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| hw | meV | Neutron energy transfer | 0.5 |
| Ef | meV | Final neutron energy | 5 |
| A3offset | deg | sample rotation around a vertical axis | 0 |
| A3auto | 1 | Flag for selecting automatic setting of A3 to have q-vector along [h 0 0] (0=manual; 1=auto) | 1 |
| aa | AA | lattice parameter for cubic system (passed on to the sample component) | 4.5 |
| qh | 1 | Length of the q-vector in r.l.u., sets the value of scattering angle A4. If A3auto=1, then A3 is set to q=[qh 0 0] | 1.1 |
| highres | 1 | Flag for setting unrealistic high resolution (used for fine testing) (0=standard TAS / RITA2; 1=high resolution TAS) | 0 |
| sample_J | meV | Nearest Neighbor magnetic interaction in sample | 0.2 |
| TT | K | temperature | 300 |
| FerroMagnet |  | Flag: 1 if ferromagnet, 0 if AFM | 0 |
| Verbose | 1 | Flag for printing of test statements from magnon component (0=silent, 1=print) | 0 |

## Links

- [Source code](Test_Magnon_bcc_TAS.instr) for `Test_Magnon_bcc_TAS.instr`.
- <reference/HTML link>

---
