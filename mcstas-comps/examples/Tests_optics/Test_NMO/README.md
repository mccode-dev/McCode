# The `Test_NMO` Instrument

*McStas: Implements a test instrument for the component FlatEllipse_finite_mirror.*

## Identification

- **Site:** Tests_optics
- **Author:** Christoph Herb, TUM
- **Origin:** TUM
- **Date:** 2022-2023

## Description

```text
Implements a test instrument for the component FlatEllipse_finite_mirror,
implementing Nested Mirror Optic (NMO) as suggested by B&ouml;ni et al.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| det_width | m | Detector width | 0.218 |
| source_width | m | Source width | 0.03 |
| source_height | m | Source height | 0.00000001 |
| lam_source | AA | Central source wavelength | 5 |
| dL | AA | Source wavelength spread | 0.5 |
| v_divergence | deg | Source vertical divergence | 2.5 |
| h_divergence | deg | Source horizontal divergence | 0.000001 |
| b0 | m | Parameter r0 of mirrors (distance to the mirror at lStart) | 0.2076 |
| mf | 1 | M-value of the inner side of the coating | 100 |
| mb | 1 | M-value of the outer side of the coating | 0 |
| mirror_width | m | Width of mirrors | 0 |
| focal_length | m | Mirror focal-length | 6 |
| mirrors | 1 | Number of NMO mirrors | 29 |
| mirror_sidelength | m | Side-length of mirrors | 0.1 |
| lStart | m | Parameter lStart of the mirrors | -0.6 |
| lEnd | m | Parameter lEnd of the mirrors | 0.6 |
| pixels | 1 | Number of detector pixels | 100 |
| flux | n/s/cm^2/st/AA | Source flux | 1 |
| det_width_focus | m | Detector width at focusing pt. | 0.03 |
| rs_at_zero_str | str | Filename to specify mirror rfront_inner_file (e.g. "rs_at_lstart.txt") | "NULL" |
| activate_mirror | 1 | Flag to activate mirror | 1 |

## Links

- [Source code](Test_NMO.instr) for `Test_NMO.instr`.
- P B&ouml;ni <a href="https://indico.psi.ch/event/13583/contributions/42618/attachments/24446/44119/Presentation_NFO-Workshop-PSI_final%20-%20Boeni.pdf">presentation at PSI NFO workshop</a>
- Christoph Herb et al., Nucl. Instrum. Meth. A 1040, 1671564 (1-18) 2022.

---
